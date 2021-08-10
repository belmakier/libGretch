FLAGS=-std=c++17 -I`root-config --incdir` -O3
LDFLAGS=-L`root-config --libdir` `root-config --glibs` -lMathMore -lMinuit -lSpectrum -lm

all : libGretch

libGretch : Reader.cc Reader.hh Gretina.cc GretinaHit.cc GretinaConf.cc ChicoHit.cc ChicoConf.cc Chico.cc
	clang++ $(FLAGS) -shared -dynamiclib -o libGretch.dylib Reader.cc Gretina.cc GretinaHit.cc GretinaConf.cc ChicoHit.cc ChicoConf.cc Chico.cc $(LDFLAGS) -shared

install : libGretch.dylib
	cp libGretch.dylib ~/.local/lib/
	cp *.hh ~/.local/include/libGretch/
