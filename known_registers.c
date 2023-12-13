#include "known_registers.h"
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define NUM_KNOWN_REGISTERS 10
struct KnownRegister *known_registers[NUM_KNOWN_REGISTERS];
bool known_registers_populated = false;

// TODO:  probably want to use a CSV or something to populate this.
void populate_known_registers()
{
  struct KnownRegister* icpm_ac_current_A = (struct KnownRegister *)malloc(sizeof(struct KnownRegister));

  icpm_ac_current_A->unit_number = 1;
  icpm_ac_current_A->register_number = 40073;
  icpm_ac_current_A->data_type = uint16_t_type;
  icpm_ac_current_A->readable = true;
  icpm_ac_current_A->writeable = false;

  char *name = "Inverter-charger power module AC current - Phase A";
  icpm_ac_current_A->name = (char *)malloc(strlen(name) + 1);
  strcpy(icpm_ac_current_A->name, name);

  known_registers[0] = icpm_ac_current_A;

  struct KnownRegister* icpm_input_energy_lifetime = (struct KnownRegister*)malloc(sizeof(struct KnownRegister));
  icpm_input_energy_lifetime->unit_number = 1;
  icpm_input_energy_lifetime->register_number = 40310;
  icpm_input_energy_lifetime->data_type = uint32_t_type;
  icpm_input_energy_lifetime->readable = true;
  icpm_input_energy_lifetime->writeable = false;

  char *name_e_life = "Inverter-charger power module input energy, lifetime";
  icpm_input_energy_lifetime->name = (char *)malloc(strlen(name_e_life) + 1);
  strcpy(icpm_input_energy_lifetime->name, name_e_life);

  known_registers[1] = icpm_input_energy_lifetime;

  known_registers_populated = true;
}

struct KnownRegister *register_for_unit_and_number(uint8_t unit, uint16_t number)
{
  if (!known_registers_populated)
  {
    populate_known_registers();
  }

  for (unsigned int i = 0; i < NUM_KNOWN_REGISTERS; i++)
  {
    struct KnownRegister *this_register = known_registers[i];

    if (this_register != NULL && this_register->unit_number == unit && this_register->register_number == number)
    {
      return this_register;
    }
  }
  return NULL;
}