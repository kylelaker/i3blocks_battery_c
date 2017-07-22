EXE=battery
MODS=battery.o
OBJS=
LIBS=

default: $(EXE)

# compiler/linker settings

CC=gcc
CFLAGS=-g -O3 -Wall --std=c99 -pedantic $$(pkg-config --cflags libnotify)
CPPFLAGS=
LDFLAGS=-g -O3 $$(pkg-config --libs libnotify)
INSTALL=install
INSTALLFLAGS=-C -D

# build targets

$(EXE): main.o $(MODS) $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $^ $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(EXE) main.o $(MODS)

install:
	$(INSTALL) $(INSTALLFLAGS) $(EXE) $(HOME)/.local/bin/$(EXE)

.PHONY: default clean install

