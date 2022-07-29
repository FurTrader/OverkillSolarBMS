# Capacity Parameter Registers

It was recently brought to our attention that the newest JBD desktop app lists capacity parameters in 10 percent increments.   
As far as we can remember, all of the apps used to list these parameters in 20 percent increments.    

I sniffed out the registers using JBDTools -V2.9-20210524 to change each capacity parameter.   

First, I tried a 4s BMS with firmware version 0x21.    
Results:    

| parameter | Register |
| :----: | :----: |
| 10% | 0x46 |
| 20% | 0x35 |
| 30% | 0x45 |
| 40% | 0x34 |
| 50% | 0x44 |
| 60% | 0x33 |
| 70% | 0x43 |
| 80% | 0x32 |
| 90% | 0x42 |
| 100% | 0x47 |

 
