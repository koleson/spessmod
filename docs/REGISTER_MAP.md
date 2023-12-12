The following documents registers I've actually seen in use by the system.  Anything in the documentation but unseen in practice is omitted here for brevity.

Unit 1:  Schneider Electric Conext Gateway (later units use similar but different gateways)
Unit 10:  Schneider Electric XW Pro Inverter/Charger
Unit 230:  Battery

Unit 1 (Gateway) Registers

| Register | Name                                                        | Data Type | Unit    | Scale | R/W |
|----------|-------------------------------------------------------------|-----------|-------------|-------|-----|
| 400073   | Inverter-charger power module AC current - Phase A          | uint16    | volts       | 0.01  | r   |
| 400074   | Inverter-charger power module AC current - Phase B          | uint16    | volts       | 0.01  | r   |
| 400075   | Inverter-charger power module AC current - Phase C          | uint16    | volts       | 0.01  | r   |
| 400076   | Inverter-charger power module AC current scaling factor     | sint16    | -           | -     | r   |
| ...      |                                                             |           |             |       |     |
| 400080   | Inverter-charger power module phase voltage A               | uint16    | amperes     | 0.01  | r   |
| 400081   | Inverter-charger power module phase voltage B               | uint16    | amperes     | 0.01  | r   |
| 400082   | Inverter-charger power module phase voltage C               | uint16    | amperes     | 0.01  | r   |
| 400083   | Inverter-charger power module phase voltage scaling factor  | uint16    | -           | -     | r   |
| 400084   | Inverter-charger power module total AC power                | sint16    | watts       | 1.0   | r   |
| 400085   | Inverter-charger power module total AC power scaling factor | sint16    | -           | -     | r   |
| 400086   | Inverter-charger power module frequency                     | uint16    | hertz       | 0.01  | r   |
| 400087   | Inverter-charger power module frequency scaling factor      | sint16    | -           | -     | r   |
| 400088   | Inverter-charger power module apparent power                | sint16    | volt-ampere | 1.0   | r   |