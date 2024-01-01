#
# Makefile for syasokoban
# Copyright (c) David Joffe, 2001-2024
# https://djoffe.com/
#
# Note you need libsdl1.2 as dependency
#


CPP=gcc
BIN=syasokoban
CFLAGS= -ansi -Wall -I/usr/local/include $(shell sdl-config --cflags)

OBJ = $(shell find src -iname '*.cpp' -type f | sed 's/\.cpp$$/.o/' | sort)

LIBS= -lstdc++ $(shell sdl-config --libs) -lm


default: syasokoban

syasokoban: $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) $(LIBS)

clean:
	rm -f $(BIN) $(OBJ) *~ core #*

#install:
#	cp $(BIN) /usr/local/bin

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

