#ifndef GRETCH_CHICOHIT_HH
#define GRETCH_CHICOHIT_HH

#include <vector>

#include "RawData.hh"
#include "Particle.hh"
#include "ChicoConf.hh"

namespace Gretch {
  enum class TDCType : unsigned int {
                      kData = 0,
                      kTDCTrailer = 0x80000000                      
  };
  
  class ChicoHit {
  private:
    static Mask mStatus;
    static Mask mLEDtsLong;
    static Mask mLEDtsShort;
    static Mask mNextIntLong;
    static Mask mNextIntShort;
    static Mask mQDCType;
    static Mask mQDCGEO;
    static Mask mQDCChan;
    static Mask mQDCData;
    static Mask mCount;

    static Mask mTDCType;
    static Mask mTDCGEO;
    static Mask mTDCChan;
    static Mask mTDCData;
    
    static unsigned int ANODE_E_VSN;
    static unsigned int ANODE_T_VSN;
    static unsigned int CATHODE_T_VSN;
    static unsigned int QDCHEADER;
    static unsigned int DATA;
    static unsigned int QDCTRAILER;
    static unsigned int TDCHEADER;
    static unsigned int TDCTRAILER;
    static unsigned int ANODE_REFCH;
    static unsigned int CATHODE_REFCH;
    static unsigned int RFCH;
    static unsigned int SingleFlag;
    
  public:    
    unsigned int Status;
    unsigned long long int LEDts;
    std::vector<unsigned int> AnodeQDCCh;
    std::vector<int> AnodeQDCVal;

    std::vector<unsigned int> AnodeTDCCh;
    std::vector<int> AnodeTDCVal;
    std::vector<unsigned int> CathodeTDCCh;
    std::vector<int> CathodeTDCVal;
    int RF;
    bool Single;
    
  public:
    ChicoHit()      
    {
      AnodeQDCCh.reserve(32);
      AnodeQDCVal.reserve(32);

      AnodeTDCCh.reserve(32);
      AnodeTDCVal.reserve(32);

      CathodeTDCCh.reserve(32);
      CathodeTDCVal.reserve(32);
    }
    ChicoHit(const unsigned int (&data)[8192], int &multiAnodeTDCNum, int &multiCathodeTDCNum) : ChicoHit() { Build(data, multiAnodeTDCNum, multiCathodeTDCNum); }
    ~ChicoHit() {
      AnodeQDCCh.clear();
      AnodeQDCVal.clear();
      AnodeTDCCh.clear();
      AnodeTDCVal.clear();
      CathodeTDCCh.clear();
      CathodeTDCVal.clear();
    }
    int Build(const unsigned int (&data)[8192], int &multiAnodeTDCNum, int &multiCathodeTDCNum);
    unsigned int GetNext(const unsigned int (&data)[8192], int &i);
    int GetParticle(Particle &part, const ChicoConf *conf);
  };
}

#endif
