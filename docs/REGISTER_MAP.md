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

Unit 10 (Inverter-charger) Registers

| Register | Name                                                                       | Data Type | Unit           | Scale | R/W |
|----------|----------------------------------------------------------------------------|-----------|----------------|-------|-----|
| 40213    | External power control command                                             | uint8     |                |       | rw  |
| ...      |                                                                            |           |                |       |     |
| 40220    | External power control Maximum Discharge Power                             | uint16    | percent        | 0.01  | rw  |
| 40221    | External power control Maximum Charge Power                                | uint16    | percent        | 0.01  | rw  |
| ...      |                                                                            |           |                |       |     |
| 40236    | Energy at the transformer, lifetime - charging                             | uint32    | kilowatt-hours | 0.001 | r   |
| 40237    | (40236 continued)                                                          |           |                |       |     |
| 40238    | Max charge power                                                           | uint16    | watts          | 1.0   | r   |
| 40239    | Max discharge power                                                        | uint16    | watts          | 1.0   | r   |
| 40240    | Max charge/discharge power scaling factor                                  | uint16    | -              | -     | r   |
| 40241    | Operating Mode *                                                           | uint16    | bitmask        | -     | rw  |
| 40242    | Battery sensor count                                                       | uint16    | -              | -     | r
| 40243    | Battery sensor config mode                                                 | uint16    | bitmask?       | -     | rw  |
| 40244    | Inverter clear fault command                                               | uint16    | bitmask?       | -     | rw  |
| 40245    | AC PV Charge SOC Limit                                                     | uint16    | percent        | 0.01  | rw  |
| 40246    | High SOC Cut Out                                                           | sint16    | percent        | 0.01  | rw  |
| 40247    | Recharge SOC                                                               | sint16    | percent        | 0.01  | rw  |
| 40248    | Low SOC Cut Out                                                            | sint16    | percent        | 0.01  | rw  |
| 40249    | SOC Scaling Factor                                                         | sint16    | -              | -     | r   |
| 40250    | Fail-over action on loss of SunSpec Controller heartbeat *                 | uint8     | enumeration    | -     | rw  |
| 40251    | Battery Assocation *                                                       | uint16    | enumeration    | -     | rw  |
| 40252    | Inverter Status *                                                          | uint16    | enumeration    | -     | r   |
| 40253    | Charger Status *                                                           | uint16    | enumeration    | -     | r   |

* 40241 - Operating Mode
2 - Standby
3 - Operating

* 40250 - Fail-over action on loss of SunSpec Controller heartbeat
0 - Heartbeat disabled
1 - Do Nothing
2 - Autonomous Operation
3 - AC Passthrough

* 40251 - Battery Assocation
3 - House Battery Bank 1
4 - House Battery Bank 2
5 - House Battery Bank 3
6 - House Battery Bank 4
7 - House Battery Bank 5

* 40252 - Inverter Status
1024 - Invert
1025 - AC Pass Through
1026 - APS Only
1027 - Load Sense
1028 - Inverter Disabled
1029 - Load Sense Ready
1030 - Engaging Inverter
1031 - Inverter Fault
1032 - Inverter Standby
1033 - Grid-tied
1034 - Grid Support
1035 - Gen Support
1036 - Sell-to-grid
1037 - Load Shaving
1038 - Grid Frequency Stabilization
1039 - AC Coupling
1040 - Reverse Ibatt

* 40253 - Charger Status
768 - Not Charging
769 - Bulk
770 - Absorption
771 - Overcharge
772 - Equalize
773 - Float
774 - No Float
775 - Constant VI
776 - Charger Disabled
777 - Qualifying AC
778 - Qualifying APS
779 - Engaging Charger
780 - Charge Fault
781 - Charger Suspend
782 - AC Good
783 - APS Good
784 - AC Fault
785 - Charge
786 - Absorption Exit Pending
787 - Ground Fault
788 - AC Good Pending 

Unit 230 (Battery) Registers

(To date, have not found documentation.)

[ 40081 - 40092 ]
[ 40104 - 40131 ]
[ 40136 - 40139 ]