//
// Created by koleson on 12/10/23.
//

#ifndef PACKET_PROCESSING_H
#define PACKET_PROCESSING_H

#include <pcap/pcap.h>

void print_struct_sizes();
int add_filter(pcap_t* pcap);
void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif //PACKET_PROCESSING_H
