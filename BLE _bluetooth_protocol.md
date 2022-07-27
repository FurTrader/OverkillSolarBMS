# BLE protocol documentation
### Bluetooth module protocol for JBD BMSs

NOTE: I am transcribing my written notes a few months after doing this work and I may be forgetting the meaning of certain things. -ST

We reverse engineered the BLE (bluetooth low energy) protocol used by the JBD apps using a sniffer and wireshark.

We were then able to send data directly to the BMS UART via a Raspberry Pi with GATTTOOL and via an Arduino Wifi with the BLE library.

There are several versions of the BLE dongle in circulation.   
Typical:   
![BLE dongle photo](https://overkillsolar.com/wp-content/uploads/2021/12/Bluetoothmod-200x200.png)   
They all vary slightly in physical appearance.   
They all advertise one service UUID: FF00 with 2 properties: FF01 Read Notify and FF02 ReadWrite Without Response (according to lightblue)

* The older ones advertise characteristics of "READ" and "WRITE"  
* The current versions advertise characteristics of "READ" and "WRITE WITHOUT RESPONSE"   
