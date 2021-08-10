#ifndef GRETCH_READER_HH
#define GRETCH_READER_HH

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "RawData.hh"
#include "DetectorConf.hh"
#include "Event.hh"
#include "GretinaConf.hh"
#include "ChicoConf.hh"

namespace Gretch {
  class Reader {
  public:
    FILE *DataFile;
    int64_t old_timestamp;
    int coinc_window;
    int multiAnodeTDCNum = 0;
    int multiCathodeTDCNum = 0;
    static double Dither;

    std::vector<DetectorConf*> Configurations;

    GEBHeader header = {0};
    ChicoEvent chico = {0};
    crys_intpts crystal = {0};
    unsigned int sub[8192];
    GretinaConf *gretina_conf = NULL;
    ChicoConf *chico_conf = NULL;

    long long eventmult;

  public:
    Event *event;
    Event *lastevent;

  public:    
    Reader() : DataFile(0),
               old_timestamp(-1),
               coinc_window(171)
    {}
    Reader(std::string fn) : Reader() { DataFile = fopen(fn.c_str(), "r"); }
    static double GetDither();
    int Init();
    int Read();
    int currIndex;
    int Next();

    template <typename T>
    T* GetConf() {
      for(auto conf : Configurations) {
        T* output = dynamic_cast<T*>(conf);
        if(output){
          return output;
        }
      }
      return NULL;
    }

    template <typename T>
    void SetConf(T conf) {
      T* existing_conf = GetConf<T>();
      if (existing_conf) {
        delete existing_conf; //risky, could be accessed elsewhere?        
      }
      T* configuration = new T(conf);
      Configurations.push_back(configuration);
    }
  };
}

#endif
