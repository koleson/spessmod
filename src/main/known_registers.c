#include "known_registers.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "log.h"

/* private forward declarations */
struct KnownRegisterIntermediate;
void read_csv(const char* filename);
struct KnownRegister* known_register_from_intermediate(struct KnownRegisterIntermediate*);
/* end private forward declarations */

#define MAX_KNOWN_REGISTERS 100
struct KnownRegister *known_registers[MAX_KNOWN_REGISTERS];
bool known_registers_populated = false;

void populate_known_registers()
{
  // TODO:  pass pointer to valid struct array
  read_csv("../docs/Register_Map.csv");

  // can hard-code specific registers here if parsing not supported, but
  // really should just update the parser.
  // kmo 24 dec 2023 13h31

  // if just testing known registers, can stop here
  // exit(1);

  known_registers_populated = true;
}

struct KnownRegister *register_for_unit_and_number(uint8_t unit, uint16_t number)
{
  if (!known_registers_populated)
  {
    populate_known_registers();
  }

  for (unsigned int i = 0; i < MAX_KNOWN_REGISTERS; i++)
  {
    struct KnownRegister *this_register = known_registers[i];

    if (this_register != NULL && this_register->unit_number == unit && this_register->register_number == number)
    {
      return this_register;
    }
  }
  return NULL;
}

// intermediate format with string data types for CSV intake
struct KnownRegisterIntermediate {
  char unit_number_string[16];
  char register_number_string[16];
  char name_string[256];
  char data_type_string[16];
  char value_unit_string[64];
  char value_scale_string[16];
  char read_write_string[8];
};

void read_csv(const char* filename) {
  FILE* file = fopen(filename, "r");
  char line[sizeof(struct KnownRegisterIntermediate)];

  
  if (file == NULL) {
    LOG_ERROR("known_registers.c: could not open file %s", filename);
    // program can still run, it just won't know anything about what the registers mean.
    return;
  }

  fgets(line, sizeof(struct KnownRegisterIntermediate), file);
  if (line == NULL) {
    LOG_ERROR("known_register.c: could not read line from file %s", filename);
    fclose(file);
    return;
  }
  // check file structure via headers
  {
    char* current_token;
    char* cursor = line;
    struct KnownRegisterIntermediate intermediate;

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "UNIT_NUMBER") == 0);

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "REGISTER_NUMBER") == 0);

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "NAME") == 0);

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "DATA_TYPE") == 0);

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "VALUE_UNIT") == 0);

    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "VALUE_SCALE") == 0);

    // last field:  include `\n` to assert it's the end of the line.
    // this will be significant when reading the last value as well.
    // alternative:  strip whitespace at every step.
    // READ_WRITE is a good candidate for last value since
    // we are just looking for presence of `r` and `w` - whitespace insignifcant.
    current_token = strtok_r(cursor, ",", &cursor);
    assert(strcmp(current_token, "READ_WRITE\n") == 0);
  }

  // import lines to intermediate format
  char* current_token;
  char* cursor;
  int valid_register = 0;
  while (fgets(line, sizeof(struct KnownRegisterIntermediate), file))
  {
    struct KnownRegisterIntermediate intermediate_record;
    LOG_INFO("ingesting line: %s", line);
    cursor = line;

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.unit_number_string, current_token);

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.register_number_string, current_token);

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.name_string, current_token);

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.data_type_string, current_token);

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.value_unit_string, current_token);
    
    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.value_scale_string, current_token);

    current_token = strtok_r(cursor, ",", &cursor);
    strcpy(intermediate_record.read_write_string, current_token);

    struct KnownRegister* known_register = known_register_from_intermediate(&intermediate_record);
    
    LOG_INFO("done converting register %u", valid_register);
    
    known_registers[valid_register] = known_register;
    
    valid_register++;
  }

  LOG_INFO("imported %d known registers", valid_register);
}

struct KnownRegister* known_register_from_intermediate(struct KnownRegisterIntermediate* intermediate) {
  // INCOMPLETE
  struct KnownRegister* const current_register = (struct KnownRegister* const)malloc(sizeof(struct KnownRegister));
  current_register->data_type = uint16_t_type;
  
  uint8_t intermediate_unit_num;
  int sscanf_result_unit_number = sscanf(intermediate->unit_number_string, "%u", &intermediate_unit_num);
  
  if (sscanf_result_unit_number == 1) 
  {
    // assigning in here, current_register is a different value for some reason?
    LOG_DEBUG("intermediate_unit_num = %u", intermediate_unit_num);
    current_register->unit_number = intermediate_unit_num;
    LOG_DEBUG("current register 6: %p", current_register);
  }
  else
  {
    LOG_ERROR("intermediate conversion: couldn't get unit number for intermediate %s", intermediate->unit_number_string);
    return NULL;
  }

  
  LOG_INFO("intermediate conversion: got unit number %u", current_register->unit_number);

  uint16_t intermediate_register_num;
  int sscanf_result_register_number = sscanf(intermediate->register_number_string, "%u", &intermediate_register_num);
  if (sscanf_result_register_number == 1) 
  {
    LOG_DEBUG("intermediate_register_num = %u", intermediate_register_num);
    LOG_DEBUG("current register 11: %p", current_register);

    // if we try to assign current_register->register_number = intermediate_register_num here, we segfault ????
     current_register->register_number = intermediate_register_num;
  }
  else
  {
    LOG_ERROR("intermediate conversion: couldn't get register number for intermediate %s", intermediate->register_number_string);
    return NULL;
  }
  // current_register->register_number = intermediate_register_num;
  LOG_DEBUG("register_number assigned successfully");
  LOG_INFO("intermediate conversion: got register number %u", current_register->register_number);
  LOG_DEBUG("current register 12: %p", current_register);
  
  // data_type
  // value_unit
  // scale_factor

  int16_t intermediate_value_scale_num;
  int sscanf_result_value_scale_number = sscanf(intermediate->value_scale_string, "%d", &intermediate_value_scale_num);
  
  if (sscanf_result_value_scale_number == 1) 
  {
    // nothing
    // testing segfault here - didn't segfault
    current_register->scale_factor = intermediate_value_scale_num;
  }
  else 
  {
    // is it a slash?  if so, ignore the value (set to 1)
    if (strcmp(intermediate->value_scale_string, "-") == 0) 
    {
      intermediate_value_scale_num = 0; // value * 10^0, which is value * 1, which is value
    } else 
    {
      LOG_DEBUG("intermediate conversion:  couldn't get value scale number for intermediate %s", intermediate->value_scale_string);
      return NULL;
    }
    
  }
  current_register->scale_factor = intermediate_value_scale_num;

  current_register->data_type = uint16_t_type;
  current_register->readable = true;
  current_register->writeable = false;
  

  char* name = intermediate->name_string;
  current_register->name = (char *)malloc(strlen(name) + 1);
  strcpy(current_register->name, name);

  LOG_DEBUG("done converting intermediate register");
  print_known_register(current_register);

  return current_register;
}

char* string_for_value_unit(enum RegisterValueUnit value_unit) {
  // INCOMPLETE
  return "no string yet for value unit";
}

char* string_for_data_type(enum RegisterDataType data_type) {
  // INCOMPLETE
  switch (data_type)
  {
  case uint16_t_type:
    return "uint16";
  case uint32_t_type:
    return "uint32";
  case sint16_t_type:
    return "sint16";
  }
}

void print_known_register(const struct KnownRegister* known_register) {
  LOG_DEBUG("known reg %u on unit %u: %s\n", 
  known_register->register_number, known_register->unit_number, known_register->name);
  LOG_DEBUG("data: type %s, value_unit: %s, value_scale: 10^%u, readable: %s, writeable: %s\n",
  string_for_data_type(known_register->data_type), 
  string_for_value_unit(known_register->value_unit), 
  known_register->scale_factor,
  (known_register->readable ? "true" : "false"), 
  (known_register->writeable ? "true" : "false"));
}