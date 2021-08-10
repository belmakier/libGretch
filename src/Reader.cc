#include <iostream>
#include <vector>

#include "Reader.hh"
#include "Event.hh"
#include "Gretina.hh"
#include "Chico.hh"
#include "ChicoHit.hh"
#include "ChicoConf.hh"
#include "Particle.hh"
#include "GretinaConf.hh"
#include "ChicoConf.hh"

namespace Gretch {
  double Reader::Dither = 0.0;

  double Reader::GetDither() { 
    Reader::Dither += 0.02;
    if (Reader::Dither >= 0.9999) { Reader::Dither = 0.0; } 
    return Reader::Dither;
  }

  int Reader::Init() {
    gretina_conf = GetConf<GretinaConf>();
    chico_conf = GetConf<ChicoConf>();

    eventmult = 0;
    event = new Event();
    lastevent = new Event();
    return 1;
  }
  
  int Reader::Next() {
    Event *temp = lastevent;
    lastevent = event;
    event = temp;

    event->Clear();
    int xtlmult = 0;
    int subevt = 0;
    old_timestamp = -1;
    while (1) { //subevent loop
      if (fread(&header, sizeof(header), 1, DataFile) == 0) {
        break;
      }

      if (0) {
        printf("type = %d\n", header.type);
        printf("length = %d\n", header.length);
        printf("timestamp = %ld\n", header.timestamp);
      }
        
      if (old_timestamp != -1) { 
        long long int dt = (header.timestamp - old_timestamp)/2 + 100;
        if (dt < 100 && header.type != 3) {
          printf("SEVERE ERROR: dt = %lld\n", dt);
        }
        if (dt > coinc_window) {
          fseek(DataFile, -sizeof(header), SEEK_CUR);
          break;           
        }  
      }    

      old_timestamp = header.timestamp;

      ++subevt;

      HeaderType htype(static_cast<HeaderType>(header.type));

      if (htype ==  HeaderType::kGretinaSignalDecomp) { //GRETINA SINGAL-DECOMP
        Gretina* gret = event->GetDetector<Gretina>();
        if (!gret) {
          gret = event->MakeDetector<Gretina>();
        }
          
        if (sizeof(crystal) != header.length) {
          printf("Severe Error: Inconsistent Gretina Payload length\n");
          return 1;
        }
    
        if (fread(&crystal, sizeof(crystal), 1, DataFile) == 0) {
          break;
        }

        gret->AddHit(header.timestamp, crystal, gretina_conf);
          
        xtlmult++;
      }
      else if (htype == HeaderType::kChico) {
        Chico* chic = event->GetDetector<Chico>();
        if (!chic) {
          chic = event->MakeDetector<Chico>();
        }
          
        if (sizeof(sub) < header.length) {
          printf("Severe Error: Payload length larger than buffer\n");
          printf("sizeof(sub)=%ld and header.length=%d\n", sizeof(sub), header.length);
          return 1;
        }             
        if (fread(sub, header.length, 1, DataFile) == 0) break;   
            
        if (sizeof(chico) < header.length) {
          printf("Severe Error: Inconsistent Chico Payload length\n");
          printf("sizeof(chico)=%ld and header.length=%d\n", sizeof(chico), header.length);
          continue;
        }

        chic->AddHit(header.length, sub, multiAnodeTDCNum, multiCathodeTDCNum, chico_conf);
       
      }
      else {
        if (fread(sub, header.length, 1, DataFile) == 0) {
          break;
        }
      }            
    }  //end subevent loop
    if (subevt == 0) { return 0; }  //no subevents, end of file

    Gretina* gret = event->GetDetector<Gretina>();
    if (gret) {
      gret->BuildGammas(gretina_conf);
    }

    return 1;
  }
      
}
