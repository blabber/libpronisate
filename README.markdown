A small library to generate a bytestream as used by the [PRON-Protocol][1] from
arbitrary pictures.  Each byte represents the brightness of one pixel. It uses
the MagickWand API provided by ImageMagick.

Currently the makefiles are BSD-style makefiles using the FreeBSD make
infrastructure.

To build the libraries: `cd src && make`

To build the example: `cd example && make`

To run the example: `cd example && env LD_PRELOAD_PATH=../src ./example`

*This code is work in progress and will continue to evolve.*

[1]: http://raumzeitlabor.de/wiki/PRON-Protokoll "RZL1337 - PRON-Wall Protocol"
