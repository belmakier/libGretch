#ifndef GRETCH_CHICOCONF_HH
#define GRETCH_CHICOCONF_HH

#include <map>

#include "RawData.hh"
#include "DetectorConf.hh"

namespace Gretch {
  
  class ChicoConf : public DetectorConf {
  public:
    int nPPACs;
    double ch2ns;
    double ThetaGain[nppacs];
    double ThetaOffset[nppacs];
    double PhiGain[nppacs];
    double PhiOffset[nppacs];    
    int AnodeMap[nppacs][nppacs];
    int CathodeMap[4*nppacs][4*nppacs];
    //std::map<std::pair<int, int>, int> CathodeMap;
  public:
    ChicoConf() : DetectorConf(DetType::kChico), nPPACs(nppacs), ch2ns(0.1) { SetAnodeMap(); SetCathodeMap(); }
    ~ChicoConf() {}
    int ReadThetaCal(std::string fn);
    int ReadPhiCal(std::string fn);
    int SetAnodeMap();
    int SetCathodeMap();
  };
}

#endif
