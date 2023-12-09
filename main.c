#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>

// http://yuba.stanford.edu/~casado/pcap/section1.html

pcap_if_t* get_interface(int argc, char** argv, char* errbuf);

int main(int argc, char** argv) {
  
  
  /*
  we actually don't care about the network address or mask
  of the interface we capture on since we assume only the
  PVS and ESS are on the interface.
  kmo 8 dec 2023 23h46
  */
  // char* net; /* dot notation of the network address */
  // char* mask; /* dot notation of the network mask */

  int retval; /* return code */
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_if_t* selected_interface = get_interface(argc, argv, errbuf);

}

pcap_if_t* get_interface(int argc, char** argv, char* errbuf) {
  printf("argc: %d\n", argc);
  if (argc < 2) {
    printf("no arguments - using first available en or eth interface\n");
  }
  if (argc > 2) {
    printf("too many arguments, confused, bailing out\n");
    exit(1);
  }

  pcap_if_t *all_devs, *selected_device;

  int findalldevs_return = pcap_findalldevs(&all_devs, errbuf);

  if (findalldevs_return != 0) {
    printf("error finding devices: %s\n", errbuf);
    exit(1);
  }

  int devnum = 0;
  for (selected_device = all_devs; selected_device != NULL; selected_device = selected_device->next) {
    printf("device %d: %s - %s\n", devnum, selected_device->name, selected_device->description);
    // TODO:  compare selected device name against argument or "en"/"eth"
    devnum++;
  }
  
  // TODO:  should not get here
  return NULL;
}
