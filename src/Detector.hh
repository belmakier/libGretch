#ifndef GRETCH_DETECTOR_HH
#define GRETCH_DETECTOR_HH

namespace Gretch {
  enum class DetType {
                      kNone = 0,
                      kGretina = 1,
                      kChico = 2,
  };
    
  class Detector {
  public:
    DetType type;

    Detector() : type(DetType::kNone) {}
    Detector(DetType t) : type(t) {}
    virtual ~Detector() {};
  };
}

#endif
