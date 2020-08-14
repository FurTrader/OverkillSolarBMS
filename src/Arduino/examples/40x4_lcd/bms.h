/* Copyright 2020 Neil Jansen (njansen1@gmail.com)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef BMS_H
#define BMS_H

#include "Arduino.h"
#include "options.h"

// Constants
#define BMS_STARTBYTE 0xDD
#define BMS_STOPBYTE  0x77
#define BMS_READ      0xA5
#define BMS_WRITE     0x5A

// Command codes (registers)
#define BMS_REG_BASIC_SYSTEM_INFO 0x03
#define BMS_REG_CELL_VOLTAGES     0x04
#define BMS_REG_NAME              0x05
#define BMS_REG_CTL_MOSFET        0xE1

// State machine states
#define BMS_STATE_WAIT_FOR_START_BYTE   0x00
#define BMS_STATE_WAIT_FOR_STATUS_BYTE  0x01
#define BMS_STATE_WAIT_FOR_CMD_CODE     0x02
#define BMS_STATE_WAIT_FOR_LENGTH       0x03
#define BMS_STATE_WAIT_FOR_DATA         0x04
#define BMS_STATE_WAIT_FOR_CHECKSUM_MSB 0x05
#define BMS_STATE_WAIT_FOR_CHECKSUM_LSB 0x06
#define BMS_STATE_WAIT_FOR_STOP_BYTE    0x07
#define BMS_STATE_ERROR                 0xFF


#ifdef BMS_OPTION_SW_VERSION
    typedef struct SoftwareVersion {
        uint8_t major;
        uint8_t minor;
    } SoftwareVersion;
#endif


#ifdef BMS_OPTION_PRODUCTION_DATE
    typedef struct ProductionDate {
        uint16_t year;
        uint8_t month;
        uint8_t day;
    } ProductionDate;
#endif

typedef struct ProtectionStatus {
    bool single_cell_overvoltage_protection;
    bool single_cell_undervoltage_protection;
    bool whole_pack_overvoltage_protection;
    bool whole_pack_undervoltage_protection;
    bool charging_over_temperature_protection;
    bool charging_low_temperature_protection;
    bool discharge_over_temperature_protection;
    bool discharge_low_temperature_protection;
    bool charging_overcurrent_protection;
    bool discharge_overcurrent_protection;
    bool short_circuit_protection;
    bool front_end_detection_ic_error;
    bool software_lock_mos;
} ProtectionStatus;

#ifdef BMS_OPTION_FAULT_COUNTS
typedef struct FaultCount {
    uint8_t single_cell_overvoltage_protection;
    uint8_t single_cell_undervoltage_protection;
    uint8_t whole_pack_overvoltage_protection;
    uint8_t whole_pack_undervoltage_protection;
    uint8_t charging_over_temperature_protection;
    uint8_t charging_low_temperature_protection;
    uint8_t discharge_over_temperature_protection;
    uint8_t discharge_low_temperature_protection;
    uint8_t charging_overcurrent_protection;
    uint8_t discharge_overcurrent_protection;
    uint8_t short_circuit_protection;
    uint8_t front_end_detection_ic_error;
    uint8_t software_lock_mos;
} FaultCount;
#endif


class OverkillSolarBms
{
public:
    OverkillSolarBms();  // constructor

    // #######################################################################
    // Top-level control

    void begin(Stream *port);  // Start processing.  Call this in the sketch's setup() function
    void end();    // End processing.  Call this to stop querying the BMS and processing data.

    void main_task(); // Call this as fast as possible within the sketch's loop() function

    void set_query_rate(uint16_t rate);  // Set the 

    bool get_comm_error_state();  // Returns true if the BMS is not responding

    // #######################################################################
    // 0x03 Basic Info

    float get_voltage();  // Returns the total voltage, in volts
    float get_current();  // Returns the instantaneous current, in amps
    float get_balance_capacity();  // Returns the balance capacity, in amp hours
    float get_rate_capacity();  // Returns the rate capacity, in amp hours
    uint16_t get_cycle_count();  // Returns the cycle count (number of charge/discharge cycles)

#ifdef BMS_OPTION_PRODUCTION_DATE
    ProductionDate get_production_date();  // Return the production date (day, month, year)
#endif

    bool get_balance_status(uint8_t cell_index);  // Returns the balance status of the specified cell index
    ProtectionStatus get_protection_status(); // Returns the protection status
    bool get_protection_status_summary();  // Returns True if any protection status bits are currently active

#ifdef BMS_OPTION_FAULT_COUNTS
    FaultCount get_fault_counts();
    void clear_fault_counts();
#endif

#ifdef BMS_OPTION_SW_VERSION
    SoftwareVersion get_software_version();  // Returns the software version (major, minor)
#endif
    uint8_t get_state_of_charge();  // retursn the state of charge, in percent (0-100)
    bool get_discharge_mosfet_status();  // Returns true if the discharge FET is enabled
    bool get_charge_mosfet_status();  // Returns true if the charge FET is enabled
    uint8_t get_num_cells();  // Returns the # of cells in which the BMS is configured for
    uint8_t get_num_ntcs();  // Returns the # of temperature sensors
    float get_ntc_temperature(uint8_t ntc_index);  // Returns the temperature, in celsius, of the specified temp sensor index

    // #######################################################################
    // 0x04 Cell Voltages

    float get_cell_voltage(uint8_t cell_index);  // Returns the cell voltage, in volts, of the specified cell index

    // #######################################################################
    // 0x05 BMS Name

#ifdef BMS_OPTION_NAME
    String get_bms_name();  // Returns the BMS name
#endif

    // #######################################################################
    // 0xE1 MOSFET Control

    void set_mosfet_control(bool charge, bool discharge);  // Controls the charge and discharge MOSFETs

    // #######################################################################
    // Misc

#ifdef BMS_OPTION_DEBUG
    void debug();  // Calling this method will print out the received data to the main serial port
#endif
    uint8_t get_rx_errors();  // Returns the # of RX errors that have occured while trying to 

private:
    bool is_initialized;
    Stream* serial;  // Reference to the BMS serial port

    // Store the data internally in raw format, as received from the BMS.
    // This used considerably less memory than converting it to the desired
    // format when it is received.
    uint16_t voltage;  // The total voltage, stored as units of 10 mV
    int16_t current;  // The total current, stored in units 10mA 
    uint16_t balance_capacity;
    uint16_t rate_capacity;
    uint16_t cycle_count;
#ifdef BMS_OPTION_PRODUCTION_DATE
    uint16_t production_date;
#endif
    uint32_t balance_status;  // The cell balance statuses, stored as a bitfield
    uint16_t protection_status;
#ifdef BMS_OPTION_FAULT_COUNTS
    FaultCount fault_count;  // Optional tally of faults from power-up
    uint16_t last_protection_status;
    bool has_new_fault_occured(uint8_t index);
#endif
#ifdef BMS_OPTION_SW_VERSION
    uint8_t software_version;
#endif
    uint8_t remaining_soc;
    uint8_t mosfet_status;
    uint8_t num_cells;
    uint8_t num_ntcs;

    uint16_t ntc_temps[BMS_MAX_NTCs];
    uint16_t cell_voltages[BMS_MAX_CELLS];

#ifdef BMS_OPTION_NAME
    String bms_name;
#endif

    // Background tasks
    void serial_rx_task();
    uint32_t last_query_time;
    uint32_t last_10ms_time;

    // RX framer, state data
    uint8_t rx_state;  // RX framer state machine state
    uint8_t rx_cmd_code;  // current RX frame's command code (register)
    uint8_t rx_status;  // current RX frame's status
    uint8_t rx_length;  // current RX frame's length
    uint8_t rx_data[BMS_MAX_RX_DATA_LEN];  // current RX frame's data
    uint8_t rx_data_index;  // Current RX frame's data index (starts at zero)
    uint16_t rx_checksum;  // current RX frame's checksum
    uint8_t num_rx_errors;  // Current number of RX framing errors encountered

    uint16_t tx_query_rate;
    uint32_t last_0x03_timestamp;
    uint32_t last_0x04_timestamp;

    // Low-level communication methods:
    void write(bool read, uint8_t command_code, uint8_t* data, uint8_t length);  // Write to BMS
    void query_basic_info();
    void query_cell_voltages();

};

#endif  // BMS_H 
