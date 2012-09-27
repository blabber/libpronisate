A small library to generate a bytestream as used by the [PRON-Protocol][1] from
arbitrary pictures.  Each byte represents the brightness of one pixel. It uses
the MagickWand API provided by ImageMagick.

Currently the makefiles are BSD-style makefiles using the FreeBSD make
infrastructure.

The `example` binary has compiled in the relative runtime path `../src`.
Therefore it has to be called from the example subdirectory.

The example gif has been taken from [this][2] site.

[1]: http://raumzeitlabor.de/wiki/PRON-Protokoll "RZL1337 - PRON-Wall Protocol"
[2]: http://www.archive.org/details/blackandwhiteanimatedgifs "black and white animated gifs"
