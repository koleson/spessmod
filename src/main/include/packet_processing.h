//
// Created by koleson on 12/10/23.
//

#ifndef PACKET_PROCESSING_H
#define PACKET_PROCESSING_H

#include <pcap/pcap.h>



struct Modbus_Response_Context {
  uint64_t pcap_packet_number;
  uint16_t base_register;
};

struct Modbus_Response_Data {
  uint16_t transaction;
  uint16_t protocol;
  uint16_t length;
  uint8_t unit;
  uint8_t function_code;
  uint8_t byte_count;
  uint16_t register_data[];
};

struct Modbus_Response {
  struct Modbus_Response_Context* context;
  struct Modbus_Response_Data* data;
};

typedef void (*ModbusResponseProcessor)(struct Modbus_Response*);

// TODO:  method to free Modbus_Response (really, register_data)

void print_struct_sizes();
int add_filter(pcap_t* pcap);
void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void add_response_processor(ModbusResponseProcessor processor);

#endif //PACKET_PROCESSING_H
