The following documents registers I've actually seen in use by the system.  Anything in the documentation but unseen in practice is omitted here for brevity.

Unit 1:  Schneider Electric Conext Gateway (later units use similar but different gateways)
Unit 10:  Schneider Electric XW Pro Inverter/Charger
Unit 230:  Battery

Unit 1 (Gateway) Registers

| Register | Name                                                                       | Data Type | Unit        | Scale | R/W |
|----------|----------------------------------------------------------------------------|-----------|-------------|-------|-----|
| 40073    | Inverter-charger power module AC current - Phase A                         | uint16    | volts       | 0.01  | r   |
| 40074    | Inverter-charger power module AC current - Phase B                         | uint16    | volts       | 0.01  | r   |
| 40075    | Inverter-charger power module AC current - Phase C                         | uint16    | volts       | 0.01  | r   |
| 40076    | Inverter-charger power module AC current scaling factor                    | sint16    | -           | -     | r   |
| ..       |                                                                            |           |             |       |     |
| 40080    | Inverter-charger power module phase voltage A                              | uint16    | amperes     | 0.01  | r   |
| 40081    | Inverter-charger power module phase voltage B                              | uint16    | amperes     | 0.01  | r   |
| 40082    | Inverter-charger power module phase voltage C                              | uint16    | amperes     | 0.01  | r   |
| 40083    | Inverter-charger power module phase voltage scaling factor                 | uint16    | -           | -     | r   |
| 40084    | Inverter-charger power module total AC power                               | sint16    | watts       | 1.0   | r   |
| 40085    | Inverter-charger power module total AC power scaling factor                | sint16    | -           | -     | r   |
| 40086    | Inverter-charger power module frequency                                    | uint16    | hertz       | 0.01  | r   |
| 40087    | Inverter-charger power module frequency scaling factor                     | sint16    | -           | -     | r   |
| 40088    | Inverter-charger power module apparent power                               | sint16    | volt-ampere | 1.0   | r   |
| 40089    | Inverter-charger power module apparent power scaling factor                | sint16    | -           | -     | r   |
| 40090    | Inverter-charger power module reactive power                               | sint16    | volt-ampere | 1.0   | r   |
| 40091    | Inverter-charger power module reactive power scaling factor                | sint16    | -           | -     | r   |
| 40092    | Inverter-charger power module power factor                                 | sint16    | percent     | 1.0   | r   |
| 40093    | Inverter-charger power module power factor scaling factor                  | sint16    | -           | -     | r   |
| 40094    | Inverter-charger power module output energy, lifetime                      | uint32    | kWh         | 0.001 | r   |
| 40095    | (40094 continued)                                                          |           |             |       |     |
| 40096    | Inverter-charger power module output energy, lifetime scaling factor       | sint16    | -           | -     | r   |
| ...      |                                                                            |           |             |       |     |
| 40310    | Inverter-charger power module input energy, lifetime                       | uint32    | kWh         | 0.001 | r   |
| 40311    | (40311 continued)                                                          |           |             |       |     |
| 40312    | Heartbeat Timeout                                                          | uint16    | seconds?    | 1.0   | rw  |
| 40313    | Heartbeat Counter                                                          | uint16    | -           | -     | rw  |