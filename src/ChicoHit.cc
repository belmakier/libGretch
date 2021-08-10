#include <cmath>
#include <map>

#include "RawData.hh"
#include "ChicoHit.hh"
#include "Particle.hh"
#include "ChicoConf.hh"
#include "Reader.hh"

namespace Gretch {
  Mask ChicoHit::mStatus = {0xffff0000, 16};
  Mask ChicoHit::mLEDtsLong = {0xffffffff, 0};
  Mask ChicoHit::mLEDtsShort = {0x0000ffff, 0};
  Mask ChicoHit::mNextIntLong = {0xffff0000, 16};
  Mask ChicoHit::mNextIntShort = {0x0000ffff, 0};

  Mask ChicoHit::mQDCType = {0x7000000, 24};
  Mask ChicoHit::mQDCGEO = {0xf8000000, 27};
  Mask ChicoHit::mQDCChan = {0x003f0000, 16};
  Mask ChicoHit::mQDCData = {0x00000fff, 0};

  Mask ChicoHit::mCount = {0x00003f00, 8};
  
  Mask ChicoHit::mTDCType = {0xf8000000, 0};
  Mask ChicoHit::mTDCGEO = {0x0000001f, 0};
  Mask ChicoHit::mTDCChan = {0x03f80000, 19};
  Mask ChicoHit::mTDCData = {0x0007fff, 0};

  unsigned int ChicoHit::ANODE_E_VSN = 1;
  unsigned int ChicoHit::ANODE_T_VSN = 2;
  unsigned int ChicoHit::CATHODE_T_VSN = 3;

  unsigned int ChicoHit::QDCHEADER = 2;
  unsigned int ChicoHit::DATA = 0;
  unsigned int ChicoHit::QDCTRAILER = 4;

  unsigned int ChicoHit::TDCHEADER = 0x40000000;
  unsigned int ChicoHit::TDCTRAILER = 0x80000000;

  unsigned int ChicoHit::ANODE_REFCH = 24;
  unsigned int ChicoHit::CATHODE_REFCH = 80;
  unsigned int ChicoHit::RFCH = 25;
  unsigned int ChicoHit::SingleFlag = 31;
    
  int ChicoHit::Build(const unsigned int (&data)[8192], int &multiAnodeTDCNum, int &multiCathodeTDCNum) {
    Status = ChicoHit::mStatus(data[0]); 

    LEDts = static_cast<unsigned long long int>(ChicoHit::mLEDtsLong(data[1]));
    LEDts += (static_cast<unsigned long long int>(ChicoHit::mLEDtsShort(data[2])) << 32);

    int index = 2;
    unsigned int NextInt = GetNext(data, index);

    //////////////////////////////////////
    //// QDC PROCESSING
    /////////////////////////////////////
    if (NextInt != 0xffffffff) { //Anode QDC
      if (ChicoHit::mQDCType(NextInt) != ChicoHit::QDCHEADER) {
        return 0;
      }
      if (ChicoHit::mQDCGEO(NextInt) != ChicoHit::ANODE_E_VSN) {
        return 0;
      }
      int ChCounter = ChicoHit::mCount(NextInt);

      for (int i=0; i<ChCounter; ++i) {
        NextInt = GetNext(data, index);
        if (ChicoHit::mQDCType(NextInt) != ChicoHit::DATA) {
          return 0;
        }
        unsigned short int chan = mQDCChan(NextInt);
        int val = mQDCData(NextInt);

        //no if statment checking chan<PPAC_NUM etc.
        if (val > 0) {
          AnodeQDCCh.push_back(chan);
          AnodeQDCVal.push_back(val);
        }
      }
      NextInt = GetNext(data, index);
      if (ChicoHit::mQDCType(NextInt) != ChicoHit::QDCTRAILER) {
        return 0;
      }
      
      while (1) {
        NextInt = GetNext(data, index);
        if (NextInt == 0xffffffff) {
          break;
        }
      }
    }

    //////////////////////////////////////
    //// TDC ANODE PROCESSING
    /////////////////////////////////////
    NextInt = GetNext(data, index);
    if (NextInt != 0xffffffff) {
      if (ChicoHit::mTDCType(NextInt) != ChicoHit::TDCHEADER) {
        return 0;
      }
      if (ChicoHit::mTDCGEO(NextInt) != ChicoHit::ANODE_T_VSN) {
        return 0;
      }

      int SeenTrailer = 0;
      int refval = 0;
      while (!SeenTrailer) {
        NextInt = GetNext(data, index);
        switch(ChicoHit::mTDCType(NextInt)) {
        case static_cast<unsigned int>(TDCType::kData) : {
          unsigned short int chan = static_cast<unsigned short int>(ChicoHit::mTDCChan(NextInt));
          int val = ChicoHit::mTDCData(NextInt);
          if (chan != ChicoHit::ANODE_REFCH &&
              chan != ChicoHit::RFCH &&
              chan != ChicoHit::SingleFlag) {
            //again, no check for PPAC_NUM here
            AnodeTDCCh.push_back(chan);
            AnodeTDCVal.push_back(val);
          }
          else if (chan == ChicoHit::RFCH) {
            RF = val;
          }
          else if (chan == ChicoHit::SingleFlag) {
            Single = true;
          }
          else if (chan == ChicoHit::ANODE_REFCH) {
            refval = ChicoHit::mTDCData(NextInt);
          }
          break;
        }
        case static_cast<unsigned int>(TDCType::kTDCTrailer): {
          SeenTrailer = 1;
          break;
        }
        default:
          break;
        }
      }
        
      for (int i=0; i<(int)(AnodeTDCVal.size()); ++i) {
        AnodeTDCVal[i] -= refval;
      }

      NextInt = GetNext(data, index);
      if (NextInt != 0xffffffff) {
        multiAnodeTDCNum++;
        if ((multiAnodeTDCNum%1000) == 0) {
          printf("*Warning* anode TDC package with multiple events: %i\n",multiAnodeTDCNum);
        }
        while (1) {
          NextInt = GetNext(data, index);
          if (NextInt == 0xffffffff) {
            break;
          }
        }
      }        
    }

    //////////////////////////////////////
    //// TDC CATHODE PROCESSING
    /////////////////////////////////////
      
    NextInt = GetNext(data, index);
    if (NextInt != 0xffffffff) {
      if (ChicoHit::mTDCType(NextInt) != ChicoHit::TDCHEADER) {
        return 0;
      }
      if (ChicoHit::mTDCGEO(NextInt) != ChicoHit::CATHODE_T_VSN) {
        return 0;
      }

      int SeenTrailer = 0;
      int refval = 0;
      while (!SeenTrailer) {
        NextInt = GetNext(data, index);
        switch(ChicoHit::mTDCType(NextInt)) {
        case static_cast<unsigned int>(TDCType::kData): {
          unsigned short int chan = static_cast<unsigned short int>(ChicoHit::mTDCChan(NextInt));
          int val = ChicoHit::mTDCData(NextInt);
          if (chan != ChicoHit::CATHODE_REFCH) {
            //again, no check for PPAC_NUM here
            CathodeTDCCh.push_back(chan);
            CathodeTDCVal.push_back(val);
          }
          else if (chan == ChicoHit::CATHODE_REFCH) {
            refval = ChicoHit::mTDCData(NextInt);
          }
          break;
        }
        case static_cast<unsigned int>(TDCType::kTDCTrailer): {
          SeenTrailer = 1;
          break;
        }
        default:
          break;
        }
      }
        
      for (int i=0; i<(int)(CathodeTDCVal.size()); ++i) {
        CathodeTDCVal[i] -= refval;
      }

      NextInt = GetNext(data, index);
      if (NextInt != 0xffffffff) {
        multiCathodeTDCNum++;
        if ((multiCathodeTDCNum%1000) == 0) {
          printf("*Warning* cathode TDC package with multiple events: %i\n",multiCathodeTDCNum);
        }
        while (1) {
          NextInt = GetNext(data, index);
          if (NextInt == 0xffffffff) {
            break;
          }
        }
      }
    }

    return 1;
  }

  unsigned int ChicoHit::GetNext(const unsigned int (&data)[8192], int &i) {
    unsigned int retval = ChicoHit::mNextIntLong(data[i]) + (ChicoHit::mNextIntShort(data[i+1]) << 16);
    ++i;
    return retval;
  }

  int ChicoHit::GetParticle(Particle &part, const ChicoConf *conf) {
    const int nPPACs = conf->nPPACs;
    int theta[2] = {0};
    int phi[2] = {0};
    int validT[2] = {0};
    int validP[2] = {0};
    double fTheta[2] = {0};
    double fPhi[2] = {0};

    if (AnodeTDCCh.size() == 1) {
      const int aCh0 = AnodeTDCCh[0];
      if (aCh0 >= nPPACs/2 && aCh0 < nPPACs) {
        for (int i=0; i<(int)(CathodeTDCCh.size()) - 1; ++i) { //TJG - why the -1?
          const int ch_i = CathodeTDCCh[i];          
          for (int j=i+1; j<(int)(CathodeTDCCh.size()); ++j) { 
            const int ch_j = CathodeTDCCh[j];          
            if (ch_i < nPPACs*4 && ch_j < nPPACs*4) {
              //int vCathodeMap = conf->CathodeMap.at({ch_i, ch_j});
              const int vCathodeMap = conf->CathodeMap[ch_i][ch_j];
              int CathodeSign = 1;
              if (vCathodeMap != 0) {
                CathodeSign = vCathodeMap/std::abs(vCathodeMap);              
                if (CathodeTDCCh[0]/4 == aCh0) {
                  if (std::abs(vCathodeMap)==1) {
                    theta[0] = (CathodeTDCVal[i] - CathodeTDCVal[j])*CathodeSign;
                    //fTheta[0] = ((float)theta[0] + Reader::GetDither())*conf->ThetaCal.at(aCh0).second;
                    fTheta[0] = ((float)theta[0] + Reader::GetDither())*conf->ThetaGain[aCh0];
                    fTheta[0] += conf->ThetaOffset[aCh0];
                    validT[0] = 1;
                  }
                  if (std::abs(vCathodeMap)==2) {
                    phi[0] = (CathodeTDCVal[i] - CathodeTDCVal[j])*CathodeSign;
                    fPhi[0] = ((float)phi[0] + Reader::GetDither())*conf->PhiGain[aCh0];
                    fPhi[0] += conf->PhiOffset[aCh0];
                    fPhi[0] += 36.0*(float)(AnodeTDCCh[0]%10);
                    validP[0] = 1;
                  }
                }
              }
            }
          }
        }
        if (validP[0] == 1 && validT[0] == 1) {
          double pi = 3.14159265358979323846;
          part.Time = (double)LEDts;
          part.RF = ((RF+Reader::GetDither()) * conf->ch2ns)*0.1;
          part.ID = AnodeTDCCh[0];
          part.dT = 0.0;
          part.ThetaL = theta[0];
          part.fThetaL = fTheta[0]*pi/180.0;
          part.ThetaR = 180.0;
          part.fThetaR = pi;
          part.PhiL = phi[0];
          part.PhiR = 0.0;
          if(fPhi[0] < 180.0) {
            part.fPhiL = fPhi[0]*pi/180.0;
            part.fPhiR = fPhi[0]*pi/180.0 + pi;
          }
          else if (fPhi[0] >= 180.) {
            part.fPhiL = fPhi[0]*pi/180.0;
            part.fPhiR = fPhi[0]*pi/180.0 - pi;
          }
          part.dL = 0.0;
          part.dR = 0.0;
          part.Single = Single;
          return 1;
        }
      }
    }

    int doubleOK = 0;
    int channel_1 = 0;
    int channel_2 = 0;
    int val_1 = 0;
    int val_2 = 0;
    if (AnodeTDCCh.size() >= 2) {
      for (int i=0; i<(int)(AnodeTDCCh.size()) - 1; ++i) {
        const int aCh_i = AnodeTDCCh[i];          
        for (int j=i+1; j<(int)(AnodeTDCCh.size()); ++j) {
          const int aCh_j = AnodeTDCCh[j];
          if ( aCh_i < nPPACs && aCh_j < nPPACs) {
            //int vAnodeMap = conf->AnodeMap.at({aCh_i, aCh_j});
            int vAnodeMap = conf->AnodeMap[aCh_i][aCh_j];
            if (vAnodeMap != 0) {
              channel_1 = aCh_i;
              channel_2 = aCh_j;
              val_1 = AnodeTDCVal[i];
              val_2 = AnodeTDCVal[j];
              doubleOK = 1;
            }
          }                
        }
      }
    }

    if (doubleOK == 1) {
      AnodeTDCCh.clear();
      AnodeTDCVal.clear();
      AnodeTDCCh.push_back(channel_1);
      AnodeTDCCh.push_back(channel_2);
      AnodeTDCVal.push_back(val_1);
      AnodeTDCVal.push_back(val_2);
    }

    std::vector<int> anode(2);
    double dT = 0;
    if (AnodeTDCCh.size() == 2) {
      const int aCh0 = AnodeTDCCh[0];
      const int aCh1 = AnodeTDCCh[1];
      if ( aCh0 < nPPACs && aCh1 < nPPACs ) {
        //int vAnodeMap = conf->AnodeMap.at({aCh0, aCh1});
        const int vAnodeMap = conf->AnodeMap[aCh0][aCh1];
        if (vAnodeMap != 0) {
          for (int i=0; i<2; ++i) {
            anode[i] = AnodeTDCCh[((vAnodeMap+1)/2==i)];
          }

          dT = (double)(AnodeTDCVal[0] - AnodeTDCVal[1])*vAnodeMap;
          for (int i=0; i<(int)(CathodeTDCCh.size()) - 1; ++i) { //TJG - why the -1?
            const int ch_i = CathodeTDCCh[i];          
            for (int j=i+1; j<(int)(CathodeTDCCh.size()); ++j) { 
              const int ch_j = CathodeTDCCh[j];          
              if (ch_i < nPPACs*4 && ch_j < nPPACs*4) {
                //int vCathodeMap = conf->CathodeMap.at({ch_i, ch_j});
                const int vCathodeMap = conf->CathodeMap[ch_i][ch_j];
                int CathodeSign = 1;
                if (vCathodeMap != 0) {
                  CathodeSign = vCathodeMap/std::abs(vCathodeMap);
                  for (int l=0; l<2; ++l) {
                    if (CathodeTDCCh[i]/4 == anode[l]) {
                      if (std::abs(vCathodeMap)==1) {
                        theta[l] = (CathodeTDCVal[i] - CathodeTDCVal[j])*CathodeSign;
                        fTheta[l] = ((float)theta[l] + Reader::GetDither())*conf->ThetaGain[anode[l]];
                        fTheta[l] += conf->ThetaOffset[anode[l]];
                        validT[l] = 1;
                      }
                      if (std::abs(vCathodeMap)==2) {
                        phi[l] = (CathodeTDCVal[i] - CathodeTDCVal[j])*CathodeSign;
                        fPhi[l] = ((float)phi[l] + Reader::GetDither())*conf->PhiGain[anode[l]];
                        fPhi[l] += conf->PhiOffset[anode[l]];
                        fPhi[l] += 36.0*(float)(anode[l]%10);
                        validP[l] = 1;
                      }
                    }
                  }
                }
              }
            }            
          }       
        }
      }    
    }

    int validTheta = 0;
    int validPhi = 0;
    if (validT[0] == 1 && validT[1] == 1) {
      validTheta = 1;
    }
    if (((validP[0] + validP[1]) == 1) || ((validP[0]*validP[1]) == 1)) {
      validPhi = 1;        
    }
    if (validTheta == 1 && validPhi == 1) {
      double pi = 3.14159265358979323846;
      part.Time = (double)LEDts;
      part.RF = ((RF+Reader::GetDither()) * conf->ch2ns)*0.1;
      part.ID = anode[0];
      part.dT = dT;
      part.ThetaL = theta[0];
      part.fThetaL = fTheta[0]*pi/180.0;
      part.ThetaR = theta[1];
      part.fThetaR = fTheta[1]*pi/180.0;
      part.PhiL = phi[0];
      part.PhiR = phi[1];
      if(validP[0] == 1 && validP[1] == 0) {
        part.fPhiL = fPhi[0]*pi/180.0;
        part.fPhiR = fPhi[0]*pi/180.0 + pi;
      }
      else if (validP[1] == 1 && validP[1] == 0) {
        part.fPhiL = fPhi[1]*pi/180.0;
        part.fPhiR = fPhi[1]*pi/180.0 - pi;
      }
      else if (validP[1] == 1 && validP[1] == 1) {
        if (fPhi[0] < 180.0 && fPhi[1] < 360.0) {
          part.fPhiL = (fPhi[0] + fPhi[1] - 180.0)*0.5*pi/180.0;
          part.fPhiR = (fPhi[0] + fPhi[1] + 180.0)*0.5*pi/180.0;
        }          
      }
      part.dL = 12.8/(0.75471 * std::sin(part.fThetaL)*std::cos(part.fPhiL- (18.0 + (float)(anode[0]%10)*36.0)*pi/180.0) + 0.65606*std::cos(part.fThetaL));
         
      part.dR = 12.8/(0.75471 * std::sin(part.fThetaR)*std::cos(part.fPhiR- (18.0 + (float)(anode[1]%10)*36.0)*pi/180.0) + 0.65606*std::cos(part.fThetaR));
      part.Single = Single;
      return 1;
    }
    else {
      return 0;
    }
    return 0;
  }
  
}
