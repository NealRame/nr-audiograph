/*
 * main.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: jux
 */

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "AudioBuffer.h"
#include "AudioError.h"
#include "codec/AudioMP3Decoder.h"
#include "codec/AudioPCMDecoder.h"
#include "codec/AudioPCMCoder.h"

#include "GraphBrush.h"
#include "GraphError.h"
#include "GraphSurface.h"

#include "Parser.h"
#include "WaveFormGenerator.h"

using namespace com::nealrame;

std::string lower(const std::string &s) {
	std::string copy(s);
	std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
	return copy;
}

std::ostream & usage(std::ostream &out) {

	out << "usage: [OPTIONS] {FILE}"
		"\n  Display options:"
		"\n    -bg|-background-color=COLOR : Specify the background color. Default color is white."
		"\n    -fg|-foreground-color=COLOR|GRADIENT : Specify the foreground color. Default color is black."
		"\n       COLOR                        ::= RED,GREEN,BLUE[,ALPHA]"
		"\n       GRADIENT                     ::= STOP;STOP{;STOP}"
		"\n       STOP                         ::= POS:COLOR"
		"\n       POS, RED, GREEN, BLUE, ALPHA ::= real in [0..1]"
		"\n    -size=SIZE : Specify the size of the image"
		"\n       SIZE is WIDHTxHEIGHT, WIDTH and HEIGHT are positive integers."
		"\n       Default value is 1024x256."
		"\n"
		"\n  Common options:"
		"\n    -help : Print this help and exit"
		"\n    -output=FILE : Specify the output filename";

	return out;
}

//bool match(const std::string &a, const std::string &b) {
//	return a.size() < b.size() ? false : (a.substr(0, b.size()) == b);
//}
//
//std::string option_value(const std::string &option) {
//	int index  = option.find("=");
//	return index > 0 ? option.substr(index + 1) : "";
//}

int main(int argc, char **argv) {
	WaveFormGenerator wfgenerator;
	std::string input_filename, output_filename;

	try {
		std::vector<std::string> arguments;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			std::string opt_name = parser::optionName(arg);
			std::string opt_value = parser::optionValue(arg);

			if (opt_name.size() > 0) {
				if (opt_name == "help") {
					std::cout << usage << std::endl;
					return 0;
				} else
				if (opt_name == "background-color" || opt_name == "bg") {
					if (opt_value.size() == 0) {
						parser::Error::raise("-background-color: value is missing.");
					}
					boost::shared_ptr<graph::Color> color = parser::readColor(opt_value);
					if (! color) {
						parser::Error::raise("-background-color: bad value.");
					}
					wfgenerator.setBackgroundColor(*color);
				} else
				if (opt_name == "foreground-color" || opt_name == "fg") {
					if (opt_value.size() == 0) {
						parser::Error::raise("-foreground-color: value is missing.");
					}
					boost::shared_ptr<graph::Color> color = parser::readColor(opt_value);
					if (! color) {
						parser::Error::raise("-foreground-color: bad value.");
					}
					wfgenerator.setBrush(*color);
				} else
				if (opt_name == "-output") {
					if (opt_value.size() == 0) {
						parser::Error::raise("-output: value is missing.");
					}
					output_filename = opt_value;
				} else
				if (opt_name == "-size") {
					if (opt_value.size() == 0) {
						parser::Error::raise("-size: value is missing.");
					}
					boost::shared_ptr<graph::Size> size = parser::readSize(opt_value);
					if (! size) {
						parser::Error::raise("-size: bad value.");
					}
					wfgenerator.setSize(*size);
				} else {
					parser::Error::raise((boost::format("\'%1%\'= invalid option.") % arg).str());
				}
			} else {
				arguments.push_back(arg);
			}
		}

		if (arguments.size() < 1) {
			parser::Error::raise("Wrong number of argument.");
		}

		input_filename = arguments.front();

		if (output_filename.size() == 0) {
			output_filename = (boost::format("%1%.png")
									% boost::filesystem::path(input_filename).stem().string()).str();
		}
	} catch (parser::Error &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << "Type audiograph -help for more details." << std::endl;
		return 1;
	}

	try {
		audio::Buffer *buffer = audio::Decoder::getDecoder(input_filename)->decode(input_filename);
		graph::Surface *surface = wfgenerator.render(*buffer, 0, buffer->frameCount());

		surface->exportToPNG(output_filename);

		delete buffer;
		delete surface;
	} catch (audio::Error &err) {
		switch (err.status) {
		case audio::Error::Status::IOError:
			std::cerr
				<<
					"IO error, please verify:"
					" - that you have permission to read from input file, "
					" - that you have permission to write to output file,"
					" - and that there is enough disk space available."
				<< std::endl;
			break;

		case audio::Error::Status::NoSuitableDecoder:
			std::cerr
				<<
					"There is no suitable codec available to decode the given input file."
				<< std::endl;
			break;

		default:
			std::cerr
				<<
					"Exception caught: " << audio::Error::statusToString(err.status)
				<< std::endl;
			break;
		}
		return 1;
	} catch (graph::Error &err) {
		std::cerr
			<<
				"Exception caught: " << graph::Error::statusToString(err.status)
			<< std::endl;
		return 1;
	}

	return 0;
}
