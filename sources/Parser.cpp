/*
 * Parser.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: jux
 */

#include <limits>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/format.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/boost_tuple.hpp>

#include <GraphBrush.h>
#include <GraphColor.h>
#include <GraphLinearGradient.h>
#include <GraphSize.h>

#include "Parser.h"

namespace com {
namespace nealrame {
namespace parser {

#	define BYTE_TO_DOUBLE(VALUE) \
		static_cast<double>((unsigned char)(VALUE))/std::numeric_limits<unsigned char>::max()

	using namespace boost::spirit;

	typedef boost::tuple<double, double, double, double> RGBA_Attr;
	typedef boost::tuple<RGBA_Attr, double> GradientStop_Attr;
	typedef std::vector<GradientStop_Attr> Gradient_Attr;

	com::nealrame::graph::Color rgba_attr_to_color(RGBA_Attr &v) {
		com::nealrame::graph::Color::RGB rgb = { v.get<0>(), v.get<1>(), v.get<2>() };
		return graph::Color(rgb, v.get<3>());
	}

	com::nealrame::graph::LinearGradient gradient_attr_to_gradient(Gradient_Attr &v) {
		com::nealrame::graph::LinearGradient gradient;

		for (const GradientStop_Attr &stop_attr : v) {
			RGBA_Attr rgba_attr = stop_attr.get<0>();
			double offset = stop_attr.get<1>();
			gradient.addColorStop(offset, rgba_attr_to_color(rgba_attr));
		}

		return gradient;
	}

	struct hex1_ : qi::symbols<char, double> {
		hex1_() {
			for (unsigned int i=0; i<=0x0f; ++i) {
				add((boost::format("%1x") % i).str(),
					BYTE_TO_DOUBLE((i << 4) + i));
			}
		}
	} hex1;

	struct hex2_ : qi::symbols<char, double> {
		hex2_() {
			for (unsigned int i=0; i<=0xff; ++i) {
				add((boost::format("%02x") % i).str(),
					BYTE_TO_DOUBLE(i));
			}
		}
	} hex2;

	template <typename Iterator>
	struct ColorGrammar : 
		qi::grammar<Iterator, RGBA_Attr(), ascii::space_type>
	{
		static bool parse(Iterator &first, Iterator last, com::nealrame::graph::Brush &brush) {
			RGBA_Attr value;
			ColorGrammar<Iterator> grammar;
			if (! qi::phrase_parse(first, last, grammar, ascii::space, value)) {
				// find a solution to correctly report an error
				return false;
			}
			brush.setColor(rgba_attr_to_color(value));
			return true;
		}

		ColorGrammar() : 
			ColorGrammar::base_type(color, "color") {
			using qi::_val;
			using qi::attr;
			using qi::lexeme;
			using qi::lit;
			using qi::no_case;
			using boost::phoenix::at_c;
			using boost::phoenix::construct;
			using boost::phoenix::val;
			using boost::phoenix::ref;

			color.name("color");
			color %= hex_expr | rgb_expr | rgba_expr;

			hex_expr.name("hexadecimal color constant");
			hex_expr %= // hex_long_expr must be check before hex_short_expr
				hex_long_expr | hex_short_expr;

			hex_long_expr.name("hexadecimal color constant (long form)");
			hex_long_expr %=
				no_case[
					lexeme[ lit('#')
						>> (hex2)
						>> (hex2)
						>> (hex2)
						>> attr(1.0)
					]
				];

			hex_short_expr.name("hexadecimal color constant (short form)");
			hex_short_expr %=
				no_case[
					lexeme[ lit('#')
						>> hex1
						>> hex1
						>> hex1
						>> attr(1.0)
					]
				];

			rgb_expr.name("RGB color");
			rgb_expr %=
				no_case[
					lit("rgb") >> lit('(')
					>> double_ [check_RGBA_component]
					>> lit(',')
					>> double_ [check_RGBA_component]
					>> lit(',')
					>> double_ [check_RGBA_component]
					>> lit(')')
					>> attr(1.0)
				];

			rgba_expr.name("RGBA color");
			rgba_expr %=
				no_case[
					lit("rgba") >> lit('(')
					>> double_ [check_RGBA_component]
					>> lit(',')
					>> double_ [check_RGBA_component]
					>> lit(',')
					>> double_ [check_RGBA_component]
					>> lit(',')
					>> double_ [check_RGBA_component]
					>> lit(')')
				];
		}
		
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> color;
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> hex_expr;
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> hex_long_expr;
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> hex_short_expr;
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> rgb_expr;
		qi::rule<Iterator, RGBA_Attr(), ascii::space_type> rgba_expr;

	private:
		static void check_RGBA_component(
			double const &v, qi::unused_type, bool &ok) {
			ok = (v >= 0 && v <= 1);
		}
	};

	template <typename Iterator>
	struct GradientGrammar :
		qi::grammar<Iterator, Gradient_Attr(), ascii::space_type>
	{
		static bool parse(Iterator &first, Iterator last, com::nealrame::graph::Brush &brush) {
			Gradient_Attr value;
			GradientGrammar<Iterator> grammar;
			if (! qi::phrase_parse(first, last, grammar, ascii::space, value)) {
				// find a solution to correctly report an error
				return false;
			}
			brush.setGradient(gradient_attr_to_gradient(value));
			return true;
		}

		GradientGrammar() :
			GradientGrammar::base_type(gradient, "gradient") {
			using qi::lexeme;
			using qi::lit;
			using qi::no_case;

			gradient %=
				no_case[
					lit("gradient") 
					>> lit('(') 
					>> gradient_stop % lit(',') 
					>> lit(')')
				];

			gradient_stop %=
				color_grammar.color >> double_[check_stop_offset];

		}

		ColorGrammar<Iterator> color_grammar;
		qi::rule<Iterator, Gradient_Attr(), ascii::space_type> gradient;
		qi::rule<Iterator, GradientStop_Attr(), ascii::space_type> gradient_stop;

	private:
		static void check_stop_offset(
			double const &v, qi::unused_type, bool &ok) {
			ok = (v >= 0 && v <= 1);
		}
	};

	bool parseColor(const std::string &s, com::nealrame::graph::Brush &brush) {
		std::string::const_iterator it = s.begin(), end = s.end();
		return ColorGrammar<std::string::const_iterator>::parse(it, end, brush);
	}

	bool parseBrush(const std::string &s, com::nealrame::graph::Brush &brush) {
		if (! parseColor(s, brush)) {
			std::string::const_iterator it = s.begin(), end = s.end();
			return GradientGrammar<std::string::const_iterator>::parse(it, end, brush);
		}
		return true;
	}

} /* namespace parser */
} /* namespace nealrame */
} /* namespace com */
