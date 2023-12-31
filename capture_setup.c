//
// Created by koleson on 12/10/23.
//

#include "capture_setup.h"
#include "log.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <pcap.h>

// private interface
const char* get_interface(const int argc, const char **argv, char *errbuf);


pcap_t* get_pcap(const int argc, const char **argv, char *errbuf) {
  LOG_DEBUG("argc: %d", argc);
  if (argc == 3)
  {
    if ((strcmp(argv[1], "-f") == 0) || strcmp(argv[1], "--file")) {
      // open pcap from file
      const char* filename = argv[2];
      pcap_t* pcap = pcap_open_offline(filename, errbuf);
      return pcap;
    }
    else {
      LOG_ERROR("incorrect arguments, confused, bailing out");
      LOG_ERROR("(use -f or --file to specify pcap file; interface name for live capture)");
      return NULL;
    }
  }
  else if (argc > 3) {
    LOG_ERROR("too many arguments, confused, bailing out");
      LOG_ERROR("(use -f or --file to specify pcap file; interface name for live capture)");
      return NULL;
  }
  else
  {
    const char *selected_interface = get_interface(argc, argv, errbuf);
    int promiscuous = 1;

    if (selected_interface == NULL)
    {
      LOG_ERROR("exiting - unable to acquire interface.");
      return NULL;
    }

    LOG_DEBUG("opening pcap on interface %s", selected_interface);
    pcap_t* pcap = pcap_open_live(selected_interface, BUFSIZ, promiscuous, 1000, errbuf);
    return pcap;
  }
}

const char *get_interface(const int argc, const char **argv, char *errbuf)
{
  
  
  

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
    specified_interface_name = (char *)argv[1];
    LOG_INFO("specified interface: %s", specified_interface_name);
  }

  pcap_if_t *all_devs, *selected_device;

  int findalldevs_return = pcap_findalldevs(&all_devs, errbuf);

  if (findalldevs_return != 0)
  {
    LOG_ERROR("error finding devices: %s", errbuf);
    return NULL;
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
    return NULL;
  }
  else
  {
    LOG_DEBUG("returning device name %s", selected_device_name);
    return selected_device_name;
  }
}
