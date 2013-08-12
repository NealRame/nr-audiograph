/*
 * Parser.h
 *
 *  Created on: Jun 22, 2013
 *      Author: jux
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <exception>
#include <string>
#include <boost/shared_ptr.hpp>

namespace com {
namespace nealrame {

namespace graph {
class Brush;
class Color;
class Gradient;
class Size;
}

namespace parser {

class Error : public std::exception {
public:
	static void raise(std::string m) {
		throw Error(m);
	}
public:
	Error() throw() { }
	Error(std::string m) throw() : message(m) { }
	~Error() throw() { }
	virtual const char * what() const throw() { return message.data(); }
	std::string message;
};

std::string optionName(const std::string &);
std::string optionValue(const std::string &);

boost::shared_ptr<graph::Size> readSize(const std::string &);
boost::shared_ptr<graph::Color> readColor(const std::string &);
boost::shared_ptr<graph::Gradient> readGradient(const std::string &);
boost::shared_ptr<graph::Brush> readBrush(const std::string &s);

} /* namespace parser */
} /* namespace nealrame */
} /* namespace com */
#endif /* PARSER_H_ */
