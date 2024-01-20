#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pcap/pcap.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "log.h"
#include "capture_setup.h"
#include "packet_processing.h"
#include "known_registers.h"

#ifdef INFLUX_LOGGING
#include "influx_logger.h"
#endif // INFLUX_LOGGING

// http://yuba.stanford.edu/~casado/pcap/section1.html


int main(const int argc, const char **argv)
{
#ifdef INFLUX_LOGGING
  influx_log_test();
#endif // INFLUX_LOGGING

  populate_known_registers();

  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_t* pcap = get_pcap(argc, argv, errbuf);
  if (!pcap) {
    LOG_ERROR("could not get pcap");
#ifdef INFLUX_LOGGING
  LOG_INFO("INFLUX_LOGGING was defined");
#else
  LOG_INFO("INFLUX_LOGGING was not defined");
#endif
    exit(1);
  }

  const int add_filter_result = add_filter(pcap);
  if (add_filter_result != 0) {
    LOG_ERROR("error applying filter to pcap");
    exit(1);
  }

  print_struct_sizes();

  // TODO:  change to -1 once we're sure we're looping well
  const int packets_to_capture = 25; // negative values = capture indefinitely.

  pcap_loop(pcap, packets_to_capture, process_packet, NULL);

  pcap_close(pcap);



  return 0;
}
