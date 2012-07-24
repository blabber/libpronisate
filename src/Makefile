LIB=		pronisate
SHLIB_MAJOR=	0
SRCS=		pronisate.c init.c deinit.c context_open.c context_close.c
SRCS+=		getters.c
WARNS?=		5

CFLAGS=		-I/usr/local/include/ImageMagick
LDFLAGS=	-L/usr/local/lib 
LDADD=		-lMagickWand -lMagickCore

MAN3=		libpronisate.3

.include <bsd.lib.mk>

all:		README

COL_CMD=		/usr/bin/col -bx
NROFF_CMD=		/usr/bin/nroff -c -mandoc

README:		${MAN3}
	${NROFF_CMD} ${MAN3} | ${COL_CMD} > ${.TARGET}
