audiograph
==========

Audiograph is a command line tool aimed at producing waveform images from audio input files. It is possible to customize the generated waveform using some program options. It takes as input a Wav/PCM, a MP3 or an Ogg/Vorbis and outputs a PNG image. It will run exclusively on a GNU/Linux platform. 

Features
--------

* runs on GNU/Linux platform,
* supports Wav/PCM, MP3 and Ogg/Vorbis audio files,
* customizable output using colors and gradients.

Installation
------------

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
### Fedora

Usage
-----

### Synopsis
`audiograph [OPTIONS] FILE`

### Options

#### Common options :

* **`-h,--help`** 
  
  Print usage on standard input and exit.

* **`-o FILENAME,--output=FILENAME`**
  
  Set output filename. Default name of the output file will be determined as the base name of the input file with the extension replaced by *.png*.

* **`-v,--version`**
  
  Print the command version string and exit.

#### Audio options :

* **`-I FORMAT,--input-audio-format=FORMAT`**
  
  Set the input audio format. Format must be one of the following values : **mp3**, **oggvorbis** or **pcm**.

  When the option is not passed on the command line, the format of the input file is assumed to be :
    + WAV/PCM when input file name extension matches *.wav*, 
    + MP3 when input file name extension matches *.mp3*,
    + Ogg/Vorbis when input file name extension matches *.ogg*.

#### Render options :

* **`-b COLOR,--background=COLOR`**
  
  Set the background brush. By default the background color will be rgba(0, 0, 0, 0).
  
  *See below for COLOR syntax.*

* **`-f BRUSH,--foreground=BRUSH`**
  
  Set the foreground brush. By default the foreground brush will be solid color #000.
  
  *See below for BRUSH syntax.*

* **`-s SIZE,--size=SIZE`**
  
  Define the size of the waveform. By default the size of the waveform will be 1024 pixels wide by 256 pixels high.
  
  *See below for SIZE syntax.*

#### Syntax

```
SIZE          ::= integerxinteger
COLOR         ::= #XXX | #XXXXXX | rgb(decimal,decimal,decimal) | rgba(decimal,decimal,decimal,decimal)
GRADIENT      ::= gradient({GRADIENT_STOP})
GRADIENT_STOP ::= COLOR decimal
```

*decimal numbers (for colors component and GRADIENT_STOP) must belong to the interval [0,1].*

Gradients are vertical gradients. The decimal value after the color determines the color of a relative position. The absolute position is obtained by the product of the given decimal value by the height of the wave form.

### Examples

* **`> audiograph "Boards of Canada - Music Is Math.mp3"`**

  ![Boards of Canada - Geogaddi - Music is Math](http://nealrame.github.io/nr-audiograph/MusicIsMath_1024x256.png)
  
* **`> audiograph -s 1024x128 "Boards of Canada - Music Is Math.mp3"`**
  
  ![Boards of Canada - Geogaddi - Music is Math](http://nealrame.github.io/nr-audiograph/MusicIsMath_1024x128.png)

* **`> audiograph -f "#f03a11" "Skinny Puppy - Reclamation.ogg"`**
  
  ![Skinny Puppy - Too Dark Park - Reclamation](http://nealrame.github.io/nr-audiograph/Reclamation.png)

* **`> audiograph -s 1024x128 -f "gradient(#444 0, #333 0.25, #222 0.5, #333 0.75, #444 1)" "Tool - Schism.ogg"`**
  
  ![Tool - Lateralus - Schism](http://nealrame.github.io/nr-audiograph/Schism.png)
