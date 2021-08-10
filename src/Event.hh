#ifndef GRETCH_EVENT_HH
#define GRETCH_EVENT_HH

#include <iostream>

#include "Detector.hh"

namespace Gretch {
  class Event {
  public:
    std::vector<Detector*> Detectors;
    int Size() { return Detectors.size(); }

    Event() : Detectors(0) {}
    
    template <typename T>
    T* GetDetector() {
      for(auto det : Detectors) {
        T* output = dynamic_cast<T*>(det);
        if(output){
          return output;
        }
      }
      return NULL;
    }

    template <typename T>
    T* MakeDetector() {
      T* det = new T();
      Detectors.push_back(det);
      return det;
    }

    ~Event() {
      for (Detector *det : Detectors) {
        delete det;
      }
    }

    int Clear() {
      for (Detector *det : Detectors) {
        delete det;
      }
      Detectors.clear();
      return 1;
    }
  };
}

#endif
