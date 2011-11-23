LIB=		pronisate
SHLIB_MAJOR=	0
SRCS=		pronisate.c
WARNS?=		5

CFLAGS=		-I/usr/local/include/ImageMagick
LDFLAGS=	-L/usr/local/lib 
LDADD=		-lMagickWand -lMagickCore

NO_MAN=		YES

.include <bsd.lib.mk>
