//
// Created by koleson on 12/10/23.
//

#ifndef CAPTURE_SETUP_H
#define CAPTURE_SETUP_H

#include <pcap.h>

// using args, chooses between a file-based and interface-based pcap.
pcap_t* get_pcap(const int argc, const char **argv, char *errbuf);

#endif //CAPTURE_SETUP_H
