Overkill Solar BMS Monitor Library
==================================

Introduction
------------

This library communicates with an Overkill Solar BMS (Battery Monitoring
System), which can be purchased from https://overkillsolar.com/

If you'd like to purchase an Overkill Solar battery monitoring system, use this
link:

- https://overkillsolar.com/?myboi=187467882

Please note that Overkill Solar supports the open-source community, and by
clicking on the link above, you will be supporting the developer who wrote this
library.

This library is capable of returning the following information:

- Total voltage, in volts
- Current in amps
- Balance capacity in amp-hours
- Rate capacity in amp-hours
- Cycle count
- Production date:
- Balance status of each cell
- Software version
- Remaining state of charge, 0-100 percent
- Charge FET on/off status
- Discharge FET on/off status
- # of batteries in series (int)
- # of NTCS [the number of temperature sensors present] (int)
- Temperatures of each temperature sensor, in Celsius (up to 16 floats)
- Voltages of each cell, in volts
- Protection status flags:

    - Software lock MOS
    - Front-end detection IC error
    - Short circuit protection
    - Discharge overcurrent protection
    - Charging overcurrent protection
    - Discharge low-temperature protection
    - Discharge over-temperature protection
    - Charging low temperature protection
    - Charging over-temperature protection
    - Whole pack undervoltage protection
    - Whole pack overvoltage protection
    - Single cell undervoltage protection
    - Single cell overvoltage protection

"Bare Metal" Disclaimer
-----------------------

By using this library, you accept the small but quantifiable risk that 
you may damage or destroy the BMS or batteries if precautions are not followed.
We've tested this library and have taken precautions to ensure that it works
as it is expected to, but we cannot test all possible configurations, and
cannot give any guarantees once it is modified or used on an untested
platform.

If you're an experienced embedded software developer, then the usage of this
libray should be extremely straightforward.  We've followed standard
Arduino library standards of practice.

If not, then please heed the following advice:  

- Realize that while the Arduino platform is one of the easiest platforms to
  learn embedded computing on, you're still moving ones and zeros around
  on bare metal (https://en.wikipedia.org/wiki/Bare_machine).  This means that
  small changes may have a large (and potentially disastrous) effect.  Unlike
  a laptop, PC, or mobile device, memory locations are not protected and
  therefore, it is easy to shoot yourself in the foot if you are not careful.
  Our advice to you, dear reader, is to practice your Arduino mastery on a
  small sensor or display first, before graduating into something of this
  magnitude.
- Read the introduction (https://www.arduino.cc/en/Guide/Introduction) 
  if you are new to the Arduino platform.
- Read the documentation on installing Arduino libraries
  (https://www.arduino.cc/en/Guide/Libraries) and writing libraries
  (https://www.arduino.cc/en/Hacking/LibraryTutorial) if you are new to usiung,
  writing, or modifying libraries on the Arduino platform.
- This library was written for the Arduino AVR platform.  Other platforms
  may work, but are not supported at this time.  Specifically, the libray has
  been tested to work with the Arduino Pro Micro and Arduino Mega 2560.
- Solar battery systems can produce tremendous energy, and can be dangerous
  if proper precautions are not followed.  A complete course in electrical
  theory of operation is of-of-scope for this document.


Theory of Operation
-------------------

This library was written to conform with the serial protocol published here:
https://github.com/FurTrader/OverkillSolarBMS/blob/master/JBD%20Protocol%20English%20version.pdf
The protocol is Modbus-like, in that it's a binary protocol that sends and
receives frames that consist of a fixed start byte, an address register,
variable-length data, a checksum, and a fixed stop byte.

The Arduino communicates with the BMS over a 5V TTL asynchronous serial port
connection, at 9600 baud, 8 data bits, no parity, and 1 stop bit. 

The library is intended to be used in a similar manner to typical Arduino
libraries.  The constructor call is typically placed near the top of the
sketch.  The library is initialized within the sketch's ``setup()``
function.  And in order for it to read and write to the serial port, the
``bms.main_task()`` function must be called repeatedtly in the sketch's
``loop()`` function.  Typically, the sketch will need to do other things
while the serial port task is running.  For example, writing the received
data to the screen, or reading buttons or encoders.  The Arduino is not
multi-threaded, and cannot run things simultaneously.  However, as long as
each task is very short (on the order of microseconds or milliseconds),
the tasks can take turns carrying out their role.  The BMS library's main task
handles all of the reading and writing on the BMS serial port.  It needs
to be called at least once every 25 milliseconds (it may be called faster).
This means that the other tasks being performed should not take longer
than 25 milliseconds to execute.  Even a single ``delay(1000);`` call
would block access to the serial port.  If this strategy isn't desirable,
then it's possible to setup a periodic hardware timer to call an interrupt,
and have the interrupt call the ``bms.main_task()`` function.  This may
have other side-effects, so only use this as a last resort.

The library periodically requests basic info and cell voltage from the BMS
at a default rate of 1000 milliseconds. This periodic rate may be configured
to any value, although values under 250 milliseconds are not recommended.

Response data is received from the BMS in the ``bms.serial_rx_task()`` method.
Whenever this method is called, it will read all available bytes from the
serial port and sequentially pass them to the framer.  The framer's job is
to reconstruct each frame.  It will begin by waiting for a start byte, then
the command code (register address), then the status byte, the length byte,
the data, two checksum bytes, and finally the stop byte.  The internal
state of the framer is stored in memory, so in some cases, the
``bms.serial_rx_task()`` may need to be called several times before a full frame
is received.  

Once a frame is received, it is checked for validity.  If the checksum passes,
and the status by indicates that is correct, the data is stored internally.
This data is stored in a raw state that's much more memory efficient than its
full decoded form.  When the data is requested, the encoded form is converted
to the full form and returned.  For example, the voltage is stored in 2 bytes
of memory, but when requested, it gets converted to a floating-point number
which is 4 bytes.

The polling rate of the data being requested from the BMS can be changed,
using the ``bms.set_query_rate()`` method.  Simply provide it with an integer
period time, in miliseconds.  There is an example in the section below.

If the BMS is not responding, then the method ``bms.get_comm_error_state()``
will return true.  If it is responding and outputing valid data, it will
return false.


Library Usage
-------------

See the tutorial file, which should be included in this document.  You will
also find examples in the examples folder.


Contributing
------------

Send suggestions and bugs to OverkillSolar@gmail.com 


Copyright & License
-------------------

Copyright 2020 Neil Jansen (njansen1@gmail.com)

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
