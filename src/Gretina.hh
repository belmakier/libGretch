#ifndef GRETCH_GRETINA_HH
#define GRETCH_GRETINA_HH

#include <vector>
#include <map>
#include <iostream>

#include "Detector.hh"
#include "GretinaHit.hh"
#include "GretinaConf.hh"
#include "Gamma.hh"

namespace Gretch {
  class GretinaHit;
  
  class Gretina : public Detector {
  public:
    std::vector<GretinaHit> hits;
    std::vector<Gamma> gammas;
  public:
    Gretina() : Detector(DetType::kGretina) { hits.reserve(32); gammas.reserve(32); }
    int AddHit(const int64_t GEBtimestamp,
               const crys_intpts &data,
               const GretinaConf *conf);
    int BuildGammas(const GretinaConf *conf);
    ~Gretina() {
      hits.clear(); gammas.clear();
    }
  };
}

#endif
