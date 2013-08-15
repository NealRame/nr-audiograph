/*
 * Parser.h
 *
 *  Created on: Jun 22, 2013
 *      Author: jux
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <string>

namespace com {
namespace nealrame {

namespace graph {
class Brush;
class Color;
class Gradient;
class Size;
}

namespace parser {

bool parseColor(const std::string &s, com::nealrame::graph::Color &);
bool parseBrush(const std::string &s, com::nealrame::graph::Brush &);
bool parseSize (const std::string &s, com::nealrame::graph::Size  &);

} /* namespace parser */
} /* namespace nealrame */
} /* namespace com */
#endif /* PARSER_H_ */
