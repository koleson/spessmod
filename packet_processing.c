//
// Created by koleson on 12/10/23.
//

#include "packet_processing.h"
#include "log.h"

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
   // from here, packet and header are the relevant params.  should be able to extract.
  // kmo 10 dec 2023 13h35
  LOG_INFO("got a packet with length [%d]", header->len);

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
    LOG_WARN("got an IPv6 packet, which is a surprise - not processing");
    return;
  }
  else
  {
    LOG_WARN("this doesn't look like an IP packet - not processing");
    return;
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
    LOG_WARN("packet appears to not be TCP - how this happened given the filter is a mystery.  not processing.");
    return;
  }

  const struct tcphdr* tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));

  // TODO:  is this the right size for this?  VSCode very angry at `struct ip`
  // kmo 9 dec 2023 20h42
  // kernel source says 16 bits which sounds right.
  // kmo 9 dec 20h59
  // endianness!  kmo 9 dec 2023 21h05

  uint16_t ack = tcp_header->ack;
  uint32_t ack_seq = tcp_header->ack_seq;
  uint32_t seq = tcp_header->seq;
  uint8_t fin = tcp_header->fin;
  LOG_INFO("ack: %u - ack_seq: %u - seq: %u- fin: %u", ack, ack_seq, seq, fin);

  uint8_t data_offset_words = tcp_header->doff;
  uint16_t source_port = ntohs(tcp_header->source);
  uint16_t dest_port = ntohs(tcp_header->dest);
  LOG_INFO("source port: %d (0x%04x)", source_port, source_port);
  LOG_INFO("destination port: %d (0x%04x)", dest_port, dest_port);

  // replies don't have to be to port 502, it seems
  if (dest_port == 502 || source_port == 502)
  {
    LOG_INFO("confirmed port 502 as source or destination");
  }
  else
  {
    LOG_ERROR("packet not heading for port 502 in spite of filters - possibly not modbus 0 not processing further.");
    return;
  }

  LOG_INFO("sizes - ether %lu, ip %lu, tcp %lu",
    sizeof(struct ether_header), sizeof(struct ip), sizeof(struct tcphdr));

  uint8_t data_offset_bytes = data_offset_words * 4;
  uint8_t options_length = data_offset_bytes - sizeof(struct tcphdr);
  LOG_INFO("TCP Header data offset: %d bytes, %d of which are TCP options",
    data_offset_bytes, options_length);

  u_char* data = (u_char*)(packet + sizeof(struct ether_header) + sizeof(struct ip)
    + sizeof(struct tcphdr) + (sizeof(uint8_t) * options_length));
  uint32_t data_length = header->len - (sizeof(struct ether_header) + sizeof(struct ip)
    + sizeof(struct tcphdr) + (sizeof(uint8_t) * options_length));

  LOG_INFO("data length: %d", data_length);

  if (data_length == 0) {
    LOG_INFO("no modbus data to analyze - leaving packet");
    return;
  }

  LOG_INFO("hex dump follows:");
  printf("\n\n");
  for (int byte = 0; byte < data_length; byte++) {
    printf("%02x.", (uint8_t)data[byte]);
  }
  printf("\n\n");

  uint16_t transaction = (data[0] << 8) | data[1];
  uint16_t protocol = (data[2] << 8) | data[3];
  uint16_t length = (data[4] << 8) | data[5];
  uint8_t unit = data[6];
  uint8_t function = data[7];

  // TODO:  further interpretation of modbus data,
  // which is conditional on function and other state.
  // kmo 9 dec 2023 22h15

  // data follows
  // uint16_t checksum = (last 2 bytes)
  LOG_INFO("transaction: %d", transaction);
  LOG_INFO("protocol: %d", protocol);
  LOG_INFO("length: %d", length);
  LOG_INFO("unit: %d", unit);
  LOG_INFO("function: %d", function);

  if (function == 3) {
    LOG_INFO("modbus function: Read Holding Registers");
    // TODO:  check if this is query or response
    // hacky - could base on IP in subnet (1 vs non-1)

    // read registers
    // uint16_t base_register = (data[8] << 8) | data[9];
    // uint16_t num_registers = (data[10] << 8 | data[11]);
  }
}

int add_filter(pcap_t* pcap) {
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
    return 1;
  }

  int setfilter_result = pcap_setfilter(pcap, &filter_program);
  if (setfilter_result != 0)
  {
    LOG_ERROR("could not set filter on pcap");
    return 2;
  }

  return setfilter_result;
}