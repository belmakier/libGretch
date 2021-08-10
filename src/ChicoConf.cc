#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "ChicoConf.hh"

namespace Gretch {
  int ChicoConf::SetCathodeMap() {
    //CathodeMap.clear();
    for (int i=0; i<nPPACs*4; ++i) {
      for (int j=0; j<nPPACs*4; ++j) {
        //CathodeMap[{i,j}] = 0;
        CathodeMap[i][j] = 0;
      }
    }
    for (int i=0; i<nPPACs*4; ++i) {
      for (int j=0; j<nPPACs*4; ++j) {
        if ((i%4)==0 && (j==i+1)) {
          //CathodeMap[{i,j}] = 1;
          CathodeMap[i][j] = 1;
          //CathodeMap[{j,i}] = -1;
          CathodeMap[j][i] = -1;
        }
        if ((i%4)==2 && (j==i+1)) {
          CathodeMap[i][j] = 2;
          //CathodeMap[{i,j}] = 2;
          CathodeMap[j][i] = -2;
          //CathodeMap[{j,i}] = -2;
        }
      }
    }    
    return 0;
  }

  int ChicoConf::SetAnodeMap() {
    //AnodeMap.clear();
    for (int i=0; i<nPPACs; ++i) {
      for (int j=0; j<nPPACs; ++j) {
        //AnodeMap[{i,j}] = 0;
        AnodeMap[i][j] = 0;
      }
    }

    for (int i=0; i<5; ++i) {
      for (int j=0; j<10; ++j) {
        if (j==(i+5)) {
          AnodeMap[i][j] = 1;
          //AnodeMap[{i,j}] = 1;
          AnodeMap[j][i] = -1;
          //AnodeMap[{j,i}] = -1;
        }
      }
    }
    return 0;
  }

  int ChicoConf::ReadThetaCal(std::string fn) {
    //ThetaCal.clear();

    std::ifstream file(fn.c_str());

    std::string line;
    std::getline(file, line);
    std::getline(file, line);

    int chan;
    double offset, gain;
    std::cout << "Reading PPAC Theta cals" << std::endl;
    for (int i=0; i<nPPACs; ++i) {
      file >> chan >> offset >> gain;
      //ThetaCal[chan] = {offset, gain};
      ThetaGain[chan] = gain;
      ThetaOffset[chan] = offset;
      std::cout << chan << "   " << offset << "   " << gain << std::endl;
    }
    return 0;
  }

  int ChicoConf::ReadPhiCal(std::string fn) {
    //PhiCal.clear();

    std::ifstream file(fn.c_str());

    std::string line;
    std::getline(file, line);
    std::getline(file, line);

    int chan;
    double offset, gain;
    std::cout << "Reading PPAC Phi cals" << std::endl;
    for (int i=0; i<nPPACs; ++i) {
      file >> chan >> offset >> gain;
      PhiGain[chan] = gain;
      PhiOffset[chan] = offset;
      //PhiCal[chan] = {offset, gain};
      std::cout << chan << "   " << offset << "   " << gain << std::endl;
    }

    return 0;
  }
  
}
