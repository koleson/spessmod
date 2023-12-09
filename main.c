#include <stdio.h>
#include <pcap.h>

// http://yuba.stanford.edu/~casado/pcap/section1.html

int main(int argc, char** argv) {
  char* dev; /* name of device to use */
  char* net; /* dot notation of the network address */
  char* mask; /* dot notation of the network mask */
  int retval; /* return code */
  char errbuf[PCAP_ERRBUF_SIZE];

  /*
  lookupdev, as you might guess, is deprecated now that computers typically
  have more than one network port. 
  kmo 8 dec 2023 20h29
  */

  dev = pcap_lookupdev(errbuf);

  printf("DEV: %s\n", dev);
  printf("hello!\n");
}
