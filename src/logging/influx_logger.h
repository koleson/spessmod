#include <stdint.h>
#include "packet_processing.h"

void influx_log_test();
void influx_log_raw(uint8_t unit, uint16_t register_num, uint16_t value);
void influx_log_response(struct Modbus_Response*);