nr-audiograph
=============

Audiograph is a command line tool aimed at producing waveform images from audio input files. It is possible to customize the generated waveform using some program options. It takes as input a Wav/PCM, a MP3 or an Ogg/Vorbis and outputs a PNG image. It will run exclusively on a GNU/Linux platform. 

Features :
----------

* runs on GNU/Linux platform,
* supports Wav/PCM, MP3 and Ogg/Vorbis audio files,
* customizable output using colors and gradients.

Installation :
--------------

### Ubuntu 
```
> apt-get install build-essential git-core libboost-all-dev libcairo2-dev libogg-dev libmp3lame-dev libvorbis-dev 
> git clone https://github.com/NealRame/nr-audiograph.git
> cd nr-audiograph
> git submodule init
> git submodule update
> make depends
> make Release
```

Usage :
-------
```
Usage: audiograph [options] FILE

Common options:
  -h [ --help ]         print this help and exit
  -o [ --output ] arg   set output file
  -v [ --version ]      print version string and exit

Audio options:
  -I [ --input-audio-format ] arg set the input audio format

Render options:
  -b [ --background ] arg (=rgba(0,0,0,0))
                                        set the background brush
  -f [ --foreground ] arg (=#000)       set the foreground brush
  -s [ --size ] arg (=1024x256)         set the waveform bounding box
```
