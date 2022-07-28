# BLE protocol documentation
### Bluetooth module protocol for JBD BMSs

___
### Executive Summary

Look for a BLE module advertising servive UUID FF00
 
* Characteristic FF01 is Read/Notify from the UART   
 *The module will notify when this value changes via the UART RX*

* Characteristic FF02 is Write Without Response to the UART   
 *The module will transmit on the UART when this value changes*     
 *Can also be used to send instructions to the BLE module itself*
___

NOTE: I am transcribing my written notes a few months after doing this work and I may be forgetting the meaning of certain things.   
All numbers are hexidecimal unless specified otherwise -ST    

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

# Examples:

### Normal BMS communication

The BMS never sends anything without a request

__Request basic info__
Write the following data to 0x0015:   
dd a5 03 00 ff fd 77   


header | command | payload | checksum | footer
| :---: | :---: | :---: |:---:| :---: |
| dd | a5 | 03 00 | ff fd | 77 |
| always the same   |read|0x03 basic info| sum of payload bytes subtracted from 0x10000 | always the same |


__Basic info response__
The response comes back as 2 consecutive notifications to 0x0011.   
Shorter responses come back as one notification.    
Typical response looks like: dd 03 00 1b 05 34 00 00 19 ab 27 10 00 00 2a 75 00 00 00 00 00 00 20 42 03 04 02 09 7f 0b a3 fc 71 77   
header | register | length | data | checksum | footer
| :---: | :---: | :---: |:---:| :---: | :---: |
| dd | 03 | 00 1b | 05 34 00 00 19 ab 27 10 00 00 2a 75 00 00 00 00 00 00 20 42 03 04 02 09 7f 0b a3 | fc 71 | 77   
| always the same   | 0x03 basic info | 0x1b bytes to follow | data | sum of payload bytes subtracted from 0x10000 | always the same |

### BLE Module Name Change

The module's advertising name can be changed by sending a message to 0x0015    
The formatting of this message differs from normal BMS communication, and the BMS will ignore it.   
The notification responses come only from the BLE module.

__Change the advertising name to "XYZ"__
Write the following data to 0x0015:    
ff aa 07 03 58 59 5a 15    

header | command | length | payload | checksum 
| :---:| :---: | :---: | :---: |:---:|
| ff aa | 07 | 03 | 58 59 5a | 15 |
|    |    |    | the new name | modulo 256 of all bytes between header and checksum |

The BLE module responds with a notification on 0x0011     
This is always the same, I assume it means "ok" :     
ff aa 07 00 07

header | command | length | payload | checksum 
| :---:| :---: | :---: | :---: |:---:|
| ff aa | 07 | 00 |  | 07 |
|    |    |    | empty | modulo 256 of all bytes between header and checksum |

# Checksum calculations

### BMS communications

The BMS communications include a 2 byte checksum which is the sum of the payload bytes subracted from 0x1000, all in hex

Example:   
Request basic info    
| dd a5 | 03 00 | ff fd 77
| :---:| :---: | :---: |
|  | checksum of these bytes |  |

sum(03 00) = 0x03    
0x10000 - 0x03 = 0xffdd    
Checksum = 0xFFDD     

Example:
Basic info response
| dd 03 00 | 1b 05 34 00 00 19 ab 27 10 00 00 2a 75 00 00 00 00 00 00 20 42 03 04 02 09 7f 0b a3 | fc 71 77 |
| :---:| :---: | :---: |
|  | checksum of these bytes |  |

sum(1b 05 34 00 00 19 ab 27 10 00 00 2a 75 00 00 00 00 00 00 20 42 03 04 02 09 7f 0b a3) = 0x38f    
0x10000 - 0x38f = 0xfc71    
Checksum = 0xfc71    

There are a few messages that don't follow this pattern. The BMS sometimes responds with a "ok" or "access denied" message that doesnt have a valid checksum.

### BLE module communications

Messages that are directed at the BLE module use a different checksum called modulo 256.

"modulo" is the remainder in a division operation.

To calculate the checksum, add all the bytes between the header (ff) and checksum. Divide this by 0x100 (decimal 256).
The remainder is the checksum.

Example:   
Send a name change to the BLE module
| ff aa | 07 03 58 59 5a | 15 |
| :---:| :---: | :---: |
|  | calculate modulo 256 of these bytes |  |

sum(07 03 58 59 5a) = 0x115    
0x115 / 0x100 = 0x01 modulo 0x15    
Checksum = 0x15
