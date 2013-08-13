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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <AudioBuffer.h>
#include <AudioError.h>
#include <codec/AudioMP3Decoder.h>
#include <codec/AudioPCMDecoder.h>
#include <codec/AudioPCMCoder.h>

#include <GraphBrush.h>
#include <GraphError.h>
#include <GraphSurface.h>

#include "Parser.h"
#include "WaveFormGenerator.h"

using namespace com::nealrame;

namespace po = boost::program_options;

int main(int argc, char **argv) {
	std::string input, output;

	po::options_description options("Allowed options");
	options.add_options()
		("help,h", "print this help and exit")
		("output,o", po::value<std::string>(&output), "set output file")
		("input,i", po::value<std::string>(&input), "set input file");

	po::positional_options_description args;
	args.add("input", 1);

	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv).options(options).positional(args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << (boost::format("Usage: %1% [options] file.[mp3|ogg|wav]")
				% boost::filesystem::basename(argv[0])).str() << std::endl;
		std::cout << options << std::endl;
		return 1;
	}

	if (! vm.count("input")) {
		std::cerr << (boost::format("Argument missing.\nType « %1% --help » for more details.") 
				% boost::filesystem::basename(argv[0])).str() << std::endl;
		return 1;
	}

	if (! vm.count("output")) {
		boost::filesystem::path path(input);
		output = (path.has_parent_path() ?
				(boost::format("%1%/%2%.png") % path.parent_path().string() % path.stem().string())
					: (boost::format("%1%.png") % path.stem().string())).str();
	}

	std::cout << "input: " << input << std::endl;
	std::cout << "output: " << output << std::endl;

	return 0;
}
