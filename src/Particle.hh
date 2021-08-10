#ifndef GRETCH_PARTICLE_HH
#define GRETCH_PARTICLE_HH

namespace Gretch {
  class Particle {
  public:
    double Time;
    double RF;
    int ID;
    int dT;
    int ThetaL;
    float fThetaL;
    int ThetaR;
    float fThetaR;
    int PhiL;
    int PhiR;
    float fPhiL;
    float fPhiR;
    float dL;
    float dR;
    bool Single;
  public:
    Particle() :
      Time(0),
      RF(0),
      ID(0),
      dT(0),
      ThetaL(0),
      ThetaR(0),
      fThetaL(0),
      fThetaR(0),
      PhiL(0),
      PhiR(0),
      fPhiL(0),
      fPhiR(0),
      dL(0),
      dR(0),
      Single(false) {}    
  };
  
}

#endif
