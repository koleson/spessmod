#include <stdint.h>
#include <stdbool.h>

#ifndef KNOWN_REGISTERS_H
#define KNOWN_REGISTERS_H

enum RegisterDataType {
  uint8_t_type = 0,
  uint16_t_type,
  uint32_t_type,
  sint16_t_type,
  bitmask_16_type
};

enum RegisterValueUnit {
  volts = 0,
  amperes,
  watts,
  hertz,
  percent,
  kilowatt_hours
};

struct KnownRegister {
  uint8_t unit_number;
  uint16_t register_number;
  enum RegisterDataType data_type;
  enum RegisterValueUnit value_unit;
  int16_t scale_factor;     // i think this is expressed as *10^scale_factor.  kmo 12 dec 2023 21h15
  bool writeable;
  bool readable;
  char* name;
};

struct KnownRegister* register_for_unit_and_number(uint8_t unit, uint16_t number);

#endif  // KNOWN_REGISTERS_H
