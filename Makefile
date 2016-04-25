RM?=		rm -f
YACC=		yacc -d

NAME=		xoutputd
VERSION=	0.1

OBJS=		log.o parse.o scan.o xmalloc.o xoutputd.o

CPPFLAGS+=	-DVERSION=\"$(VERSION)\"
CPPFLAGS+=	-D_XOPEN_SOURCE=500
CPPFLAGS+=	-I/usr/X11R6/include
CFLAGS+=	-std=c99
CFLAGS+=	-Wall
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare
CFLAGS+=	-Wshadow
CFLAGS+=	-O2
# CFLAGS+=	-g3 -ggdb -fno-inline -O0
LDFLAGS+=	-L/usr/X11R6/lib
LDADD+=		-ll -ly -lX11 -lXrandr

all:		$(NAME)

$(NAME):	$(OBJS)
		$(CC) -o $@ $? $(LDFLAGS) $(LDADD)
		$(RM) y.tab.h

clean:
		$(RM) $(OBJS)

fclean:		clean
		$(RM) $(NAME)

re:		fclean all
