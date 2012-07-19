LIB=		pronisate
SHLIB_MAJOR=	0
SRCS=		pronisate.c init.c deinit.c context_open.c context_close.c
SRCS+=		getters.c
WARNS?=		5

CFLAGS=		-I/usr/local/include/ImageMagick
LDFLAGS=	-L/usr/local/lib 
LDADD=		-lMagickWand -lMagickCore

NO_MAN=		YES

.include <bsd.lib.mk>
