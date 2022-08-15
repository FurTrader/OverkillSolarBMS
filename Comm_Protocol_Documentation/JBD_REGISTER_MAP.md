# JBD BMS SERIAL INTERFACE AND REGISTER MAP

## Glossary / abbreviations

<table border="1">
    <tr>
        <th>Abbreviation</th>
        <th>Meaning</th>
    </tr>
    <tr>
        <td>U16</td>
        <td>Unsigned 16 bit value</td>
    </tr>
    <tr>
        <td>S16</td>
        <td>Signed 16 bit value</td>
    </tr>
</table>



## Serial communication 

Serial port communcation is 9600 baud, 8 data bits, no parity, 1 stop bit  (9600 8N1).

### Packet details

Packets to/from the BMS consist of:

<table border="1">
    <tr>
     <th>Start Byte</th>
     <th>Payload</th>
     <th>Checksum</th>
     <th>End Byte</th>
    </tr>
    <tr>
     <td>0xDD</td>
     <td>3 or more bytes</td>
     <td>2 bytes, U16</td>
     <td>0x77</td>
</table>
<br>

#### Checksum

The checksum is simply sum of the payload byte values subtracted from 0x10000 (65536).

#### Payload to BMS 

<table border="1">
    <tr>
     <td>Command byte: Read: 0xA5, Write: 0x5A</td>
     <td>Register Address Byte</td>
     <td>Data length byte</td>
     <td>Data bytes, n = data length byte</td>
    </tr>
</table>

#### Payload from BMS 

<table border="1">
    <tr>
     <td>Register Address Byte</td>
     <td>Command status: OK: 0x0, Error: 0x80 </td>
     <td>Data length byte</td>
     <td>Data bytes, n = data length byte</td>
    </tr>
</table>

### Register Descriptions

#### Register 0x03 "Basic Info" (READ ONLY)
<table border="1">
    <tr>
     <th>Byte offset</th>
     <th>Data</th>
     <th>Format</th>
     <th>Unit</th>
     <th>Field name(s)</th>
     <th>Notes</th>
    </tr>
    <tr>
     <td>0x0</td>
     <td>Pack voltage</td>
     <td>U16</td>
     <td>10mV </td>
     <td>pack_mv</td>
     <td></td>
    </tr>
    <tr>
     <td>0x2</td>
     <td>Pack amperes</td>
     <td>S16</td>
     <td>10mA</td>
     <td>pack_ma</td>
     <td>Negative values indicate discharge</td>
    </tr>
    <tr>
     <td>0x4</td>
     <td>Balance Capacity</td>
     <td>U16</td>
     <td>10mAH</td>
     <td>cycle_cap</td>
     <td></td>
    </tr>
    <tr>
     <td>0x6</td>
     <td>Full Capacity</td>
     <td>U16</td>
     <td>10mAH</td>
     <td>design_cap</td>
     <td></td>
    </tr>
    <tr>
     <td>0x8</td>
     <td>Discharge/Charge Cycles</td>
     <td>U16</td>
     <td>1 cycle</td>
     <td>cycle_cnt</td>
     <td></td>
    </tr>
    <tr>
     <td>0xA</td>
     <td>Manufacture date</td>
     <td>
        16 bit:<br>
        bits 15:9=year - 2000<br>
        bits 8:5=month<br>
        bits 4:0=Day<br>
     </td>
     <td>--</td>
     <td>year, month, day</td>
     <td></td>
    </tr>
    <tr>
     <td>0xC</td>
     <td>Cell balance status</td>
     <td>16 bit:<br>
         bit 0: cell 1 balance active<br>
         bit 1: cell 2 balance active<br>
         ...etc...</td>
     <td>--</td>
     <td>bal_0, bal_1, ... etc ...  </td>
     <td></td>
    </tr>
    <tr>
     <td>0xE</td>
     <td>Cell balance status</td>
     <td>16 bit:<br>
         bit 0: cell 17 balance active<br>
         bit 1: cell 18 balance active<br>
         ...etc...</td>
     <td>--</td>
     <td>bal_16, bal_17, &lt;etc&gt; </td>
     <td></td>
    </tr>
    <tr>
     <td>0x10</td>
     <td>Current errors</td>
     <td>16 bit:<br>
         bit 0: Cell overvolt<br>
         bit 1: Cell undervolt<br>
         bit 2: Pack overvolt<br>
         bit 3: Pack undervolt<br>
         bit 4: Charge overtemp<br>
         bit 5: Charge undertemp<br>
         bit 6: Discharge overtemp<br>
         bit 7: Discharge undertemp<br>
         bit 8: Charge overcurrent<br>
         bit 9: Discharge overcurrent<br>
         bit 10: Short Circuit<br>
         bit 11: Frontend IC error<br>
         bit 12: Charge or Discharge FET locked by config (See register 0xE1 "MOSFET control")<br>
        </td>
     <td>--</td>
     <td> covp_err, cuvp_err, povp_err, puvp_err, chgot_err, chgut_err, dsgot_err, dsgut_err, chgoc_err, dsgoc_err, sc_err, afe_err, software_err</td>
     <td></td>
    </tr>
    <tr>
     <td>0x11</td>
     <td>Software Version</td>
     <td>1 byte: 0x10 = 1.0 (BCD?)</td>
     <td>--</td>
     <td>version</td>
     <td></td>
    <tr>
     <td>0x12</td>
     <td>State of Charge</td>
     <td>1 byte</td>
     <td>percent</td>
     <td>cap_pct</td>
     <td></td>
    </tr>
    <tr>
     <td>0x13</td>
     <td>FET status</td>
     <td>1 byte</td>
     <td>
         bit 0: charge FET <br>
         bit 1: discharge FET<br>
         bit set = FET is conducting
     </td>
     <td>chg_fet_en, dsg_fet_en</td>
     <td></td>
    </tr>
    <tr>
     <td>0x14</td>
     <td>Pack cells</td>
     <td>1 byte</td>
     <td>1 cell</td>
     <td>cell_cnt</td>
     <td></td>
    </tr>
    <tr>
     <td>0x15</td>
     <td>NTC count</td>
     <td>1 byte</td>
     <td>1 NTC</td>
     <td>ntc_cnt</td>
     <td></td>
    </tr>
    <tr>
     <td>0x16 .. 0x16 + ntc_cnt x 2</td>
     <td>NTC Values</td>
     <td>16 bits</td>
     <td>0.1K</td>
     <td>ntc0, ntc1, &lt;etc&gt;</td>
    </tr>
</table>


#### Register 0x04 "Cell voltages" (READ ONLY)
The number of values returned depends on the cell_cnt field from 0x3 "Basic Info".
<table border="1">
    <tr>
     <th>Byte offset</th>
     <th>Data</th>
     <th>Format</th>
     <th>Field name(s)</th>
    </tr>
    <tr>
     <td>2 * cell number (starting at zero)</td>
     <td>Cell voltage</td>
     <td>16 bits, unsigned, unit: 1mV</td>
     <td>cell0_mv, cell1_mv, &lt;etc&gt;</td>
    </tr>
</table>

#### Register 0x05 "Device Name" (READ ONLY)
<table border="1">
    <tr>
     <th>Byte offset</th>
     <th>Data</th>
     <th>Format</th>
     <th>Field name(s)</th>
    </tr>
    <tr>
     <td>0x0</td>
     <td>Device name length</td>
     <td>1 byte, length of following string</td>
     <td>--</td>
    </tr>
    <tr>
     <td>0x1 .. n</td>
     <td>Device name </td>
     <td>n bytes of device name</td>
     <td>device_name</td>
    </tr>
</table>

### Passwords

**Thanks to Steve Tecza at [Overkill Solar](https://overkillsolar.com/) for doing the legwork of figuring this out.**

Devices with firmware 0x16 or higher have password capability.

_If_ there is a password set, then the password should be sent to the register password _before_ entering into factory mode by writing the password to the `use_password` register.

Note that in the stock JBD FW, you can always clear the password by using the `clear_password` register.  This effectively makes passwords useless.
<table border="1">
    <tr>
     <th>Register Address</th>
     <th>Register Name</th>
     <th>Data</th>
     <th>Format</th>
     <th>Unit</th>
     <th>Field name(s)</th>
     <th>Notes</th>
    </tr>
    <tr>
     <td>0x06</td>
     <td>use_password</td>
     <td>bytes</td>
     <td>[length byte (0x06)][6 byte password]</td>
     <td>--</td>
     <td>--</td>
     <td>Write the current password to this register to enable access to entering "factory mode," below. This register is similar to a string register (e.g. "mfg_name") in that the first byte of the payload must be the length. Length must be 6.</td>
    </tr>
    <tr>
     <td>0x07</td>
     <td>set_password</td>
     <td>bytes</td>
     <td>[length byte (0x0c)][6 byte current password][6 byte new password]</td>
     <td>--</td>
     <td>--</td>
     <td>This changes the password.  A single 13-byte payload is provided. Byte 0 is the length (0x0c); next 6 bytes are the current password; final 6 bytes are the new password.  This register is similar to a string register (e.g. "mfg_name") in that the first byte of the payload must be the length. Length is 12.</td>
    </tr>
    <tr>
     <td>0x09</td>
     <td>clear_password</td>
     <td>J1B2D4</td>
     <td>[length byte (0x6)] 'J1B2D4'</td>
     <td>--</td>
     <td>--</td>
     <td>Write the ASCII value 'J1B2D4' to remove password protection. This register is similar to a string register (e.g. "mfg_name") in that the first byte of the payload must be the length. Length is 6.</td>
    </tr>
   </table>


### EEPROM Register Descriptions
These registers are read/write configuration settings that are stored in EEPROM.  They affect the operation of the BMS.

Unless otherwise noted, all registers are 16 bit big-endian.  Signedness varies.


#### Register 0x00 "Enter factory Mode"
Write the byte sequence 0x56, 0x78 to enter "Factory Mode."  In this mode, the other registers below can be accessed.

#### Register 0x01 "Exit factory Mode"
Write the byte sequence 0x0, 0x0 to exit "Factory Mode."

Write the byte sequence 0x28, 0x28 to exit "Factory Mode," update the values in the EEPROM, and reset the "Error Counts" (0xAA) register to zeroes.

#### Stored registers:
<table border="1">
    <tr>
     <th>Register Address</th>
     <th>Register Name</th>
     <th>Data</th>
     <th>Format</th>
     <th>Unit</th>
     <th>Field name(s)</th>
     <th>Notes</th>
    </tr>
    <tr>
     <td>0x10</td>
     <td>design_cap</td>
     <td>Pack capacity, as designed</td>
     <td>U16</td>
     <td>10 mAh</td>
     <td>design_cap</td>
     <td></td>
    </tr>
    <tr>
     <td>0x11</td>
     <td>cycle_cap</td>
     <td>Pack capacity, per cycle</td>
     <td>U16</td>
     <td>10 mAh</td>
     <td>cycle_cap</td>
     <td></td>
    </tr>
    <tr>
     <td>0x12</td>
     <td>cap_100</td>
     <td>Cell capacity estimate voltage, 100%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_100</td>
     <td></td>
    </tr>
    <tr>
     <td>0x32</td>
     <td>cap_80</td>
     <td>Cell capacity estimate voltage, 80%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_80</td>
     <td></td>
    </tr>
    <tr>
     <td>0x33</td>
     <td>cap_60</td>
     <td>Cell capacity estimate voltage, 60%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_60</td>
     <td></td>
    </tr>
    <tr>
     <td>0x34</td>
     <td>cap_40</td>
     <td>Cell capacity estimate voltage, 40%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_40</td>
     <td></td>
    </tr>
    <tr>
     <td>0x35</td>
     <td>cap_20</td>
     <td>Cell capacity estimate voltage, 20%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_20</td>
     <td></td>
    </tr>
    <tr>
     <td>0x13</td>
     <td>cap_0</td>
     <td>Cell capacity estimate voltage, 0%</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cap_0</td>
     <td></td>
    </tr>
    <tr>
     <td>0x14</td>
     <td>dsg_rate</td>
     <td>Cell estimated self discharge rate</td>
     <td>U16</td>
     <td>0.1%</td>
     <td>dsg_rate</td>
     <td></td>
    </tr>
    <tr>
     <td>0x15</td>
     <td>mfg_date</td>
     <td>Manufacture date</td>
     <td>
        16 bit:<br>
        bits 15:9=year - 2000<br>
        bits 8:5=month<br>
        bits 4:0=Day<br>
     </td>
     <td> -- </td>
     <td>year, month, day</td>
     <td></td>
    </tr>
    <tr>
     <td>0x16</td>
     <td>serial_num</td>
     <td>Serial number</td>
     <td>U16</td>
     <td>--</td>
     <td>serial_num</td>
     <td></td>
    </tr>
    <tr>
     <td>0x17</td>
     <td>cycle_cnt</td>
     <td>Cycle count</td>
     <td>U16</td>
     <td>cycle</td>
     <td>cycle_cnt</td>
     <td></td>
    </tr>
    <tr>
     <td>0x18</td>
     <td>chgot</td>
     <td>Charge Overtemp threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>chgot</td>
     <td></td>
    </tr>
    <tr>
     <td>0x19</td>
     <td>chgot_rel</td>
     <td>Charge Overtemp release threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>chgot_rel</td>
     <td>Temp must fall below this value to release overtemp condtion</td>
    </tr>
    <tr>
     <td>0x1A</td>
     <td>chgut</td>
     <td>Charge Undertemp threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>chgut</td>
     <td></td>
    </tr>
    <tr>
     <td>0x1B</td>
     <td>chgut_rel</td>
     <td>Charge Undertemp release threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>chgut_rel</td>
     <td>Temp must rise above this value to release overtemp condtion</td>
    </tr>
    <tr>
     <td>0x3A</td>
     <td>chg_t_delays</td>
     <td>Charge over/undertemp release delay</td>
     <td>
        2 bytes:<br>
        &nbsp; byte 0: Charge under temp release delay<br>
        &nbsp; byte 1: Charge over temp release delay<br>
     </td>
     <td>s</td>
     <td>chgut_delay, chgot_delay</td>
     <td></td>
    </tr>
    <tr>
     <td>0x3B</td>
     <td>dsg_t_delays</td>
     <td>Discharge over/undertemp release delay</td>
     <td>
        2 bytes:<br>
        &nbsp; byte 0: Discharge under temp release delay<br>
        &nbsp; byte 1: Discharge over temp release delay<br>
     </td>
     <td>s</td>
     <td>dsgut_delay, dsgot_delay</td>
     <td></td>
    </tr>
    <tr>
     <td>0x1C</td>
     <td>dsgot</td>
     <td>Discharge Overtemp threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>dsgot</td>
     <td></td>
    </tr>
    <tr>
     <td>0x1D</td>
     <td>dsgot_rel</td>
     <td>Discharge Overtemp release threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>dsgot_rel</td>
     <td>Temp must fall below this value to release overtemp condtion</td>
    </tr>
    <tr>
     <td>0x1E</td>
     <td>dsgut</td>
     <td>Discharge Undertemp threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>dsgut</td>
     <td></td>
    </tr>
    <tr>
     <td>0x1F</td>
     <td>dsgut_rel</td>
     <td>Discharge Undertemp release threshold</td>
     <td>U16</td>
     <td>0.1K</td>
     <td>dsgut_rel</td>
     <td>Temp must rise above this value to release overtemp condtion</td>
    </tr>
    <tr>
     <td>0x20</td>
     <td>povp</td>
     <td>Pack Overvoltage Protection threshold</td>
     <td>U16</td>
     <td>10 mV</td>
     <td>povp</td>
     <td></td>
    </tr>
    <tr>
     <td>0x21</td>
     <td>povp_rel</td>
     <td>Pack Overvoltage Protection Release threshold</td>
     <td>U16</td>
     <td>10 mV</td>
     <td>povp_rel</td>
     <td>Pack voltage must fall below this value to release overvoltage condition</td>
    </tr>
    <tr>
     <td>0x22</td>
     <td>puvp</td>
     <td>Pack Undervoltage Protection threshold</td>
     <td>U16</td>
     <td>10 mV</td>
     <td>puvp</td>
     <td></td>
    </tr>
    <tr>
     <td>0x23</td>
     <td>puvp_rel</td>
     <td>Pack Undervoltage Protection Release threshold</td>
     <td>U16</td>
     <td>10 mV</td>
     <td>puvp_rel</td>
     <td>Pack voltage must rise above this value to release undervoltage condition</td>
    </tr>
    <tr>
     <td>0x3C</td>
     <td>pack_v_delays</td>
     <td>Pack over/under voltage release delay</td>
     <td>
        2 bytes:<br>
        &nbsp; byte 0: Pack under volt release delay<br>
        &nbsp; byte 1: Pack over volt release delay<br>
     </td>
     <td>s</td>
     <td>puvp_delay, povp_delay</td>
     <td></td>
    </tr>
    <tr>
     <td>0x24</td>
     <td>covp</td>
     <td>Cell Overvoltage Protection threshold</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>covp</td>
     <td></td>
    </tr>
    <tr>
     <td>0x25</td>
     <td>covp_rel</td>
     <td>Cell Overvoltage Protection Release</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>covp_rel</td>
     <td>Cell voltage must fall below this value to release overvoltage condition</td>
    </tr>
    <tr>
     <td>0x26</td>
     <td>cuvp</td>
     <td>Cell Undervoltage Protection threshold</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cuvp</td>
     <td></td>
    </tr>
    <tr>
     <td>0x27</td>
     <td>cuvp_rel</td>
     <td>Cell Undervoltage Protection Release threshold</td>
     <td>U16</td>
     <td>1 mV</td>
     <td>cuvp_rel</td>
     <td>Cell voltage must rise above this value to release undervoltage condition</td>
    </tr>
    <tr>
     <td>0x3D</td>
     <td>cell_v_delays</td>
     <td>Cell over/under voltage release delay</td>
     <td>
        2 bytes:<br>
        &nbsp; byte 0: Cell under volt release delay<br>
        &nbsp; byte 1: Cell over volt release delay<br>
     </td>
     <td>s</td>
     <td>cuvp_delay, covp_delay</td>
     <td></td>
    </tr>
    <tr>
     <td>0x28</td>
     <td>chgoc</td>
     <td>Charge overcurrent threshold</td>
     <td>S16</td>
     <td>10 mA</td>
     <td>chgoc</td>
     <td>This number must be positive</td>
    </tr>
    <tr>
     <td>0x3E</td>
     <td>chgoc_delays</td>
     <td>Charge overcurrent delays</td>
     <td>
        2 unsigned bytes:<br>
        byte 0: chgoc_delay<br>
        byte 1: chgoc_release</td>
     <td>s</td>
     <td>chgoc_delay, chgoc_rel</td>
     <td></td>
    </tr>
    <tr>
     <td>0x29</td>
     <td>dsgoc</td>
     <td>Discharge overcurrent threshold</td>
     <td>S16</td>
     <td>10 mA</td>
     <td>dsgoc</td>
     <td>This number must be negative</td>
    </tr>
    <tr>
     <td>0x3f</td>
     <td>chgoc_delays</td>
     <td>Charge overcurrent delays</td>
     <td>
        2 unsigned bytes:<br>
        byte 0: dsgoc_delay<br>
        byte 1: dsgoc_release</td>
     <td>s</td>
     <td>dsgoc_delay, dsgoc_rel</td>
     <td></td>
    </tr>
    <tr>
     <td>0x2A</td>
     <td>bal_start</td>
     <td>Cell balance voltage</td>
     <td>S16</td>
     <td>1 mV</td>
     <td>bal_start</td>
     <td></td>
    </tr>
    <tr>
     <td>0x2B</td>
     <td>bal_window</td>
     <td>Balance window</td>
     <td>U16</td>
     <td>1mV</td>
     <td>bal_window</td>
     <td></td>
    </tr>
    <tr>
     <td>0x2C</td>
     <td>shunt_res</td>
     <td>Ampere measurement shunt resistor value</td>
     <td>U16</td>
     <td>0.1mΩ</td>
     <td>shunt_res</td>
     <td></td>
    </tr>
    <tr>
     <td>0x2D</td>
     <td>func_config</td>
     <td>Various functional config bits</td>
     <td>
        U16:<br>
        bit 0: switch<br>
        bit 1: scrl<br>
        bit 2: balance_en<br>
        bit 3: chg_balance_en<br>
        bit 4: led_en<br>
        bit 5: led_num<br>
     </td>
     <td> -- </td>
     <td>switch, scrl, balance_en, chg_balance_en, led_en, led_num</td>
     <td>
        switch: Assume this enables the sw connector on the board?<br>
        scrl: ?<br>
        balance_en: Enable cell balancing<br>
        chg_balance_en: Enable balancing during charge (balance_en must also be on)<br>
        led_en: Assume that this enables LEDs?<br>
        led_num: Show battery level with the LEDs in 20% increments<br>
     </td>
    </tr>
    <tr>
     <td>0x2E</td>
     <td>ntc_config</td>
     <td>Enable / disable NTCs (thermistors)</td>
     <td>
         U16:<br>
         bit 0: NTC 1<br>
         bit 1: NTC 2<br>
         ... etc ...
     </td>
     <td> -- </td>
     <td> ntc1, ntc2 ... ntc8 </td>
     <td></td>
    </tr> 
    <tr>
     <td>0x2F</td>
     <td>cell_cnt</td>
     <td>Number of cells in the pack</td>
     <td>U16</td>
     <td>1 cell</td>
     <td>cell_cnt</td>
     <td></td>
    </tr> 
    <tr>
     <td>0x30</td>
     <td>fet_ctrl</td>
     <td>???</td>
     <td>U16</td>
     <td>1S</td>
     <td>fet_ctrl</td>
     <td></td>
    </tr> 
    <tr>
     <td>0x31</td>
     <td>led_timer</td>
     <td>???</td>
     <td>U16</td>
     <td>1S</td>
     <td>led_timer</td>
     <td>Assume it's the number of seconds the LEDs stay on when status changes?</td>
    </tr> 
    <tr>
     <td>0x36</td>
     <td>covp_high</td>
     <td>Secondary cell overvoltage protection</td>
     <td>U16</td>
     <td>1mV</td>
     <td>covp_high</td>
     <td></td>
    </tr> 
    <tr>
     <td>0x37</td>
     <td>cuvp_high</td>
     <td>Secondary cell undervoltage protection</td>
     <td>U16</td>
     <td>1mV</td>
     <td>cuvp_high</td>
     <td></td>
    </tr> 
    <tr>
     <td>0x38</td>
     <td>sc_dsgoc2</td>
     <td>Short circuit and secondary overcurrent settings</td>
     <td>
        2 bytes:<br>
        <br>
        &nbsp;byte 0: <br>
        &nbsp;&nbsp;bit 7: sc_dsgoc_x2<br>
        &nbsp;&nbsp;bits 4:3: sc_delay<br>
        &nbsp;&nbsp;&nbsp;0 = 70uS<br>
        &nbsp;&nbsp;&nbsp;1 = 100uS<br>
        &nbsp;&nbsp;&nbsp;2 = 200uS<br>
        &nbsp;&nbsp;&nbsp;3 = 400uS<br>
        &nbsp;&nbsp;bits 2:0: sc<br>
        &nbsp;&nbsp;&nbsp;0 = 22mV<br>
        &nbsp;&nbsp;&nbsp;1 = 33mV<br>
        &nbsp;&nbsp;&nbsp;2 = 44mV<br>
        &nbsp;&nbsp;&nbsp;3 = 56mV<br>
        &nbsp;&nbsp;&nbsp;4 = 67mV<br>
        &nbsp;&nbsp;&nbsp;5 = 78mV<br>
        &nbsp;&nbsp;&nbsp;6 = 89mV<br>
        &nbsp;&nbsp;&nbsp;7 = 100mV<br>
        <br>
        &nbsp;byte 1: <br>
        &nbsp;&nbsp;bits 7:4: dsgoc2_delay<br>
        &nbsp;&nbsp;&nbsp;0 = 8ms<br>
        &nbsp;&nbsp;&nbsp;1 = 20ms<br>
        &nbsp;&nbsp;&nbsp;2 = 40ms<br>
        &nbsp;&nbsp;&nbsp;3 = 80ms<br>
        &nbsp;&nbsp;&nbsp;4 = 160ms<br>
        &nbsp;&nbsp;&nbsp;5 = 320ms<br>
        &nbsp;&nbsp;&nbsp;6 = 640ms<br>
        &nbsp;&nbsp;&nbsp;7 = 1280ms<br>
        &nbsp;&nbsp;bits 3:0: dsgoc2<br>
        &nbsp;&nbsp;&nbsp;0 = 8mV<br>
        &nbsp;&nbsp;&nbsp;1 = 11mV<br>
        &nbsp;&nbsp;&nbsp;2 = 14mV<br>
        &nbsp;&nbsp;&nbsp;3 = 17mV<br>
        &nbsp;&nbsp;&nbsp;4 = 19mV<br>
        &nbsp;&nbsp;&nbsp;5 = 22mV<br>
        &nbsp;&nbsp;&nbsp;6 = 25mV<br>
        &nbsp;&nbsp;&nbsp;7 = 28mV<br>
        &nbsp;&nbsp;&nbsp;8 = 31mV<br>
        &nbsp;&nbsp;&nbsp;9 = 33mV<br>
        &nbsp;&nbsp;&nbsp;A = 36mV<br>
        &nbsp;&nbsp;&nbsp;B = 39mV<br>
        &nbsp;&nbsp;&nbsp;C = 42mV<br>
        &nbsp;&nbsp;&nbsp;D = 44mV<br>
        &nbsp;&nbsp;&nbsp;E = 47mV<br>
        &nbsp;&nbsp;&nbsp;F = 50mV<br>
     </td>
     <td> -- </td>
     <td>sc, sc_delay, dsgoc2, dsgoc2_delay, sc_dsgoc_x2</td>
     <td>
        sc: Assuming this is the voltage across that shunt that would indicate short-circuit<br>
        sc_delay: Assuming this is how long that voltage would need to be present<br>
        dsgoc2: Assuming this is the voltage across that shunt that would indicate secondary overcurrent<br>
        dsgoc2_delay: Assuming this is how long that voltage would need to be present<br>
        dsgoc2_x2: Boolean; if set, assume all the values here are doubled<br>
     </td>
    </tr> 
    <tr>
     <td>0x39</td>
     <td>cxvp_high_delay_sc_rel</td>
     <td>Secondary cell under/over voltage release times, and short circuilt release time</td>
     <td>
        2 bytes:<br>
        <br>
        &nbsp;byte 0: <br>
        &nbsp;&nbsp;bits 7:6: cuvp_high_delay<br>
        &nbsp;&nbsp;&nbsp;0 = 1S<br>
        &nbsp;&nbsp;&nbsp;1 = 4S<br>
        &nbsp;&nbsp;&nbsp;2 = 8S<br>
        &nbsp;&nbsp;&nbsp;3 = 16S<br>
        &nbsp;&nbsp;bits 5:4: covp_high_delay<br>
        &nbsp;&nbsp;&nbsp;0 = 1S<br>
        &nbsp;&nbsp;&nbsp;1 = 2S<br>
        &nbsp;&nbsp;&nbsp;2 = 4S<br>
        &nbsp;&nbsp;&nbsp;3 = 8S<br>
        <br>
        &nbsp;byte 1: Short Circuit release time, seconds
     </td>
     <td> -- </td>
     <td>cuvp_high_delay, covp_high_delay, sc_rel</td>
     <td></td>
    <tr>
     <td>0xA0</td>
     <td>mfg_name</td>
     <td>Manufacturer name</td>
     <td>
        Variable length string: <br>
        Byte 0: Length of string (n)<br>
        Byte 1 ... n + 1: String<br>
     </td>
     <td> -- </td>
     <td>mfg_name</td>
     <td></td>
    </tr> 
    <tr>
     <td>0xA1</td>
     <td>device_name</td>
     <td>Device name</td>
     <td>
        Variable length string: <br>
        Byte 0: Length of string (n)<br>
        Byte 1 ... n + 1: String<br>
     </td>
     <td> -- </td>
     <td>device_name</td>
     <td></td>
    </tr> 
    <tr>
     <td>0xA2</td>
     <td>barcode</td>
     <td>Barcode</td>
     <td>
        Variable length string: <br>
        Byte 0: Length of string (n)<br>
        Byte 1 ... n + 1: String<br>
     </td>
     <td> -- </td>
     <td>barcode</td>
     <td></td>
    </tr> 
    <tr>
     <td>0xAA</td>
     <td>error_cnts</td>
     <td>Various error condition counts</td>
     <td>11 U16</td>
     <td> -- </td>
     <td>sc_err_cnt, chgoc_err_cnt, dsgoc_err_cnt, covp_err_cnt, cuvp_err_cnt, chgot_err_cnt, chgut_err_cnt, dsgot_err_cnt, dsgut_err_cnt, povp_err_cnt, puvp_err_cnt</td>
     <td>READ ONLY</td>
    </tr> 
    <tr>
     <td>0xB0 .. 0xCF</td>
     <td></td>
     <td>Cell voltage calibration registers (32)</td>
     <td>U16</td>
     <td>1mV</td>
     <td></td>
     <td>Write the actually measured value of each cell to set the calibration</td>
    </tr> 
    <tr>
     <td>0xD0 .. 0xD7</td>
     <td></td>
     <td>NTC calibration registers (8)</td>
     <td>U16</td>
     <td>0.1K</td>
     <td></td>
     <td>Write the actually measured temperature of each NTC to set the calibration</td>
    </tr> 
    <tr>
     <td>0xE1</td>
     <td></td>
     <td>MOSFET control</td>
     <td>
        U16:<br>
        &nbsp;bit 0: Set to disable charge FET<br>
        &nbsp;bit 1: Set to disable discharge FET<br>
     </td>
     <td>--</td>
     <td></td>
     <td></td>
    </tr> 
    <tr>
     <td>0xE2</td>
     <td></td>
     <td>Balance control</td>
     <td>
        U16:<br>
        &nbsp;0x01: Open odd cells
        &nbsp;0x02: Open even cells
        &nbsp;0x03: Close all cells
     </td>
     <td>--</td>
     <td></td>
     <td>To exit this mode: Enter, then exit factory mode.</td>
    </tr> 
    <tr>
     <td>0xAD</td>
     <td></td>
     <td>Idle Current Calibration</td>
     <td>
        U16: Write 0x0 when no current is flowing
     </td>
     <td>--</td>
     <td></td>
     <td></td>
    </tr> 
    <tr>
     <td>0xAE</td>
     <td></td>
     <td>Charge Current Calibration</td>
     <td>U16</td>
     <td>10mA<td>
     <td>Write the actual current. This value is positive.</td>
    </tr> 
    <tr>
     <td>0xAF</td>
     <td></td>
     <td>Discharge Current Calibration</td>
     <td>U16</td>
     <td>10mA<td>
     <td>Write the actual current. This value is positive.</td>
    </tr> 
    <tr>
     <td>0xE0</td>
     <td></td>
     <td>Capacity remaining</td>
     <td>U16</td>
     <td>10mAH<td>
     <td></td>
    </tr> 

</table>