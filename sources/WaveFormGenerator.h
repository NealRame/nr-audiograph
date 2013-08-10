/*
 * WaveFormGenerator.h
 *
 *  Created on: Jun 21, 2013
 *      Author: jux
 */

#ifndef WAVEFORMGENERATOR_H_
#define WAVEFORMGENERATOR_H_

namespace com {
namespace nealrame {

namespace graph {
class Brush;
class Color;
class Size;
class Surface;
};

namespace audio {
class Buffer;
};

class WaveFormGenerator {
public:
	WaveFormGenerator();
	virtual ~WaveFormGenerator();

public:
	void setSize(const graph::Size &);
	void setBackgroundColor(const graph::Color &);
	void setBrush(const graph::Brush &);

public:
	graph::Surface * render(const audio::Buffer &, unsigned int offset, unsigned int count) const;

public:
	void *_priv_data;
};

} /* namespace nealrame */
} /* namespace com */
#endif /* WAVEFORMGENERATOR_H_ */
