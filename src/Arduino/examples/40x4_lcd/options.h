#ifndef OPTIONS_H
#define OPTIONS_H

// Memory-saving options: Comment these out if you do not need them.
// It will save a slight bit of SRAM, which is very precious on 8-bit AVRs.
// #define BMS_OPTION_PRODUCTION_DATE      // Comment this out if reading the production date is not needed.
// #define BMS_OPTION_SW_VERSION           // Comment this out if reading the software version is not needed.
// #define BMS_OPTION_NAME                 // Comment this out if reading the BMS name is not needed.
#define BMS_OPTION_FAULT_COUNTS         // Comment this out to not store the number of times a fault occurred.
//#define BMS_OPTION_DEBUG                // Uncomment this to output debug logging data on the Serial monitor port.
//#define BMS_OPTION_DEBUG_STATE_MACHINE  // Uncomment this to output debug the state machine


#define BMS_TIMEOUT         1000  // The longest time to wait, in milliseconds for a response

#define BMS_MAX_CELLS       8  // Preallocates this number of cells voltages in the array
#define BMS_MAX_NTCs        2  // Preallocates this number of temperatures in the array
#define BMS_MAX_RX_DATA_LEN 64  // Preallocates this number of bytes to store RX data field


#endif  // OPTIONS_H 
