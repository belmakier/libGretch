#ifndef GRETCH_GRETINACONF_HH
#define GRETCH_GRETINACONF_HH

#include <map>

#include "DetectorConf.hh"

namespace Gretch {
  enum class FirstX;

  enum class EffType {
                      kLogLogPoly = 1,
                      kRadWare = 2
  };
                      
  
  class GretinaConf : public DetectorConf {
  public:
    std::map<int, std::pair<double, double> > Calibration;
    std::map<std::pair<int, int>, double[4][4]> CrystalMap;
    double PosXOffset;
    double PosYOffset;
    double PosZOffset;
    double MeanT0;
    FirstX FX;

    double ClusterAngle;
    float EfficiencyCal[7];
    float AbsEff;
    EffType EfficiencyType;
    bool Addback;
    bool PAD128;
    bool PAD128Fix;
    bool FixValid;
    int CrystalSwap[120];
    int NoisyCrystals[120];
    int AddbackDets[120];
  public:
    GretinaConf() : DetectorConf(DetType::kGretina),
                    PAD128(false),
                    PAD128Fix(false),
                    FixValid(false),
                    PosXOffset(0),
                    PosYOffset(0),
                    PosZOffset(0) {
      for (int i=0; i<120; ++i) { CrystalSwap[i] = i; }
      for (int i=0; i<120; ++i) { NoisyCrystals[i] = 0; }
      for (int i=0; i<120; ++i) { AddbackDets[i] = 1; }
    }
    ~GretinaConf() {}
    int ReadCrystalMap(std::string fn);
    int ReadCalibration(std::string fn);
    int ReadEfficiencyCal(std::string fn);
    int ReadCrystalSwap(std::string fn);
    int ReadNoisyCrystals(std::string fn);
    int ReadAddbackDets(std::string fn);
    std::vector<double> GetPosition(int hole, int xtl, double x, double y, double z) const;
    float Efficiency(const double &e) const;
    void PrintAngles();
  };
}

#endif
