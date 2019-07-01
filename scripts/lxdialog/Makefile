HOSTCFLAGS += -DLOCALE 
LIBS = -lncurses

ifeq (/usr/include/ncurses/ncurses.h, $(wildcard /usr/include/ncurses/ncurses.h))
        HOSTCFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"
else
ifeq (/usr/include/ncurses/curses.h, $(wildcard /usr/include/ncurses/curses.h))
        HOSTCFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"
else
ifeq (/usr/include/ncurses.h, $(wildcard /usr/include/ncurses.h))
        HOSTCFLAGS += -DCURSES_LOC="<ncurses.h>"
else
	HOSTCFLAGS += -DCURSES_LOC="<curses.h>"
endif
endif
endif


OBJS = checklist.o menubox.o textbox.o yesno.o inputbox.o \
       util.o lxdialog.o msgbox.o

%.o: %.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

all: lxdialog

lxdialog: $(OBJS)
	$(HOSTCC) -o lxdialog $(OBJS) $(LIBS)

clean:
	rm -f core *.o *~ lxdialog
