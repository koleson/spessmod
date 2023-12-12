//
// Created by koleson on 12/10/23.
//

#include "packet_processing.h"
#include "log.h"

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

void print_struct_sizes() {
  LOG_INFO("sizes - ether %lu, ip %lu, tcp %lu",
           sizeof(struct ether_header), sizeof(struct ip), sizeof(struct tcphdr));
}

void process_packet(u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
  // from here, packet and header are the relevant params.  should be able to extract.
  // kmo 10 dec 2023 13h35
  LOG_INFO("got a packet with length [%u] (caplen %u)", header->len, header->caplen);

  // following use of structs/pointer arithmetic based on:
  // https://elf11.github.io/2017/01/22/libpcap-in-C.html
  // kmo 9 dec 2023 20h23

  // bail out if packet isn't IPv4
  const struct ether_header* ethernet_header = (struct ether_header *) packet;
  const uint16_t ethertype = ethernet_header->ether_type;

  LOG_INFO("type: 0x%04x", ethertype);

  if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP)
  {
    LOG_DEBUG("confirmed this is an IPv4 packet");
  }
  else if (ntohs(ethernet_header->ether_type == ETHERTYPE_IPV6))
  {
    LOG_WARN("got an IPv6 packet, which is a surprise - not processing");
    return;
  }
  else
  {
    LOG_WARN("this doesn't look like an IP packet - not processing (ethertype )");
    return;
  }

  // informational only:  print MAC addresses involved
  LOG_INFO("destination MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           ethernet_header->ether_dhost[0], ethernet_header->ether_dhost[1], ethernet_header->ether_dhost[2],
           ethernet_header->ether_dhost[3], ethernet_header->ether_dhost[4], ethernet_header->ether_dhost[5]);
  LOG_INFO("source MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           ethernet_header->ether_shost[0], ethernet_header->ether_shost[1], ethernet_header->ether_shost[2],
           ethernet_header->ether_shost[3], ethernet_header->ether_shost[4], ethernet_header->ether_shost[5]);

  // informational only:  print IP addresses involved
  const struct ip* ip_header = (struct ip *) (packet + sizeof(struct ether_header));
  char source_ip[INET_ADDRSTRLEN];
  char dest_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);
  LOG_INFO("Destination / Source IP: %s / %s", dest_ip, source_ip);

  // informational only:  print protocol
  const uint16_t ip_protocol = ip_header->ip_p;
  LOG_INFO("IP protocol: %01x", ip_protocol);

  if (ip_header->ip_p == IPPROTO_TCP)
  {
    LOG_INFO("confirmed packet is TCP");
  }
  else
  {
    LOG_WARN("packet appears to not be TCP - how this happened given the filter is a mystery.  not processing.");
    return;
  }

  const struct tcphdr* tcp_header = (struct tcphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));

  // TODO:  is this the right size for this?  VSCode very angry at `struct ip`
  // kmo 9 dec 2023 20h42
  // kernel source says 16 bits which sounds right.
  // kmo 9 dec 20h59
  // endianness!  kmo 9 dec 2023 21h05

  const uint16_t ack = tcp_header->ack;
  const uint32_t ack_seq = tcp_header->ack_seq;
  const uint32_t seq = tcp_header->seq;
  const uint8_t fin = tcp_header->fin;
  const uint16_t tcp_checksum = tcp_header->check;
  LOG_INFO("ack: %u - ack_seq: %u - seq: %u - fin: %u - check: %u (0x%04x)",
           ack, ack_seq, seq, fin, tcp_checksum, tcp_checksum);

  const uint8_t data_offset_words = tcp_header->doff;
  const uint16_t source_port = ntohs(tcp_header->source);
  const uint16_t dest_port = ntohs(tcp_header->dest);
  LOG_INFO("source / dest port: %d (0x%04x) / %d (0x%04x)", source_port, source_port , dest_port, dest_port);

  if (dest_port == 502 || source_port == 502)
  {
    LOG_INFO("confirmed port 502 as source or destination");
  }
  else
  {
    LOG_ERROR("packet not heading for port 502 in spite of filters - possibly not modbus 0 not processing further.");
    return;
  }

  const uint8_t data_offset_bytes = data_offset_words * 4;
  const uint8_t options_length = data_offset_bytes - sizeof(struct tcphdr);
  LOG_INFO("TCP Header data offset: %d bytes, %d of which are TCP options",
           data_offset_bytes, options_length);

  const u_char* data = (u_char *) (packet + sizeof(struct ether_header) + sizeof(struct ip)
                                   + sizeof(struct tcphdr) + (sizeof(uint8_t) * options_length));
  const uint32_t data_length = header->len - (sizeof(struct ether_header) + sizeof(struct ip)
                                              + sizeof(struct tcphdr) + (sizeof(uint8_t) * options_length));

  LOG_INFO("data length: %d", data_length);

  if (data_length == 0)
  {
    LOG_INFO("no modbus data to analyze - leaving packet ============\n\n");
    return;
  }

  LOG_INFO("hex dump follows:");
  printf("\n");
  for (int byte = 0; byte < data_length; byte++)
  {
    printf("%02x.", (uint8_t) data[byte]);
  }
  printf("\n\n");

  const uint16_t transaction = (data[0] << 8) | data[1];
  const uint16_t protocol = (data[2] << 8) | data[3];
  const uint16_t length = (data[4] << 8) | data[5];
  const uint8_t unit = data[6];
  const uint8_t function = data[7];
  // data follows
  // uint16_t checksum = (last 2 bytes)

  LOG_INFO("transaction: %u / protocol: %u / length: %u",
           transaction, protocol, length);
  LOG_INFO("unit: %u / function: %u", unit, function);

  if (function == 3)
  {
    LOG_INFO("modbus function: Read Holding Registers");
    uint8_t host = ntohl(ip_header->ip_src.s_addr) & 0x000000FF;

    if (host == 0x01) {
      // requesting registers
      
      uint16_t base_register = (data[8] << 8) | data[9];
      uint16_t word_count = (data[10] << 8) | data[11];
      LOG_INFO("PVS requesting %u words from base register %u", word_count, base_register);
      /*
       TODO:  store sequence info and base register and word count so we can match those up
       with the response.  kmo 11 dec 2023 17h34
      */
    } else {
      LOG_INFO("likely response from host %u", host);
      // TODO:  compare with past sequence/base register/word count data to establish
      // format of data here.  kmo 11 dec 17h35
    }
  }
  else if (function == 14)
  {
    LOG_INFO("modbus function: Write Multiple Registers");
    // TODO:  implement.
  }
  else
  {
    LOG_WARN("surprised to find modbus function %u in the capture", function);
  }
}

int add_filter(pcap_t* pcap) {
  LOG_INFO("compiling filter...");
  // acquire all modbus over TCP packets
  const char* filter_expression = "tcp port 502";
  struct bpf_program filter_program;
  const int optimize = 0;
  const bpf_u_int32 netmask = 0;

  const int compilation_result = pcap_compile(pcap, &filter_program, filter_expression, optimize, netmask);
  if (compilation_result != 0)
  {
    LOG_ERROR("could not compile bpf_program from expression %s", filter_expression);
    return 1;
  }

  const int setfilter_result = pcap_setfilter(pcap, &filter_program);
  if (setfilter_result != 0)
  {
    LOG_ERROR("could not set filter on pcap");
    return 2;
  }

  return setfilter_result;
}
