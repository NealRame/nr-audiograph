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
#include <codec/AudioDecoder.h>
#include <codec/AudioMP3Decoder.h>
#include <codec/AudioOggVorbisDecoder.h>
#include <codec/AudioPCMDecoder.h>

#include <GraphBrush.h>
#include <GraphColor.h>
#include <GraphError.h>
#include <GraphLinearGradient.h>
#include <GraphPoint.h>
#include <GraphSize.h>
#include <GraphSurface.h>

#include "Parser.h"
#include "WaveFormGenerator.h"

#define MORE_DETAILS 	"Type « audiograph --help » for more details."
#define VERSION_STRING	"development"

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

void validate(boost::any &v, const std::vector<std::string> &values, Color *, int) {
	namespace po = boost::program_options;
	namespace parser = com::nealrame::parser;

	po::validators::check_first_occurrence(v);
	const std::string &s = po::validators::get_single_string(values);

	Color color;

	if (parser::parseColor(s, color)) {
		v = color;
	} else {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

void validate(boost::any &v, const std::vector<std::string> &values, Size *, int) {
	namespace po = boost::program_options;

	po::validators::check_first_occurrence(v);
	const std::string &s = po::validators::get_single_string(values);

	Size size;

	if (parser::parseSize(s, size)) {
		v = size;
	} else {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
} } } }

using namespace com::nealrame;

const graph::Color DefaultBackground = graph::Color(graph::Color::RGB{0,0,0},0);
const graph::Brush DefaultForeground = graph::Brush(graph::Color::Black);
const graph::Size  DefaultSize       = graph::Size(1024, 256);

namespace po = boost::program_options;

int main(int argc, char **argv) {	
	std::string input, output;
	std::shared_ptr<audio::Decoder> decoder = nullptr;

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

	po::options_description audio_options("Audio options");
	audio_options.add_options()
		(
			"input-audio-format,I",
			po::value<std::string>(),
			"set the input audio format"
		);

	po::options_description render_options("Render options");
	render_options.add_options()
		(
			"background,b",
			po::value<graph::Color>()->default_value(DefaultBackground),
			"set the background brush"
		) (
			"foreground,f", 
			po::value<graph::Brush>()->default_value(DefaultForeground),
			"set the foreground brush"
		) (
			"size,s",
			po::value<graph::Size>()->default_value(DefaultSize),
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
		.add( audio_options)
		.add(render_options)
		.add(hidden_options);

	po::options_description visible_options;
	visible_options
		.add(common_options)
		.add( audio_options)
		.add(render_options);

	po::positional_options_description args;
	args.add("input", 1);

	po::variables_map vm;

	try {
		po::store(po::command_line_parser(argc, argv)
			.options(cmdline_options)
			.positional(args).run(), vm);
		po::notify(vm);		
	} catch (std::exception &err) {
		std::cerr 
			<< err.what() << std::endl 
			<< MORE_DETAILS << std::endl;
		return 1;
	}

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
		std::cerr
			<< "Argument missing." << std::endl
			<< MORE_DETAILS << std::endl;
		return 1;
	}

	if (! vm.count("output")) {
		output = (boost::format("%1%.png") % boost::filesystem::path(input).stem().string()).str();
	}

	if (vm.count("input-audio-format")) {
		std::string audio_format = boost::to_lower_copy(vm["input-audio-format"].as<std::string>());
		if (audio_format == "mp3") {
			decoder.reset(new audio::MP3Decoder);
		} else
		if (audio_format == "oggvorbis") {
			decoder.reset(new audio::OggVorbisDecoder);
		} else 
		if (audio_format == "pcm") {
			decoder.reset(new audio::PCMDecoder);
		} else {
			std::cerr
				<< "Unsupported audio format: " << audio_format << std::endl
				<< MORE_DETAILS << std::endl;
			return 1;
		}
	} else {
		decoder.reset(audio::Decoder::getDecoder(input));
	}

	try {
		WaveFormGenerator wfgenerator;

		graph::Brush brush = vm["foreground"].as<graph::Brush>();
		graph::Size size = vm["size"].as<graph::Size>();

		if (brush.type() == graph::Brush::Type::Gradient) {
			graph::LinearGradient &gradient(static_cast<graph::LinearGradient &>(brush.gradient()));
			gradient.setStartPoint(graph::Point(0, 0));
			gradient.setEndPoint(graph::Point(0, size.height()));
		}

		wfgenerator.setSize(size);
		wfgenerator.setBackgroundColor(vm["background"].as<graph::Color>());
		wfgenerator.setBrush(brush);

		audio::Buffer *buffer = decoder->decode(input);
		graph::Surface *surface = wfgenerator.render(*buffer, 0, buffer->frameCount());

		surface->exportToPNG(output);

		delete buffer;
		delete surface;
	} catch (audio::Error &err) {
		std::cerr << (boost::format("Error while opening « %1% » !") % input).str() << std::endl;
		std::cerr << "Make sure that file exists, is readable and is a supported audio format." << std::endl;
		std::cerr << MORE_DETAILS << std::endl;
		return 0;
	} catch (graph::Error &err) {
	}

	return 0;
}
