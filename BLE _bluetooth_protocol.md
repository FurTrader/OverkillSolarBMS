# BLE protocol documentation
### Bluetooth module protocol for JBD BMSs

NOTE: I am transcribing my written notes a few months after doing this work and I may be forgetting the meaning of certain things. -ST

We reverse engineered the BLE (bluetooth low energy) protocol using a sniffer and wireshark.

We were then able to send data directly to the BMS UART via a Raspberry Pi with GATTTOOL and via an Arduino Wifi with the BLE library.

There are several versions of the BLE dongle in circulation.   
Typical:   
![BLE dongle photo](https://overkillsolar.com/wp-content/uploads/2021/12/Bluetoothmod-200x200.png)   
They all vary slightly in physical appearance.   

They all advertise one service UUID: FF00 with 2 properties, "FF01 Read Notify" and "FF02 Read Write Without Response" (according to lightblue)   
* The older ones advertise characteristics of "READ" and "WRITE"  
* The current versions advertise characteristics of "READ" and "WRITE WITHOUT RESPONSE"   


Characteristic FF02 is also called 0x0015 in wireshark.   
I dont know why but near as I can tell, FF02 and 0x0015 are the same thing.    
characteristic FF01 is also known as 0x0011.

To send data to the BMS, write to characteristic 0x0015.   
The payload data will be relayed to the UART.

Any data that comes into the BLE module from the UART will be returned in a notification from characteristic FF01, also known as 0x0011.    
Some messages (the basic info message) are broken up and returned in 2 consecutive notifications
