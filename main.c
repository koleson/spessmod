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

  const u_char *packet;
  struct pcap_pkthdr header;
  LOG_INFO("awaiting matching packet");

  packet = pcap_next(pcap, &header);
  LOG_INFO("got a packet with length [%d]", header.len);

  // following use of structs/pointer arithmetic based on:
  // https://elf11.github.io/2017/01/22/libpcap-in-C.html
  // kmo 9 dec 2023 20h23

  // bail out if packet isn't IPv4
  const struct ether_header *ethernet_header = (struct ether_header *)packet;
  LOG_INFO("type: 0x%04x", ethernet_header->ether_type);

  if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP)
  {
    LOG_INFO("confirmed this is an IPv4 packet");
  }
  else if (ntohs(ethernet_header->ether_type == ETHERTYPE_IPV6))
  {
    LOG_WARN("got an IPv6 packet, which is a surprise");
    exit(2);
  }
  else
  {
    LOG_WARN("this doesn't look like an IP packet...");
    exit(2);
  }

  // informational only:  print MAC addresses involved
  LOG_INFO("destination MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           ethernet_header->ether_dhost[0], ethernet_header->ether_dhost[1], ethernet_header->ether_dhost[2],
           ethernet_header->ether_dhost[3], ethernet_header->ether_dhost[4], ethernet_header->ether_dhost[5]);
  LOG_INFO("destination MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           ethernet_header->ether_shost[0], ethernet_header->ether_shost[1], ethernet_header->ether_shost[2],
           ethernet_header->ether_shost[3], ethernet_header->ether_shost[4], ethernet_header->ether_shost[5]);

  // informational only:  print IP addresses involved
  const struct ip* ip_header = (struct ip*)(packet + sizeof(struct ether_header));
  char source_ip[INET_ADDRSTRLEN];
  char dest_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);
  LOG_INFO("Source IP: %s", source_ip);
  LOG_INFO("Destination IP: %s", dest_ip);

  // informational only:  print protocol
  LOG_INFO("IP protocol: %01x", ip_header->ip_p);

  if (ip_header->ip_p == IPPROTO_TCP)
  {
    LOG_INFO("confirmed packet is TCP");
  }
  else
  {
    LOG_WARN("packet appears to not be TCP - how this happened given the filter is a mystery.");
    exit(2);
  }

  const struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));

  // TODO:  is this the right size for this?  VSCode very angry at `struct ip`
  // kmo 9 dec 2023 20h42
  uint32_t source_port = tcp_header->source;
  uint32_t dest_port = tcp_header->dest;
  LOG_INFO("source port: %d", source_port);
  LOG_INFO("destination port: %d", dest_port);

  if (dest_port == 502)
  {
    LOG_INFO("confirmed destination port 502");
  }
  else
  {
    LOG_ERROR("packet not heading for port 502 in spite of filters - possibly not modbus");
    exit(2);
  }

  u_char* data = (u_char*)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
  uint32_t data_length = header.len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
  LOG_INFO("data length: %d", data_length);
  
  uint8_t byte_zero = data[0];
  uint8_t byte_one = data[1];
  LOG_INFO("byte 0: 0x%02x", byte_zero);
  LOG_INFO("byte 1: 0x%02x", byte_one);

  // TODO:  more logic goes here
  uint16_t transaction = data[0];
  uint16_t protocol = data[2];
  uint16_t length = data[4];
  uint8_t unit = data[6];
  uint8_t function = data[7];

  // data follows
  // uint16_t checksum = (last 2 bytes)
  LOG_INFO("transaction: %d", transaction);
  LOG_INFO("protocol: %d", protocol);
  LOG_INFO("length: %d", length);
  LOG_INFO("unit: %d", unit);
  LOG_INFO("function: %d", function);

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
