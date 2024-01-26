This tool can be used to log Modbus data exchanges between Modbus over TCP devices without any risk of modifying their operation.

It relies on use of a mirroring switch.

In particular, I use it to monitor traffic between a Schneider Conext Gateway (connected to a Schneider XW Pro Inverter/Charger) and another device.

Currently, the register map is mostly for reference, but can be used to make the data logged to the console more intelligible.

The overarching goal here is to understand the interaction between these devices such that if one needed to be replaced, I could build or source such a device quickly and reliably.

See the `PLAN` file for more details on what steps I'm taking to further that goal!
