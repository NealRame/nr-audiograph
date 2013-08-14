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

#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <AudioBuffer.h>
#include <AudioError.h>
#include <codec/AudioMP3Decoder.h>
#include <codec/AudioPCMDecoder.h>
#include <codec/AudioPCMCoder.h>

#include <GraphBrush.h>
#include <GraphColor.h>
#include <GraphError.h>
#include <GraphSize.h>
#include <GraphSurface.h>

#include "Parser.h"
#include "WaveFormGenerator.h"

#define VERSION_STRING "development"

namespace com {
namespace nealrame {
namespace graph {
void validate(boost::any &v, const std::vector<std::string> &values, Brush *, int) {
	namespace po = boost::program_options;
	namespace parser = com::nealrame::parser;

	po::validators::check_first_occurrence(v);
	const std::string &s = po::validators::get_single_string(values);

	Brush brush;

	if (parser::parseBrush(s, brush)) {
		v = brush;
	} else {
	        throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

void validate(boost::any &v, const std::vector<std::string> &values, Size *, int) {
	namespace po = boost::program_options;
	po::validators::check_first_occurrence(v);
	po::validators::get_single_string(values);
	// const std::string &s = po::validators::get_single_string(values);
	v = boost::any(Size(1024, 256));
} } } }

using namespace com::nealrame;

const graph::Brush DefaultBackground = graph::Brush(graph::Color::Black);
const graph::Brush DefaultForeground = graph::Brush(graph::Color::White);
const graph::Size  DefaultSize       = graph::Size(1024, 256);

namespace po = boost::program_options;

int main(int argc, char **argv) {
	std::string input, output;

	po::options_description common_options("Common options");
	common_options.add_options()
		(
			"help,h", 
			"print this help and exit"
		) (
			"output,o", 
			po::value<std::string>(&output), 
			"set output file"
		) (
			"version,v", 
			"print version string and exit"
		);

	po::options_description render_options("Render options");
	render_options.add_options()
		(
			"background,b", 
			po::value<graph::Brush>()->default_value(DefaultBackground, "#000"),
			"set the background brush"
		) (
			"foreground,f", 
			po::value<graph::Brush>()->default_value(DefaultForeground, "#fff"),
			"set the foreground brush"
		) (
			"size,s",
			po::value<graph::Size>()->default_value(DefaultSize, "1024x256"),
			"set the waveform bounding box"
		);

	po::options_description hidden_options("Hidden options");
	hidden_options.add_options()
		(
			"input,i",
			po::value<std::string>(&input), 
			"set input file"
		);

	po::options_description cmdline_options;
	cmdline_options
		.add(common_options)
		.add(render_options)
		.add(hidden_options);

	po::options_description visible_options;
	visible_options
		.add(common_options)
		.add(render_options);

	po::positional_options_description args;
	args.add("input", 1);

	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv)
			.options(cmdline_options)
			.positional(args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << (boost::format("Usage: %1% [options] file.[mp3|ogg|wav]")
				% boost::filesystem::basename(argv[0])).str() << std::endl;
		std::cout << visible_options << std::endl;
		return 0;
	}

	if (vm.count("version")) {
		std::cout << (boost::format("Version: %1%") 
				% VERSION_STRING).str() << std::endl;
		return 0;
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
