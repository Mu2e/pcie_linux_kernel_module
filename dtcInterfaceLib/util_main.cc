// This file (util_main.cc) was created by Ron Rechenmacher <ron@fnal.gov> on
// May 13, 2015. "TERMS AND CONDITIONS" governing this file are in the README
// or COPYING file. If you do not have such a file, one can be obtained by
// contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
// $RCSfile: .emacs.gnu,v $
// rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";

#include <cstdio>		// printf
#include <cstdlib>		// strtoul
#include <iostream>
#include "DTC.h"
#ifdef _WIN32
# include <chrono>
# include <thread>
# define usleep(x)  std::this_thread::sleep_for(std::chrono::microseconds(x));
# ifndef TRACE
#  include <stdio.h>
#  ifdef _DEBUG
#   define TRACE(lvl,...) printf(__VA_ARGS__); printf("\n")
#  else
#   define TRACE(...)
#  endif
# endif
# define TRACE_CNTL(...)
#else
# include "trace.h"
# include <unistd.h>		// usleep
#endif

using namespace DTCLib;
using namespace std;

unsigned getOptionValue(int *index, char **argv[])
{
  char* arg = (*argv)[*index];
  if(arg[2] != '\0') {
    (*index)++;
    return strtoul((*argv)[*index], NULL, 0);
  }
  else {
    int offset = 2;
    if(arg[2] == '=') {
      offset = 3;
    }

    return strtoul(&(arg[offset]), NULL, 0);
  }
}

int
main(int	argc
, char	*argv[])
{
  bool pause = false;
  bool incrementTimestamp = true;
  unsigned delay = 0;
  unsigned number = 1;
  unsigned timestampOffset = 1;
  unsigned packetCount = 0;
  char* op = NULL;

  for(int optind = 1; optind < argc; ++optind) {
    if(argv[optind][0] == '-') {
      switch(argv[optind][1]) {
      case 'h':
        
      case 'p':
        pause = true;
        break;
      case 'i':
        incrementTimestamp = false;
        break;
      case 'd':
        delay = getOptionValue(&optind, &argv);
        break;
      case 'n':
        number = getOptionValue(&optind, &argv);
        break;
      case 'o':
        timestampOffset = getOptionValue(&optind, &argv);
        break;
      case 'c':
        packetCount = getOptionValue(&optind, &argv);
        break;
      default:
        break;
      }
    }
    else {
      op = argv[optind];
    }
  }
  sprintf("Options are: Operation: %s, Num: %u, Delay: %u, TS Offset: %u, PacketCount: %u, Pause: %s, Inrement TS: %s",op, number, delay, timestampOffset, packetCount, pause ? "true" : "false", incrementTimestamp ? "true" : "false");
  exit(0);

    if (argc > 1 && strcmp(argv[1], "read") == 0)
    {
        DTC *thisDTC = new DTC(DTC_SimMode_Hardware);
        DTC_DataHeaderPacket packet = thisDTC->ReadNextDAQPacket();
        cout << packet.toJSON() << '\n';
    }
    else if (argc > 1 && strcmp(argv[1], "read_data") == 0)
    {
        mu2edev device;
        device.init();
        unsigned reads = 1;
        if (argc > 2) reads = strtoul(argv[2], NULL, 0);
        for (unsigned ii = 0; ii < reads; ++ii)
        {
            void *buffer;
            int tmo_ms = 0;
            int sts = device.read_data(DTC_DMA_Engine_DAQ, &buffer, tmo_ms);
            TRACE(1, "util - read for DAQ - ii=%u sts=%d %p", ii, sts, buffer);
            usleep(0);
        }
    }
    else if (argc > 1 && strcmp(argv[1], "read_release") == 0)
    {
        mu2edev device;
        device.init();
        unsigned releases = 1;
        if (argc > 2) releases = strtoul(argv[2], NULL, 0);
        for (unsigned ii = 0; ii < releases; ++ii)
        {
            void *buffer;
            int tmo_ms = 0;
            int stsRD = device.read_data(DTC_DMA_Engine_DAQ, &buffer, tmo_ms);
            int stsRL = device.read_release(DTC_DMA_Engine_DAQ, 1);
            TRACE(12, "util - release/read for DAQ and DCS ii=%u stsRD=%d stsRL=%d %p", ii, stsRD, stsRL, buffer);
            usleep(0);
        }
    }
    else if (argc > 1 && strcmp(argv[1], "HW") == 0)
    {
        DTC *thisDTC = new DTC(DTC_SimMode_Hardware);
        thisDTC->EnableRing(DTC_Ring_0, DTC_RingEnableMode(true,true,false), DTC_ROC_0);
        thisDTC->SetInternalSystemClock();
        thisDTC->DisableTiming();
        thisDTC->SetMaxROCNumber(DTC_Ring_0, DTC_ROC_0);

        unsigned gets = 1;
        if(argc > 2) gets = strtoul(argv[2], NULL, 0);
  
        for(unsigned ii = 0; ii < gets; ++ii)
        {
            DTC_DataHeaderPacket header(DTC_Ring_0, (uint16_t)0, DTC_DataStatus_Valid, DTC_Timestamp((uint64_t)ii));
            std::cout << "Request: " << header.toJSON() << std::endl;
            thisDTC->WriteDMADAQPacket(header);
            thisDTC->SetFirstRead(true);
            std::cout << "Reply:   " << thisDTC->ReadNextDAQPacket().toJSON() << std::endl;
        }
    }
    else if (argc > 1 && strcmp(argv[1], "DTC") == 0)
    {
        DTC *thisDTC = new DTC(DTC_SimMode_Hardware);
        thisDTC->EnableRing(DTC_Ring_0, DTC_RingEnableMode(true, true, false), DTC_ROC_0);
        thisDTC->SetInternalSystemClock();
        thisDTC->DisableTiming();
        thisDTC->SetMaxROCNumber(DTC_Ring_0, DTC_ROC_0);
        if(!thisDTC->ReadSERDESOscillatorClock()) { thisDTC->ToggleSERDESOscillatorClock(); } // We're going to 2.5Gbps for now
        unsigned gets = 1;
        if (argc > 2) gets = strtoul(argv[2], NULL, 0);
        unsigned debug = 0;
        if (argc > 3) debug = strtoul(argv[3], NULL, 0);
        unsigned debugCount = 0;
        if (argc > 4) debugCount = strtoul(argv[4], NULL, 0);
        unsigned offset = 1;
        if(argc > 5) offset = strtoul(argv[5], NULL, 0);
        unsigned increment = 1;
        if(argc > 6) increment = strtoul(argv[6], NULL, 0);  
        unsigned delay = 0;
        if(argc > 7) delay = strtoul(argv[7], NULL, 0);       
       
        for (unsigned ii = 0; ii < gets; ++ii)
        {
            usleep(delay);
            uint64_t ts = increment ? ii + offset : offset;
            vector<void*> data = thisDTC->GetData(DTC_Timestamp((uint64_t)ts), debug, debugCount);
            if (data.size() > 0)
            {
                cout << data.size() << " packets returned\n";
                for (size_t i = 0; i < data.size(); ++i)
                {
                    TRACE(19, "DTC::GetJSONData constructing DataPacket:");
                    DTC_DataPacket     test = DTC_DataPacket(data[i]);
                    cout << test.toJSON() << '\n'; // dumps whole databuff_t
                    printf("data@%p=0x%08x\n", data[i], *(uint32_t*)(data[i]));
                    //DTC_DataHeaderPacket h1 = DTC_DataHeaderPacket(data[i]);
                    //cout << h1.toJSON() << '\n';
                    DTC_DataHeaderPacket h2 = DTC_DataHeaderPacket(test);
                    cout << h2.toJSON() << '\n';
                    for (int jj = 0; jj < h2.GetPacketCount(); ++jj) {
                        cout << "\t" << DTC_DataPacket(((uint8_t*)data[i]) + ((jj + 1) * 16)).toJSON() << endl;
                    }
                }
            }
            else
            {
                TRACE_CNTL("modeM", 0L);
                cout << "no data returned\n";
                return (0);
            }
        }
    }
    else// if (argc > 1 && strcmp(argv[1],"get")==0)
    {
        DTC *thisDTC = new DTC(DTC_SimMode_Hardware);
        unsigned gets = 1000000;
        if (argc > 1) gets = strtoul(argv[1], NULL, 0);

        for (unsigned ii = 0; ii < gets; ++ii)
        {
            vector<void*> data = thisDTC->GetData(DTC_Timestamp((uint64_t)ii));
            if (data.size() > 0)
            {
                //cout << data.size() << " packets returned\n";
                for (size_t i = 0; i < data.size(); ++i)
                {
                    TRACE(19, "DTC::GetJSONData constructing DataPacket:");
                    DTC_DataPacket     test = DTC_DataPacket(data[i]);
                    //cout << test.toJSON() << '\n'; // dumps whole databuff_t
                    //printf("data@%p=0x%08x\n", data[i], *(uint32_t*)(data[i]));
                    //DTC_DataHeaderPacket h1 = DTC_DataHeaderPacket(data[i]);
                    //cout << h1.toJSON() << '\n';
                    DTC_DataHeaderPacket h2 = DTC_DataHeaderPacket(test);
                    //cout << h2.toJSON() << '\n';
                    // for (int jj = 0; jj < h2.GetPacketCount(); ++jj) {
                    //    cout << "\t" << DTC_DataPacket(((uint8_t*)data[i]) + ((jj + 1) * 16)).toJSON() << endl;
                    //}
                }
            }
            else
            {
                TRACE_CNTL("modeM", 0L);
                cout << "no data returned\n";
                return (0);
            }
        }
    }
    return (0);
}   // main
