#include <cmath>

#include "GretinaHit.hh"
#include "Reader.hh"

namespace Gretch {
  int GretinaHit::Build(const int64_t GEBtimestamp,
                        const crys_intpts &data,
                        const GretinaConf *gret) {

    CrystalID = gret->CrystalSwap[data.crystal_id];
    if (false) {
      if (CrystalID != data.crystal_id ) {
        std::cout << "Crystal " << data.crystal_id << " treated like " << CrystalID << std::endl;
      }
    }
    Hole = CrystalID/4;  //holes are one-indexed, CrystalID starts at 4
    PAD = data.pad;
    chisq = data.chisq;
    t0 = data.t0;
    timestamp = GEBtimestamp; //units 20 ns

    if (gret->PAD128) {
      if (PAD != 0 && PAD != 128) { valid = false; }
    }
    else if (gret->PAD128Fix) {
      if (PAD != 0 && PAD != 128) { valid = false; }      
      else if (PAD == 128 && gret->NoisyCrystals[CrystalID] == 0) { valid = false; }
    }
    else {
      if (PAD != 0) { valid = false; }
    }

    // if (valid == false) {
    //   std::cout << "Bad PAD = " << PAD << ", crystal ID = " << CrystalID << std::endl;
    // }
    
    double offset = 0.0;
    double gain = 1.0;

    if (gret->Calibration.find(CrystalID) != gret->Calibration.end()) {
      auto cal = gret->Calibration.at(CrystalID);
      offset = cal.first;
      gain = cal.second;
    }

    RawEnergy = data.tot_e;
    TotalEnergy = (data.tot_e)*gain + offset;

    if (data.t0 != data.t0) {
      if (false) {
        std::cout << "Warning! crystal t0 is NaN: " << data.t0 << std::endl;
      }
      Fix = true;
      Time = data.timestamp + gret->MeanT0/10.0;      
    }
    else {
      Time = data.timestamp + data.t0/10.0;
    }

    double minz = 1e6;
    double sum_energy = 0;
    double max_energy = 0;
    x = 0;
    y = 0;
    z = 0;

    for (int i=0; i<data.num; ++i) {
      double int_x = data.intpts[i].x;
      double int_y = data.intpts[i].y;
      double int_z = data.intpts[i].z;
      double int_e = data.intpts[i].e;

      if (Fix) {
        int_x = 0;
        int_y = 0;
        int_z = 0;
        int_e = 1;
      }
      
      if (int_e <= 0) {
        int_x = 0;
        int_y = 0;
        int_z = 0;
        int_e = 1;
        Fix = true;
      }      
      
      switch(gret->FX) {        
      case FirstX::kSmallestZ:
        //find smallest z
        if (int_z < minz) {
          minz = int_z;
          x = int_x;
          y = int_y;
          z = int_z;
        }
        break;
      case FirstX::kMeanPos:
        x += int_x/(double)data.num;
        y += int_y/(double)data.num;
        z += int_z/(double)data.num;
        break;
      case FirstX::kEnergyWeighted:
        x += int_x * int_e;
        y += int_y * int_e;
        z +=  int_z * int_e;
        sum_energy += int_e;
        break;
      case FirstX::kCrystalCent:
        x = 0;
        y = 0;
        z = 0;
        break;
      case FirstX::kMaxEnergy:
        if (int_e > max_energy) {
          max_energy = int_e;
          x = int_x;
          y = int_y;
          z = int_z;            
        }
        //std::cout << x << "  " << y << "  " << z << std::endl;
        break;
      case FirstX::kHybrid:
        if (data.tot_e > 515) { //arbitrary, should this be a parameter?
          if (int_e > max_energy) {
            max_energy = int_e;
            x = int_x;
            y = int_y;
            z = int_z;            
          } 
        }
        else {
          x += int_x * int_e;
          y += int_y * int_e;
          z +=  int_z * int_e;
          sum_energy += int_e;                
        }
        break;
      }
    }
    if (sum_energy > 0.0) {
      x /= sum_energy;
      y /= sum_energy;
      z /= sum_energy;
    }

    
    if (x != x || y != y || z != z) {
      std::cout << "tote = " << data.tot_e << std::endl;
      std::cout << "x/y/z = " << x << " " << y << " " << z << std::endl;
    }

    auto pos = gret->GetPosition(Hole, CrystalID%4, x,y,z);    
    PosX = pos[0];
    PosY = pos[1];
    PosZ = pos[2];

    double x2y2 = std::sqrt(PosX*PosX + PosY*PosY);
    double x2y2z2 = std::sqrt(PosX*PosX + PosY*PosY + PosZ*PosZ);
    double costhet = PosZ/x2y2z2;
    if (costhet >= -1.0 and costhet <= 1.0) {
      Theta = std::acos(costhet);
      //std::cout << "Crystal = " << CrystalID << "   Theta = " << Theta*180.0/3.1415926535 << std::endl;
    }
    else {
      std::cout << "Cos(theta) was out of range: " << costhet << std::endl;
      std::cout << PosX << "   " << PosY << "   " << PosZ << std::endl;
      std::cout << "Crystal " << CrystalID << "   PAD = " << data.pad << std::endl;
      std::cout << "FX = " << (int)(gret->FX) << std::endl;
      Theta = 0;
    }
    double cosphi = PosX/x2y2;
    if (cosphi >= -1.0 and cosphi <= 1.0) {
      Phi = std::acos(cosphi);
      if (PosY < 0) {
        Phi = 2.0*3.1415926536 - Phi;
      }
    }
    else {
      std::cout << "Cos(phi) was out of range: " << cosphi << std::endl;
      Phi = 0;
    }

    // if (142 <= Theta*180.0/3.1415926535 && Theta <=143*180.0/3.1415926535) {
    //   if (193 <= Phi*180.0/3.1415926535 && Phi*180.0/3.1415926535 <= 194) {
    //     std::cout << Hole << "  " << CrystalID%4 << std::endl;
    //     std::cout << x << "   " << y << "   " << z << std::endl;
    //     std::cout << PosX << "   " << PosY << "   " << PosZ << std::endl;
    //     if (Fix) { std::cout << "Fix" << std::endl; }
    //   }
    // }

    if (!(gret->FixValid)) {
      if (Fix) { valid = false; }
    }
    
    return 0;
  }

}
