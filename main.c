#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pcap.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "log.h"
#include "capture_setup.h"
#include "packet_processing.h"

// http://yuba.stanford.edu/~casado/pcap/section1.html


int main(const int argc, const char **argv)
{
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_t* pcap = get_pcap(argc, argv, errbuf);

  LOG_INFO("compiling filter...");
  // acquire all modbus over TCP packets
  char *filter_expression = "tcp port 502";
  struct bpf_program filter_program;
  int optimize = 0;
  bpf_u_int32 netmask = 0;

  int compilation_result = pcap_compile(pcap, &filter_program, filter_expression, optimize, netmask);
  if (compilation_result != 0)
  {
    LOG_ERROR("could not compile bpf_program from expression %s", filter_expression);
    exit(2);
  }

  int setfilter_result = pcap_setfilter(pcap, &filter_program);
  if (setfilter_result != 0)
  {
    LOG_ERROR("could not set filter on pcap");
  }

  const u_char* packet;
  struct pcap_pkthdr header;
  LOG_INFO("awaiting matching packet");
  packet = pcap_next(pcap, &header);

  LOG_INFO("processing packet");
  process_packet(NULL, &header, packet);


  pcap_close(pcap);
  return 0;
}
