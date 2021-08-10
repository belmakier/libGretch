#ifndef GRETCH_GAMMA_HH
#define GRETCH_GAMMA_HH

namespace Gretch {
  class Gamma {
  public:
    double Energy;

    std::vector<int> HitInds;
    int FirstInt;
    int ID;
    
    double Theta;
    double Phi;

    double Time;

    float Efficiency;

    bool Fix;
  public:
    Gamma() {};
    Gamma(double en, std::vector<int> inds) :
      Energy(en), HitInds(inds), FirstInt(0), Theta(0), Phi(0), ID(0), Time(0){}
    Gamma(double en, std::vector<int> inds, int fi, double t, double p, int id, double time, bool fx, float eff) :
      Energy(en), HitInds(inds), FirstInt(fi), Theta(t), Phi(p), ID(id), Time(time), Fix(fx), Efficiency(eff) {}
  };
}

#endif
