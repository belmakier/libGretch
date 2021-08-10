#ifndef GRETCH_DETECTORCONF_HH
#define GRETCH_DETECTORCONF_HH

#include "Detector.hh"

namespace Gretch {
  class DetectorConf {
  public:
    DetType type;
  public:
    DetectorConf() : type(DetType::kNone) {}
    DetectorConf(DetType t) : type(t) {}
    virtual ~DetectorConf() {}   
  };
}          

#endif
