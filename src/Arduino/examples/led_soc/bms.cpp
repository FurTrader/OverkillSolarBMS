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

#include "bms.h"

// constructor
OverkillSolarBms::OverkillSolarBms() {
    is_initialized = false;

    voltage = 0;
    current = 0;
    balance_capacity = 0;
    rate_capacity = 0;
    cycle_count = 0;
#ifdef BMS_OPTION_PRODUCTION_DATE
    production_date = 0;
#endif
    balance_status = 0;
    protection_status = 0;
#ifdef BMS_OPTION_SW_VERSION
    software_version = 0;
#endif
    remaining_soc = 0;
    mosfet_status = 0;
    num_cells = 0;
    num_ntcs = 0;

    for (uint8_t i; i<BMS_MAX_NTCs; i++) {
        ntc_temps[i] = 0;
    }

    for (uint8_t i; i<BMS_MAX_CELLS; i++) {
        cell_voltages[i] = 0;
    }

#ifdef BMS_OPTION_NAME
    bms_name = String("");
#endif
}

// ###########################################################################

// Start processing
void OverkillSolarBms::begin(Stream *port) {
#ifdef BMS_OPTION_DEBUG
    Serial.println("OverkillSolarBMS Begin!");
#endif
    serial = port;
    is_initialized = true;
    rx_state = BMS_STATE_WAIT_FOR_START_BYTE;
    uint32_t now = millis();
    last_query_time = now;
    last_10ms_time = now;
    last_0x03_timestamp = now;
    last_0x04_timestamp = now;
    tx_query_rate = 1000;
}

// End processing
void OverkillSolarBms::end() {
    is_initialized = false;
}

// Call this as fast as possible within the sketch's loop() function
void OverkillSolarBms::main_task() {
    if (is_initialized) {
        uint32_t now = millis();

        // 10 millisecond task (serial data task)
        if (now - last_10ms_time >= 10) {
            serial_rx_task();
            last_10ms_time = now;
        }

        // query task
        if (now - last_query_time >= tx_query_rate) {
            query_basic_info();
            query_cell_voltages();
            last_query_time = now;
        }
    }
}

bool OverkillSolarBms::get_comm_error_state() {
    uint32_t now = millis();
    bool err_state = false;
    if (now - last_0x03_timestamp >= (tx_query_rate * 2)) {
        err_state = true;
    }
    if (now - last_0x04_timestamp >= (tx_query_rate * 2)) {
        err_state = true;
    }
    return err_state;
}

void OverkillSolarBms::set_query_rate(uint16_t rate) {
    tx_query_rate = rate;
}


// ###########################################################################
// 0x03 Basic Info & Status
// ###########################################################################

float OverkillSolarBms::get_voltage() {
    // Voltage is stored internally as a uint16_t in units of 10mV
    return voltage * 0.01;
}

float OverkillSolarBms::get_current() {
    // Current is stored internally as a uint16_t in units of 10mA

    // NOTE: There is an error in the API documentation:
    // When charging, the units are 10 mA.
    // When discharging, the units are 
    return current * 0.01;
}

float OverkillSolarBms::get_balance_capacity() {
    // Capacity is stored internally as a uint16_t in units of 10mAh
    return balance_capacity * 0.01;
}

float OverkillSolarBms::get_rate_capacity() {
    // Capacity is stored internally as a uint16_t in units of 10mAh
    return rate_capacity * 0.01;
}

uint16_t OverkillSolarBms::get_cycle_count() {
    return cycle_count;
}

#ifdef BMS_OPTION_PRODUCTION_DATE
ProductionDate OverkillSolarBms::get_production_date() {
    // Production date is stored internally as a uint16_t, bit-packed as follows:
    //         1111110000000000
    // Field   5432109876543210  # bits  offset
    // ======  ================  ======  ======
    // Day:               xxxxx  5       0
    // Month:         xxxx       4       5
    // Year:   xxxxxxx           7       9
    ProductionDate date;
    date.year = 2000 + ((production_date >> 9) & 0b1111111);
    date.month = (production_date >> 5) & 0b1111;
    date.day = production_date & 0b11111;
    return date;
}
#endif

bool OverkillSolarBms::get_balance_status(uint8_t cell_index) {
    if (cell_index <= 31) {
        return (balance_status >> cell_index) & 1;
    }
    else {
        return false;
    }
}

#ifdef BMS_OPTION_FAULT_COUNTS
bool OverkillSolarBms::has_new_fault_occured(uint8_t index) {
    return (bool)((((last_protection_status ^ protection_status) & protection_status) >> index) & 1);
}
#endif


ProtectionStatus OverkillSolarBms::get_protection_status() {
    ProtectionStatus status;
    status.single_cell_overvoltage_protection   = (bool)((protection_status)       & 1);
    status.single_cell_undervoltage_protection  = (bool)((protection_status >> 1)  & 1);
    status.whole_pack_overvoltage_protection    = (bool)((protection_status >> 2)  & 1);
    status.whole_pack_undervoltage_protection   = (bool)((protection_status >> 3)  & 1);
    status.charging_over_temperature_protection = (bool)((protection_status >> 4)  & 1);
    status.charging_low_temperature_protection  = (bool)((protection_status >> 5)  & 1);
    status.discharge_over_temperature_protection= (bool)((protection_status >> 6)  & 1);
    status.discharge_low_temperature_protection = (bool)((protection_status >> 7)  & 1);
    status.charging_overcurrent_protection      = (bool)((protection_status >> 8)  & 1);
    status.discharge_overcurrent_protection     = (bool)((protection_status >> 9)  & 1);
    status.short_circuit_protection             = (bool)((protection_status >> 10) & 1);
    status.front_end_detection_ic_error         = (bool)((protection_status >> 11) & 1);
    status.software_lock_mos                    = (bool)((protection_status >> 12) & 1);
    return status;
}

#ifdef BMS_OPTION_FAULT_COUNTS
FaultCount OverkillSolarBms::get_fault_counts() {
    return fault_count;
}

void OverkillSolarBms::clear_fault_counts() {
    fault_count.single_cell_overvoltage_protection    = 0;
    fault_count.single_cell_undervoltage_protection   = 0;
    fault_count.whole_pack_overvoltage_protection     = 0;
    fault_count.whole_pack_undervoltage_protection    = 0;
    fault_count.charging_over_temperature_protection  = 0;
    fault_count.charging_low_temperature_protection   = 0;
    fault_count.discharge_over_temperature_protection = 0;
    fault_count.discharge_low_temperature_protection  = 0;
    fault_count.charging_overcurrent_protection       = 0;
    fault_count.discharge_overcurrent_protection      = 0;
    fault_count.short_circuit_protection              = 0;
    fault_count.front_end_detection_ic_error          = 0;
    fault_count.software_lock_mos                     = 0;
}
#endif


bool OverkillSolarBms::get_protection_status_summary() {
    return (bool)protection_status & 0b1111111111111;
}


#ifdef BMS_OPTION_SW_VERSION
SoftwareVersion OverkillSolarBms::get_software_version() {
    SoftwareVersion version;
    version.major = (software_version >> 4) & 0b1111;
    version.minor = software_version & 0b1111;
    return version;
}
#endif

uint8_t OverkillSolarBms::get_state_of_charge() {
    // note: the result is not bounds-checked, I assume that the BMS could
    // return higher than 100%, but rather than hiding that here and
    // bounding it to 100, it is returned as-is.
    return remaining_soc;
}

bool OverkillSolarBms::get_discharge_mosfet_status() {
    return (mosfet_status >> 1) & 1;
}

bool OverkillSolarBms::get_charge_mosfet_status() {
    return mosfet_status & 1;
}

uint8_t OverkillSolarBms::get_num_cells() {
    return num_cells;
}

uint8_t OverkillSolarBms::get_num_ntcs() {
    return num_ntcs;
}

// Returns the temperature, in celsius
float OverkillSolarBms::get_ntc_temperature(uint8_t ntc_index) {
    if (ntc_index + 1 <= BMS_MAX_NTCs) {
        float temp = ntc_temps[ntc_index];
        temp *= 0.1;  // Convert fixed-precision int 0.1 degrees K per LSB to float degrees K
        temp -= 273.15;  // Convert Kelvin to Celsius
        return temp;
    }
    else {
        return 1.0 / 0.0;  // NaN
    }
}

// ###########################################################################
// 0x04 Cell Voltages
// ###########################################################################

// Returns the cell voltage, in volts
float OverkillSolarBms::get_cell_voltage(uint8_t cell_index) {
    if (cell_index + 1 <= BMS_MAX_CELLS) {
        float voltage = cell_voltages[cell_index];
        voltage *= 0.001;  // Convert millivolts to volts
        return voltage;
    }
    else {
        return 1.0 / 0.0;  // NaN
    }
}

// ###########################################################################
// 0x05 BMS Name
// ###########################################################################

#ifdef BMS_OPTION_NAME
String OverkillSolarBms::get_bms_name() {
    // Clear the bms_name string
    bms_name = String("");

#ifdef BMS_OPTION_DEBUG
    Serial.println("Query 0x05 BMS Name");
#endif
    uint8_t length = 0;
    uint8_t *data = NULL;
    write(true, BMS_REG_NAME, length, data);

    uint32_t t0 = millis();

    // Handle the incoming serial data until the BMS name is received
    while(1) {
        serial_rx_task();
        delay(10);

        // Wait for the RX task to receive the message, parse it and store
        // it in the bms_name
        if (bms_name.length() > 0) {
            break;
        }

        // Timeout if it took too long
        if (millis() - t0 >= BMS_TIMEOUT ) {
            break;
        }
    }
    return bms_name;
}
#endif

// ###########################################################################
// 0xE1 Set MOSFET
// ###########################################################################

void OverkillSolarBms::set_mosfet_control(bool charge, bool discharge) {
#ifdef BMS_OPTION_DEBUG
    Serial.println("Query 0xE1 MOSFET Control");
#endif
    uint8_t length = 2;
    uint8_t data[2];

    data[0] = 0b11;
    data[1] = 0;
    if (charge) {
        data[0] &= 0b10;
    }
    if (discharge) {
        data[0] &= 0b01;
    }

    write(true, BMS_REG_CTL_MOSFET, length, data);
}

#ifdef BMS_OPTION_DEBUG
void OverkillSolarBms::debug() {
    Serial.println("==============================================");
    Serial.print("Voltage:           ");
    Serial.print(get_voltage(), 3);
    Serial.println(" V");

    Serial.print("Current:           ");
    Serial.print(get_current(), 3);
    Serial.println(" A");

    Serial.print("Balance capacity:  ");
    Serial.print(get_balance_capacity(), 3);
    Serial.println(" Ah");

    Serial.print("Rate capacity:     ");
    Serial.print(get_rate_capacity(), 3);
    Serial.println(" Ah");

    Serial.print("Cycle count:       ");
    Serial.println(get_cycle_count() , DEC);

#ifdef BMS_OPTION_PRODUCTION_DATE
    Serial.print("Production Date:   ");
    ProductionDate date = get_production_date();
    Serial.print(date.day, DEC);
    Serial.print("/");
    Serial.print(date.month, DEC);
    Serial.print("/");
    Serial.println(date.year, DEC);
#endif

    Serial.println("Protection Status: ");
    ProtectionStatus prot_status = get_protection_status();
    Serial.print("  software_lock_mos:                    ");
    Serial.println(prot_status.software_lock_mos, DEC);
    Serial.print("  front_end_detection_ic_error:         ");
    Serial.println(prot_status.front_end_detection_ic_error, DEC);
    Serial.print("  short_circuit_protection:             ");
    Serial.println(prot_status.short_circuit_protection, DEC);
    Serial.print("  discharge_overcurrent_protection:     ");
    Serial.println(prot_status.discharge_overcurrent_protection, DEC);
    Serial.print("  charging_overcurrent_protection:      ");
    Serial.println(prot_status.charging_overcurrent_protection, DEC);
    Serial.print("  discharge_low_temperature_protection: ");
    Serial.println(prot_status.discharge_low_temperature_protection, DEC);
    Serial.print("  discharge_over_temperature_protection:");
    Serial.println(prot_status.discharge_over_temperature_protection, DEC);
    Serial.print("  charging_low_temperature_protection:  ");
    Serial.println(prot_status.charging_low_temperature_protection, DEC);
    Serial.print("  charging_over_temperature_protection: ");
    Serial.println(prot_status.charging_over_temperature_protection, DEC);
    Serial.print("  whole_pack_undervoltage_protection:   ");
    Serial.println(prot_status.whole_pack_undervoltage_protection, DEC);
    Serial.print("  whole_pack_overvoltage_protection:    ");
    Serial.println(prot_status.whole_pack_overvoltage_protection, DEC);
    Serial.print("  single_cell_undervoltage_protection:  ");
    Serial.println(prot_status.single_cell_undervoltage_protection, DEC);
    Serial.print("  single_cell_overvoltage_protection:   ");
    Serial.println(prot_status.single_cell_overvoltage_protection, DEC);

#ifdef BMS_OPTION_SW_VERSION
    Serial.print("Software version:  ");
    SoftwareVersion version = get_software_version();
    Serial.print(version.major, DEC);
    Serial.print(".");
    Serial.println(version.minor, DEC);
#endif

    Serial.print("State of Charge:   ");
    Serial.print(get_state_of_charge(), DEC);
    Serial.println("%");

    Serial.print("Discharge MOSFET:  ");
    Serial.println(get_discharge_mosfet_status()?"ON":"OFF");

    Serial.print("Charge MOSFET:     ");
    Serial.println(get_charge_mosfet_status()?"ON":"OFF");

    Serial.print("# of cells:        ");
    Serial.println(get_num_cells(), DEC);

    Serial.print("# of temp sensors: ");
    Serial.println(get_num_ntcs(), DEC);

    Serial.println("Temperatures:");
    for (uint8_t i=0; i < min(BMS_MAX_NTCs, get_num_ntcs()); i++) {
        Serial.print("  ");
        Serial.print(get_ntc_temperature(i), 1);
        Serial.println(" deg C");
    }

    Serial.println("Cell Voltages & Balance Status: ");
    for (uint8_t i=0; i < min(BMS_MAX_CELLS, get_num_cells()); i++) {
        Serial.print("  ");
        Serial.print(get_cell_voltage(i), 3);  // Returns the cell voltage, in volts
        Serial.print("V  ");
        Serial.println(get_balance_status(i)?"(balancing)":"(not balancing)");
    }

#ifdef BMS_OPTION_NAME
    Serial.print("BMS Name:         ");
    Serial.println(get_bms_name());
#endif
    // Serial.println("==============================================");
    Serial.println();
}
#endif

// ###########################################################################
// Messages to and from the BMS

void OverkillSolarBms::query_basic_info() {
#ifdef BMS_OPTION_DEBUG
    Serial.println("Query 0x03 Basic Info");
#endif
    uint8_t length = 0;
    uint8_t *data = NULL;
    write(true, BMS_REG_BASIC_SYSTEM_INFO, length, data);

    uint32_t t0 = millis();

    // Handle the incoming serial data and block until the basic info is received
    while(1) {
        serial_rx_task();
        delay(10);

        if (last_0x03_timestamp > t0) {
            break;
        }

        // Timeout if it took too long
        if (millis() - t0 >= BMS_TIMEOUT ) {
            break;
        }
    }
}

void OverkillSolarBms::query_cell_voltages() {
#ifdef BMS_OPTION_DEBUG
    Serial.println("Query 0x04 Cell Voltages");
#endif
    uint8_t length = 0;
    uint8_t *data = NULL;
    write(true, BMS_REG_CELL_VOLTAGES, length, data);

    uint32_t t0 = millis();
    // Handle the incoming serial data and block until the voltages are received
    while(1) {
        serial_rx_task();
        delay(10);

        if (last_0x04_timestamp > t0) {
            break;
        }

        // Timeout if it took too long
        if (millis() - t0 >= BMS_TIMEOUT ) {
            break;
        }
    }
}

// ###########################################################################
// Low-level read/write methods

// Write to BMS
void OverkillSolarBms::write(bool read, uint8_t command_code, uint8_t* data, uint8_t length) {
    uint16_t checksum = 0;

    // Write the start byte, 0xDD
    serial->write(BMS_STARTBYTE);

    // Write the status byte, which indicates whether a read or write is being requested
    if (read) {
        serial->write(BMS_READ);  // 0xA5
    }
    else {
        serial->write(BMS_WRITE);  // 0x5A
    }

    // Write the command code (the register address)
    serial->write(command_code);
    checksum += command_code;

    // Write the length
    serial->write(length);
    checksum += length;

    // Write the data
    for (uint8_t i=0; i< length; i++) {
        serial->write(data[i]);
        checksum += data[i];
    }

    // Write the checksum
    checksum = (uint16_t)((0x10000UL) - (uint32_t)checksum);

    uint8_t checksum_msb = (uint8_t)((checksum >> 8) & 0xFF);
    serial->write(checksum_msb);

    uint8_t checksum_lsb = (uint8_t)(checksum & 0xFF);
    serial->write(checksum_lsb);

    // Write the stop byte, 0x77
    serial->write(BMS_STOPBYTE);
}

void OverkillSolarBms::serial_rx_task() {
    int bytes_available = serial->available();
    if (bytes_available > 0) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
        Serial.print(bytes_available, DEC);
        Serial.println(" bytes available to read");
#endif
        for (int i=0; i < bytes_available; i++) {
            int c = serial->read();
            if (c == -1) {
                continue;
            }

            if (rx_state == BMS_STATE_WAIT_FOR_START_BYTE) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_START_BYTE]:  ");
                Serial.println(c, HEX);
#endif
                if (c == BMS_STARTBYTE) {
                    rx_cmd_code = 0;
                    rx_status = 0;
                    rx_length = 0;
                    rx_data_index = 0;
                    rx_checksum = 0;

                    // Reset all of the state variables
                    rx_state = BMS_STATE_WAIT_FOR_CMD_CODE;
                }
                else {
                    // Error
                    // Serial.println("");
                    // Serial.println("Framing error! Not a start byte!");
                    num_rx_errors += 1;
                    rx_state = BMS_STATE_WAIT_FOR_START_BYTE;
                }
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_CMD_CODE) {
                rx_cmd_code = c;
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_CMD_CODE]:    ");
                Serial.println(c, HEX);
#endif
                rx_state = BMS_STATE_WAIT_FOR_STATUS_BYTE;
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_STATUS_BYTE) {
                rx_status = c;
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_STATUS_BYTE]: ");
                Serial.println(c, HEX);
#endif

                if (rx_status == 0x00) {
                    // The BMS should set the status to 0x00 if it is OK.
                }

                else if (rx_status == 0x80) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                    Serial.println("");
                    Serial.println("RX error! Status byte should have been 0x00, but got 0x80!");
#endif
                    // An error occured
                    num_rx_errors += 1;
                }
                else {
                    // Any code other to us is still an error
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                    Serial.println("");
                    Serial.println("RX error! Status byte should have been 0x00 or 0x80!");
#endif
                    num_rx_errors += 1;
                }
                rx_state = BMS_STATE_WAIT_FOR_LENGTH;
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_LENGTH) {
                rx_length = c;
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_LENGTH]:      ");
                Serial.println(c, DEC);
#endif
                if (rx_length > BMS_MAX_RX_DATA_LEN) {
                    // Error
                    // Serial.println("");
                    num_rx_errors += 1;
                }
                rx_state = BMS_STATE_WAIT_FOR_DATA;
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_DATA) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_DATA]:        ");
                Serial.print(c, HEX);
#endif
                if (rx_data_index + 1 <= BMS_MAX_RX_DATA_LEN) {
                    rx_data[rx_data_index] = c;
                }
                if (rx_data_index + 1 >= rx_length) {
                    rx_state = BMS_STATE_WAIT_FOR_CHECKSUM_MSB;
                }
                else {
                    // Keep the rx_state in BMS_STATE_WAIT_FOR_DATA,
                    // until all data bytes have been received
                    rx_data_index += 1;
                }
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print(", rx_data_index=");
                Serial.println(rx_data_index, DEC);
#endif
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_CHECKSUM_MSB) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_CHECKSUM_MSB]: ");
                Serial.println(c, HEX);
#endif
                rx_checksum = (c << 8) & 0xFF00;
                rx_state = BMS_STATE_WAIT_FOR_CHECKSUM_LSB;
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_CHECKSUM_LSB) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_CHECKSUM_LSB]: ");
                Serial.println(c, HEX);
#endif
                rx_checksum |= c;
                rx_state = BMS_STATE_WAIT_FOR_STOP_BYTE;
            }
            else if (rx_state == BMS_STATE_WAIT_FOR_STOP_BYTE) {
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                Serial.print("[WAIT_FOR_STOP_BYTE]: ");
                Serial.println(c, HEX);
#endif

                // Calculate the checksum of the length and data bytes.
                // If it matches the received checksum, then continue.
                // Otherwise, flag it as an error and stop.
                uint16_t calc_checksum = 0;
                calc_checksum += rx_status;
                calc_checksum += rx_length;
                for (uint8_t i=0; i < rx_length; i++) {
                    calc_checksum += rx_data[i];
                }
                calc_checksum = (uint16_t)((0x10000UL) - (uint32_t)calc_checksum);

#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                if (rx_checksum != calc_checksum) {
                    Serial.println("");
                    Serial.println("Checksum did not calculate!");
                    Serial.print("  Calculated: ");
                    Serial.println(calc_checksum, HEX);
                    Serial.print("  Received: ");
                    Serial.println(rx_checksum, HEX);
                }
#endif

                if (rx_checksum == calc_checksum && rx_status == 0x00) {
                    if (c == BMS_STOPBYTE) {
                        // Everything looks OK, handle the data
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                        Serial.println("Got a complete msg");
#endif
                        if (rx_cmd_code == BMS_REG_BASIC_SYSTEM_INFO) {  // 0x03
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                            Serial.println("Got an 0x03 Basic Info msg");
#endif
                            voltage             = (uint16_t)(rx_data[0]  << 8) | (uint16_t)(rx_data[1]);  // 0-1   Total voltage
                            current             = (uint16_t)(rx_data[2]  << 8) | (uint16_t)(rx_data[3]);  // 2-3   Current
                            balance_capacity    = (uint16_t)(rx_data[4]  << 8) | (uint16_t)(rx_data[5]);  // 4-5   Balance capacity
                            rate_capacity       = (uint16_t)(rx_data[6]  << 8) | (uint16_t)(rx_data[7]);  // 6-7   Rate capacity
                            cycle_count         = (uint16_t)(rx_data[8]  << 8) | (uint16_t)(rx_data[9]);  // 8-9   Cycle count
#ifdef BMS_OPTION_PRODUCTION_DATE
                            production_date     = (uint16_t)(rx_data[10] << 8) | (uint16_t)(rx_data[11]);  // 10-11 Production Date
#endif
                            balance_status      = (uint16_t)(rx_data[12] << 8) | (uint16_t)(rx_data[13]);  // 12-13, 14-15 Balance Status
                            protection_status   = (uint16_t)(rx_data[16] << 8) | (uint16_t)(rx_data[17]);  // 16-17 Protection status

#ifdef BMS_OPTION_FAULT_COUNTS
                            // See if there are any new faults.  If so, then increment the count.
                            if (has_new_fault_occured(0))  { fault_count.single_cell_overvoltage_protection    += 1; }
                            if (has_new_fault_occured(1))  { fault_count.single_cell_undervoltage_protection   += 1; }
                            if (has_new_fault_occured(2))  { fault_count.whole_pack_undervoltage_protection    += 1; }
                            if (has_new_fault_occured(3))  { fault_count.single_cell_overvoltage_protection    += 1; }
                            if (has_new_fault_occured(4))  { fault_count.charging_over_temperature_protection  += 1; }
                            if (has_new_fault_occured(5))  { fault_count.charging_low_temperature_protection   += 1; }
                            if (has_new_fault_occured(6))  { fault_count.discharge_over_temperature_protection += 1; }
                            if (has_new_fault_occured(7))  { fault_count.discharge_low_temperature_protection  += 1; }
                            if (has_new_fault_occured(8))  { fault_count.charging_overcurrent_protection       += 1; }
                            if (has_new_fault_occured(9))  { fault_count.discharge_overcurrent_protection      += 1; }
                            if (has_new_fault_occured(10)) { fault_count.short_circuit_protection              += 1; }
                            if (has_new_fault_occured(11)) { fault_count.front_end_detection_ic_error          += 1; }
                            if (has_new_fault_occured(12)) { fault_count.software_lock_mos                     += 1; }
#endif

#ifdef BMS_OPTION_SW_VERSION
                            software_version    = rx_data[18];  // 18    Software version
#endif
                            remaining_soc       = rx_data[19];  // 19    Remaining state of charge
                            mosfet_status       = rx_data[20];  // 20    MOSFET status
                            num_cells           = rx_data[21];  // 21    # of batteries in series
                            num_ntcs            = rx_data[22];  // 22    # of NTCs

                            for (uint8_t i=0; i < min(BMS_MAX_NTCs, num_ntcs); i++) {
                                uint8_t ntc_index = 23 + (i * 2);
                                ntc_temps[i] = (uint16_t)(rx_data[ntc_index] << 8) | (uint16_t)(rx_data[ntc_index + 1]);
                            }
                            last_0x03_timestamp = millis();
                        }
                        else if (rx_cmd_code == BMS_REG_CELL_VOLTAGES) {  // 0x04
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
                            Serial.println("Got an 0x04 Cell Voltage msg");
#endif
                            for (uint8_t i=0; i < min(BMS_MAX_CELLS, num_cells); i++) {
                                cell_voltages[i] = (uint16_t)(rx_data[i * 2] << 8) | (uint16_t)(rx_data[(i * 2) + 1]);
                            }
                            last_0x04_timestamp = millis();
                        }
#ifdef BMS_OPTION_NAME
                        else if (rx_cmd_code == BMS_REG_NAME) {  // 0x05
                            bms_name = String("");
                            for (uint8_t i=0; i < min(BMS_MAX_RX_DATA_LEN, rx_length); i++) {
                                bms_name += (char)rx_data[i];
                            }
                        }
#endif
#ifdef BMS_OPTION_DEBUG
                        else {
                            Serial.print("Skipping unknown register: ");
                            Serial.println(rx_cmd_code, HEX);
                        }
#endif
                    }
#ifdef BMS_OPTION_DEBUG
                    else {
                        Serial.println("");
                        Serial.println("Framing error!  Expected 0x77 stop byte!");
                        num_rx_errors += 1;
                    }
#endif
                }
                else {
                    num_rx_errors += 1;
                }

                rx_state = BMS_STATE_WAIT_FOR_START_BYTE;
            }
#ifdef BMS_OPTION_DEBUG
            else {
                Serial.print("THIS SHOULD NEVER HAPPEN!  The rx_state was: ");
                Serial.println(rx_state, DEC);
            }  // rx_state
#endif
        }  // for i in bytes_available
#ifdef BMS_OPTION_DEBUG_STATE_MACHINE
        Serial.print("The rx_state is: ");
        Serial.print(rx_state, DEC);
        Serial.println("...");
#endif
    }  // if bytes_available > 0
}
