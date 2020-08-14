/* Copyright 2020 Neil Jansen

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

// The purpose of this sketch is to set turn on the built-in LED (usually
// pin 13) whenever the state-of-charge goes lower than 30%.  This a simple
// example that does not require an LCD or other peripherals.

#include "bms.h"

OverkillSolarBms bms = OverkillSolarBms();
uint32_t last_soc_check_time;

#define SOC_POLL_RATE 2000  // milliseconds

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial1.begin(9600);
    while (!Serial1) {  // Wait for the BMS serial port to initialize
    }
    bms.begin(&Serial1);
    bms.set_query_rate(2000);  // Set query rate to 2000 milliseconds (2 seconds)
    last_soc_check_time = 0;
}

void loop() {
    bms.main_task();
    uint8_t soc = bms.get_state_of_charge();

    if (millis() - last_soc_check_time > SOC_POLL_RATE) {
        if (soc < 30) {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else {
            digitalWrite(LED_BUILTIN, LOW);
        }

        last_soc_check_time = millis();
    }
}
