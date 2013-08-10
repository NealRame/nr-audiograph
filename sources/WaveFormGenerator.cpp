/*
 * WaveFormGenerator.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: jux
 */

#include <iostream>

#include "WaveFormGenerator.h"

#include "AudioBuffer.h"

#include "GraphColor.h"
#include "GraphSize.h"
#include "GraphBrush.h"
#include "GraphPainter.h"
#include "GraphPoint.h"
#include "GraphSurface.h"
#include "GraphRectangle.h"

namespace com {
namespace nealrame {

struct Data {
	graph::Brush brush;
	graph::Color color;
	graph::Size size;
};

WaveFormGenerator::WaveFormGenerator() : _priv_data(nullptr) {
	Data *data = new Data;
	data->color = graph::Color::White;
	data->brush = graph::Color::Black;
	data->size  = graph::Size(1024, 256);
	_priv_data = data;
}

WaveFormGenerator::~WaveFormGenerator() {
	delete static_cast<Data *>(_priv_data);
}

void WaveFormGenerator::setSize(const graph::Size &size) {
	static_cast<Data *>(_priv_data)->size = size;
}

void WaveFormGenerator::setBackgroundColor(const graph::Color &color) {
	static_cast<Data *>(_priv_data)->color = color;
}

void WaveFormGenerator::setBrush(const graph::Brush &brush) {
	static_cast<Data *>(_priv_data)->brush = brush;
}

graph::Surface * WaveFormGenerator::render(const audio::Buffer &buffer, uint offset, uint count) const {
	Data *data = static_cast<Data *>(_priv_data);

	unsigned int frame_count = buffer.frameCount();

	if (offset + count > frame_count) {
		return nullptr; // throw an exception here
	}

	graph::Surface *surface = new graph::Surface(data->size, graph::Surface::Backend::Image);
	graph::Painter painter(*surface);

	painter.setAntialiasing(graph::Painter::Antialias::None);
	painter.clear(data->color);
	painter.setBrush(data->brush);

	double step = data->size.width()/count, h = data->size.height(), x = 0, p = 0;
	float *samples;
	float min = 0, max = 0;
	unsigned int channel_count = buffer.format().channelCount;

	samples = new float[channel_count*1024];

	while (count > 0) {
		unsigned int n = count > 1024 ? 1024 : count;

		buffer.read(offset, n, samples);

		for (unsigned int i = 0; i < n; ++i) {
			if (x - p < 1) {
				float s = 0;
				for (unsigned int j = 0; j < channel_count; ++j) {
					s += samples[i*channel_count + j];
				}
				s = s / channel_count;
				min = std::min(s, min);
				max = std::max(s, max);
			} else {
				graph::Rectangle rect = (graph::Rectangle(graph::Point(p, h*(max + 1)/2), graph::Point(x, h/2))
											| graph::Rectangle(graph::Point(p, h/2), graph::Point(x, h*(min + 1)/2))).normalized();

#ifdef DEBUG
				std::cerr << min << ":" << max << std::endl;
				std::cerr << "(" << rect.left() << "," << rect.top() << ") " << rect.size().width() << "x" << rect.size().height() << std::endl;
#endif
				painter.fillRectangle(graph::Rectangle(graph::Point(p, h*(max + 1)/2), graph::Point(x, h/2)).normalized());
				painter.fillRectangle(graph::Rectangle(graph::Point(p, h/2), graph::Point(x, h*(min + 1)/2)).normalized());

//				painter.fillRectangle(rect);

				p = x;
				min = max = 0;
			}

			x += step;
		}

		count -= n;
		offset += n;
	}

	return surface;
}

} /* namespace nealrame */
} /* namespace com */
