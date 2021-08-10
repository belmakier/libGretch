#ifndef GRETCH_CHICO_HH
#define GRETCH_CHICO_HH

#include <vector>
#include <map>

#include "Detector.hh"
#include "ChicoHit.hh"
#include "ChicoConf.hh"
#include "Particle.hh"

namespace Gretch {
  class Chico : public Detector {
  public:
    std::vector<ChicoHit> hits;
    std::vector<Particle> parts;
  public:
    Chico() : Detector(DetType::kChico) { hits.reserve(50); parts.reserve(50); }
    int AddHit(const int64_t GEBtimestamp,
               const unsigned int (&data)[8192],
               int &multiAnodeTDCNum,
               int &multiCathodeTDCNum,
               const ChicoConf *conf);
    ~Chico() { hits.clear(); parts.clear(); }
  };
}

#endif
