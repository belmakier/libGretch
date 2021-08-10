#ifndef GRETCH_RAWDATA_HH
#define GRETCH_RAWDATA_HH

#include <stdio.h>
#include <iostream>

namespace Gretch {
    enum class HeaderType {
                         kGretinaSignalDecomp = 1,
                         kGretinaTracked = 3,
                         kChico = 12
  };
  
  struct GEBHeader {
    int32_t type;
    int32_t length; /* length of payload following the header, in bytes */
    int64_t timestamp;
  };

  static const int max_det = 30;
  static const int max_det_xtl = 4;
  static const int max_intpts = 16;
  static const int max_xtl = max_det*max_det_xtl;
  static const int nppacs = 20;

  struct crys_intpts {
    int type; /* as of June 2012: abcd5678 */
    int crystal_id;
    int num; /* # of interaction points from decomp, or # of nets on decomp error */
    float tot_e; /* CC energy for the central contact selected for use in decomp (calibrated, and for 10 MeV CC channels, includes DNL correction. */
    int core_e[4]; /* 4 raw core energies from FPGA filter (uncalibrated) */
    long long int timestamp; //(10 ns)
    long long int trig_time;
    float t0; //(1 ns)
    float cfd;
    float chisq;
    float norm_chisq;
    float baseline;
    float prestep; /* avg trace value before step (baseline) */
    float poststep; /* avg trace value after step (flat-top) */
    int pad; /* non-0 with a decomp error, value gives error type */ 
    /*    pad = 1   a null pointer was passed to dl_decomp()
          = 2   total energy below threshold
          = 3   no net charge segments in evt
          = 4   too many net charge segments
          = 5   chi^2 is bad following decomp (in this case
          crys_intpts is non-zero but post-processing step is not applied)
          = 6   bad build, i.e. <40 segment+CC channels found

          pad|= 128  PileUp, i.e. pileup flag or deltaT1<6usec

          e.g. pad = 128  pileup+Good
          = 133  pileup+BadChisq                */
    struct {
      float x,y,z,e; /* here e refers to the fraction */
      int seg; /* segment number hit */
      float seg_ener; /* energy of the hit segment */
    } intpts[max_intpts];
  };

  struct Mask {
    unsigned int BitMask;
    unsigned int BitShift;
    unsigned int operator()(const unsigned int &data) const {
      return ((data & BitMask) >> BitShift);
    }
  };
  
  struct ChicoEvent {
    unsigned long long int  LEDts;
    unsigned short int      cathode_tdc_num;
    int                     cathode_tdc_val[128];
    unsigned short int      cathode_tdc_ch[128];
    unsigned short int      anode_tdc_num;
    int                     anode_tdc_val[128];
    unsigned short int      anode_tdc_ch[128];
    unsigned short int      anode_qdc_num;
    int                     anode_qdc_val[32];
    unsigned short int      anode_qdc_ch[32];
    unsigned int            status;
    int			          RF;
    bool			          SINGLE;
  };

  struct tracked_gammas {
    int ngam;
    int pad;
    struct {
      float esum;              /* gamma ray energy */
      int ndet;                /* number of interactions */
      float fom;               /* figure of merit */
      int   tracked;           /* 1==if tracked */
      long long int timestamp; /* timestap of first interaction point */
      float x0, y0, z0, e0;    /* first interaction point */
      float x1, y1, z1, e1;    /* second interaction point */
    } gam[max_det];
  };
}

#endif
