//
// Created by koleson on 12/10/23.
//

#include <pcap/pcap.h>

#ifndef PACKET_PROCESSING_H
#define PACKET_PROCESSING_H

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif //PACKET_PROCESSING_H
