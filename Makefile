#Makefile for libGretch
SHELL := /bin/bash

FLAGS = -std=c++17 -O3
LDFLAGS= -lm
COMPILER=clang++

all : lib/libGretch

lib :
	mkdir -p lib

obj :
	mkdir -p obj

obj/Reader.o : src/Reader.cc src/Reader.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/Reader.o src/Reader.cc

obj/Gretina.o : src/Gretina.cc src/Gretina.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/Gretina.o src/Gretina.cc

obj/GretinaHit.o : src/GretinaHit.cc src/GretinaHit.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/GretinaHit.o src/GretinaHit.cc

obj/GretinaConf.o : src/GretinaConf.cc src/GretinaConf.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/GretinaConf.o src/GretinaConf.cc

obj/Chico.o : src/Chico.cc src/Chico.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/Chico.o src/Chico.cc

obj/ChicoHit.o : src/ChicoHit.cc src/ChicoHit.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/ChicoHit.o src/ChicoHit.cc

obj/ChicoConf.o : src/ChicoConf.cc src/ChicoConf.hh | obj
	$(COMPILER) $(FLAGS) -c -o obj/ChicoConf.o src/ChicoConf.cc

lib/libGretch : obj/Reader.o obj/Gretina.o obj/GretinaHit.o obj/GretinaConf.o obj/Chico.o obj/ChicoHit.o obj/ChicoConf.o src/Event.hh src/Gamma.hh src/RawData.hh src/Particle.hh | obj lib
	$(COMPILER) $(FLAGS) -shared -dynamiclib -o lib/libGretch.dylib obj/Reader.o obj/Gretina.o obj/GretinaHit.o obj/GretinaConf.o obj/ChicoHit.o obj/ChicoConf.o obj/Chico.o 

install : lib/libGretch.dylib
	cp lib/libGretch.dylib ~/.local/lib/
	cp src/*.hh ~/.local/include/libGretch/

clean :
	rm -rf obj/

cleanall :
	rm -rf obj/
	rm -rf lib/
