#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>

// http://yuba.stanford.edu/~casado/pcap/section1.html

int main(int argc, char** argv) {
  printf("argc: %d\n", argc);
  char* dev; /* name of device to use */
  char* net; /* dot notation of the network address */
  char* mask; /* dot notation of the network mask */
  int retval; /* return code */
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_if_t *all_devs, *selected_device;

  int findalldevs_return = pcap_findalldevs(&all_devs, errbuf);

  if (findalldevs_return != 0) {
    printf("error finding devices: %s\n", errbuf);
    exit(1);
  }

  int devnum = 0;
  for (selected_device = all_devs; selected_device != NULL; selected_device = selected_device->next) {
    printf("device %d: %s - %s\n", devnum, selected_device->name, selected_device->description);
    devnum++;
  }

}
