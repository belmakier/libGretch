#include <fstream>
#include <sstream>
#include <cmath>

#include "Gretina.hh"
#include "GretinaHit.hh"
#include "GretinaConf.hh"
#include "Gamma.hh"

namespace Gretch {
  
  int Gretina::AddHit(const int64_t GEBtimestamp,
                      const crys_intpts &data,
                      const GretinaConf *conf) {
      GretinaHit hit;
      hit.Build(GEBtimestamp, data, conf);
      hits.push_back(hit);
      return 0;
  }

  int Gretina::BuildGammas(const GretinaConf *conf) {
    int nAddbacks = 0;
    std::vector<int> addbacks(hits.size());
    if (conf->Addback) {
      for (int i=0; i<hits.size(); ++i) {
        auto hit_i = hits[i];
        if (hit_i.valid == false) { continue; }
        if (conf->AddbackDets[hit_i.CrystalID] == 0) { continue; }
        for (int j=0; j<hits.size(); ++j) {
          auto hit_j = hits[j];
          if (hit_j.valid == false) { continue; }
          if (conf->AddbackDets[hit_j.CrystalID] == 0) { continue; }
          if (i!=j && addbacks[i] == 0 && addbacks[j] == 0) {
            double costhet = std::sin(hit_i.Theta)*std::sin(hit_j.Theta)*std::cos(hit_i.Phi - hit_j.Phi) + std::cos(hit_i.Theta)*std::cos(hit_j.Theta);
            if (costhet >= std::cos(conf->ClusterAngle*3.14159265/180.0) && costhet <= 1.0) {
              ++nAddbacks;
            
              addbacks[i] = 1;
              addbacks[j] = 1;

              if (hit_i.TotalEnergy > hit_j.TotalEnergy) {
                std::vector<int> inds = {i,j};              
                gammas.emplace_back(hit_i.TotalEnergy + hit_j.TotalEnergy,
                                    inds,
                                    i,
                                    hit_i.Theta,
                                    hit_i.Phi,
                                    hit_i.CrystalID,
                                    hit_i.Time,
                                    hit_i.Fix,
                                    conf->Efficiency(hit_i.TotalEnergy + hit_j.TotalEnergy)
                                    );
              }
              else {
                std::vector<int> inds = {i,j};
                gammas.emplace_back(hit_i.TotalEnergy + hit_j.TotalEnergy,
                                    inds,
                                    j,
                                    hit_j.Theta,
                                    hit_j.Phi,
                                    hit_j.CrystalID,
                                    hit_j.Time,
                                    hit_j.Fix,
                                    conf->Efficiency(hit_i.TotalEnergy + hit_j.TotalEnergy)
                                    );
              } 
            }
          }
        }
      }
    }
    for (int i=0; i<hits.size(); ++i) {
      if (addbacks[i] == 0) {
        auto hit_i = hits[i];
        if (hit_i.valid == false) { continue; }
        std::vector<int> inds = {i};
        gammas.emplace_back(hit_i.TotalEnergy,
                            inds,
                            i,
                            hit_i.Theta,
                            hit_i.Phi,
                            hit_i.CrystalID,
                            hit_i.Time,
                            hit_i.Fix,
                            conf->Efficiency(hit_i.TotalEnergy)
                            ); 
      }
    }
    return nAddbacks; 
  }  
} 
