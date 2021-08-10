#include <fstream>
#include <sstream>
#include <cmath>
 
#include "Chico.hh"
#include "ChicoHit.hh"
#include "ChicoConf.hh"
#include "Particle.hh"

namespace Gretch {  
  int Chico::AddHit(const int64_t GEBtimestamp,
                    const unsigned int (&data)[8192],
                    int &multiAnodeTDCNum,
                    int &multiCathodeTDCNum,
                    const ChicoConf *conf) { 

    hits.emplace_back(data, multiAnodeTDCNum, multiCathodeTDCNum);
    Particle part;    
    if (hits[hits.size()-1].GetParticle(part, conf)) { 
      parts.push_back(part);
    }
    return 0;
  }
}
