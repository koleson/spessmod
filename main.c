#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pcap.h>

#include "log.h"

// http://yuba.stanford.edu/~casado/pcap/section1.html

const char *get_interface(const int argc, const char **argv, char *errbuf);

int main(int argc, char **argv)
{

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

  const char *selected_interface = get_interface(argc, argv, errbuf);
  int promiscuous = 1;

  if (selected_interface == NULL)
  {
    LOG_ERROR("shouldn't get here - get_interface should exit if unable to acquire interface.");
    exit(1);
  }

  LOG_DEBUG("opening pcap on interface %s", selected_interface);
  pcap_t *pcap = pcap_open_live(selected_interface, BUFSIZ, promiscuous, 1000, errbuf);
  if (pcap == NULL)
  {
    LOG_ERROR("Couldn't open interface %s in promiscuous mode.", selected_interface);
    exit(2);
  }
  else
  {
    LOG_INFO("pcap open in promiscuous mode!");
  }

  LOG_INFO("compiling filter...");
  // acquire all modbus over TCP packets
  char *filter_expression = "tcp port 502 and (tcp[13] & 0x7f == 0x01)";
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

  const unsigned char *packet;
  struct pcap_pkthdr header;
  LOG_INFO("awaiting matching packet");

  pcap_next(pcap, &header);
  printf("got a packet with length [%d]", header.len);

  // TODO:  more logic goes here

  pcap_close(pcap);
  return 0;
}

const char *get_interface(const int argc, const char **argv, char *errbuf)
{
  LOG_DEBUG("argc: %d", argc);
  if (argc > 2)
  {
    LOG_ERROR("too many arguments, confused, bailing out");
    exit(1);
  }

  bool use_first_ethernet_interface;
  char *specified_interface_name = NULL;
  if (argc < 2)
  {
    LOG_INFO("no arguments - using first available en or eth interface");
    use_first_ethernet_interface = true;
  }
  else
  {
    use_first_ethernet_interface = false;
    specified_interface_name = argv[1];
    LOG_INFO("specified interface: %s", specified_interface_name);
  }

  pcap_if_t *all_devs, *selected_device;

  int findalldevs_return = pcap_findalldevs(&all_devs, errbuf);

  if (findalldevs_return != 0)
  {
    LOG_ERROR("error finding devices: %s", errbuf);
    exit(1);
  }

  int devnum = 0;
  char *selected_device_name = NULL;

  for (selected_device = all_devs; selected_device != NULL; selected_device = selected_device->next)
  {
    if (use_first_ethernet_interface)
    {
      char *prefixes[2] = {"eth", "en"};
      for (int prefix_num = 0; prefix_num < 2; prefix_num++)
      {
        char *prefix = prefixes[prefix_num];
        if (strncmp(prefix, selected_device->name, sizeof(char) * strlen(prefix)) == 0)
        {
          LOG_INFO("found ethernet interface %s", selected_device->name);
          char *copy = (char *)malloc(strlen(selected_device->name) + 1);
          if (copy != NULL)
          {
            strcpy(copy, selected_device->name);
          }
          selected_device_name = copy;
        }
      }
    }
    else
    {
      // check to make sure `specified_interface_name` is a valid interface
      if (strcmp(specified_interface_name, selected_device->name) == 0)
      {
        LOG_INFO("found specified interface %s (device #%d)", selected_device->name, devnum);
        char *copy = (char *)malloc(strlen(selected_device->name) + 1);
        if (copy != NULL)
        {
          strcpy(copy, selected_device->name);
        }
        selected_device_name = copy;
      }
    }
    devnum++;
  }

  if (selected_device_name == NULL)
  {
    LOG_ERROR("could not find specified interface %s", specified_interface_name);
    exit(1);
  }
  else
  {
    LOG_DEBUG("returning device name %s", selected_device_name);
    return selected_device_name;
  }
}
