/*
 * Parser.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: jux
 */

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>
#include <locale>

#include "Parser.h"

#include "GraphColor.h"
#include "GraphGradient.h"
#include "GraphSize.h"

namespace com {
namespace nealrame {
namespace parser {

std::string optionName(const std::string &s) {
	boost::cmatch m;
	boost::regex re("^-([a-zA-Z0-9][a-zA-Z0-9_\\-]+)(?:=.+)?$");
	if (boost::regex_match(s.data(), m, re)) {
		return m[1];
	}
	return "";
}

std::string optionValue(const std::string &s) {
	boost::cmatch m;
	boost::regex re("^-[a-zA-Z0-9][a-zA-Z0-9_\\-]+=(.+)$");
	if (boost::regex_match(s.data(), m, re)) {
		return m[1];
	}
	return "";
}

static const char * Base10DigitClass = "[0123456789]";
static const char * Base16DigitClass = "[0123456789aAbBcCdDeEfF]";

boost::shared_ptr<graph::Size> readSize(const std::string &s) {
	const static boost::regex SizeRegex((boost::format("(%1%+)x(%1%+)") % Base10DigitClass).str());
	boost::cmatch m;
	if (! boost::regex_match(s.data(), m, SizeRegex)) {
		return boost::shared_ptr<graph::Size>();
	}
	return boost::shared_ptr<graph::Size>(new graph::Size(boost::lexical_cast<double>(m[1]), boost::lexical_cast<double>(m[2])));
}


double fromHtmlRGBComponentString(const std::string &s) {
	std::stringstream ss;
	unsigned int v;
	ss << std::hex << s;
	ss >> v;
	return static_cast<double>(v)/255;
}

boost::shared_ptr<graph::Color> readColor(const std::string &s) {
	const static std::string decimal((boost::format("\\s*(?:(?:%1%+(?:\\.%1%*)?)|(?:\\.%1%+))\\s*") % Base10DigitClass).str());

	const static boost::regex HTMLLongForm((boost::format("^#(%1%{2})(%1%{2})(%1%{2})$") % Base16DigitClass).str());
	const static boost::regex HTMLShortForm((boost::format("^#(%1%)(%1%)(%1%)$") % Base16DigitClass).str());
	const static boost::regex RGBForm((boost::format("^rgb\\((%1%),(%1%),(%1%)\\)$") % decimal).str());
	const static boost::regex RGBAForm((boost::format("^rgba\\((%1%),(%1%),(%1%),(%1%)\\)$") % decimal).str());

	boost::cmatch m;
	graph::Color::RGB rgb;

	if (boost::regex_match(s.data(), m, HTMLLongForm)) {
		rgb.red = fromHtmlRGBComponentString(m[1]);
		rgb.green = fromHtmlRGBComponentString(m[2]);
		rgb.blue = fromHtmlRGBComponentString(m[3]);
		return boost::shared_ptr<graph::Color>(new graph::Color(rgb));
	}

	if (boost::regex_match(s.data(), m, HTMLShortForm)) {
		rgb.red = fromHtmlRGBComponentString((boost::format("%1%%1%") % m[1]).str());
		rgb.green = fromHtmlRGBComponentString((boost::format("%1%%1%") % m[2]).str());
		rgb.blue = fromHtmlRGBComponentString((boost::format("%1%%1%") % m[3]).str());
		return boost::shared_ptr<graph::Color>(new graph::Color(rgb));
	}

	if (boost::regex_match(s.data(), m, RGBForm)) {
		rgb.red = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[1])));
		rgb.green = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[2])));
		rgb.blue = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[3])));
		if (rgb.red <= 1 && rgb.green <= 1 && rgb.blue <= 1) {
			return boost::shared_ptr<graph::Color>(new graph::Color(rgb));
		}
	}

	if (boost::regex_match(s.data(), m, RGBAForm)) {
		rgb.red = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[1])));
		rgb.green = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[2])));
		rgb.blue = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[3])));
		double alpha = boost::lexical_cast<double>(boost::algorithm::trim_copy(std::string(m[4])));
		if (rgb.red <= 1 && rgb.green <= 1 && rgb.blue <= 1 && alpha <= 1) {
			return boost::shared_ptr<graph::Color>(new graph::Color(rgb, alpha));
		}
	}

	return boost::shared_ptr<graph::Color>();
}

boost::shared_ptr<graph::Gradient> readGradient(const std::string &) {
	const static std::string integer((boost::format("\\s*%1%+\\s*") % Base10DigitClass).str());
	const static std::string decimal((boost::format("\\s*(?:(?:%1%+(?:\\.%1%*)?)|(?:\\.%1%+))\\s*") % Base10DigitClass).str());
	const static std::string html_color((boost::format("#(?:(?:(?:%1%{2}){3})|(?:%1%{3}))") % Base16DigitClass).str());
	const static std::string rgb_color((boost::format("rgb\\(%1%,%1%,%1%\\)") % decimal).str());
	const static std::string rgba_color((boost::format("rgba\\(%1%,%1%,%1%,%1%\\)") % decimal).str());
	const static std::string color((boost::format("(?:%1%)|(?:%2%)|(?:%3%)")
										% html_color % rgb_color % rgba_color).str());
	const static std::string stop((boost::format("%1%\\s+%2%%") % color % integer).str());
	const static std::string gradient((boost::format("gradient\\((%1%)(?:,(%1%))\\)") % stop).str());

	std::cerr << gradient << std::endl;

	return boost::shared_ptr<graph::Gradient>();
}

boost::shared_ptr<graph::Brush> readBrush(const std::string &s) {
	return boost::shared_ptr<graph::Brush>();
}

} /* namespace parser */
} /* namespace nealrame */
} /* namespace com */
