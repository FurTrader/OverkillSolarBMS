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

#include "bms.h"
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

OverkillSolarBms bms = OverkillSolarBms(); //library object for the BMS

LiquidCrystal_PCF8574 lcd(0x3F); //Library object for the LCD
// set the LCD address to 0x3F for a 20 chars and 4 line display

//global variables
long lastmillis;
byte counttelltale;

int testChar1[8] = {0x1,0x0,0x0,0x0,0x0,0x0,0x0};//map of a custom lcd character


void setup() {
  delay(500);
  Serial.begin(115200);
  Serial1.begin(9600);  // Pins 19 (RX) and 18 (TX) on the Mega 2560

  //the promicro hates this
  //while (!Serial) { }  // Wait for the debug serial port to initialize
    
  while (!Serial1) {  // Wait for the BMS serial port to initialize
  }

  //Serial.println('OverkillSolarBMS boot!');
  bms.begin(&Serial1);

  int error;
  Wire.begin();
  Wire.beginTransmission(0x3F);
  error = Wire.endTransmission();

  lcd.createChar(0, testChar1); // Sends the custom char to lcd; //testing a custom character
  
  //print a Splash Screen on the LCD
  lcd.setBacklight(255);
  lcd.home();
  lcd.noBlink();
  lcd.noCursor();
  lcd.clear();
  lcd.print("Overkill Solar");
  lcd.setCursor(0, 2);
  lcd.print("E-Z BMS Monitor v0.1");
  delay(5000);
  lcd.setBacklight(0);
  lcd.clear();
  delay(400);
  lcd.setBacklight(255);
}//end setup

void loop() {
    bms.loop(); //call the BMS library every loop. change the function name to "runBMS"

  //refresh the LCD, avoiding the use of delay()
  if ((millis() - lastmillis) >1000){ //1 second timer
    lastmillis = millis();
      
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SOC ");
    lcd.print(bms.get_state_of_charge());
    lcd.print("%");
    //calculate watts
    float watts = ((bms.get_voltage()) * (bms.get_current()));
    lcd.setCursor(10, 0);
    if(watts > 0){ lcd.print("+"); }
    lcd.print(watts, 0);
    lcd.print(" w");
    lcd.setCursor(0, 2);
    lcd.print(bms.get_voltage());
    lcd.print(" V");
    lcd.setCursor(10, 2);
    if(bms.get_current() > 0){ lcd.print("+"); }
    lcd.print(bms.get_current());
    lcd.print(" A");
    lcd.setCursor(0, 3);

    for (int i = 0; i <= 3; i++) {
      lcd.print((bms.get_cell_voltage(i) *1000), 0);
      lcd.print(" ");
    }
    telltale(); //prints an indicator to verify the program is running

    Serial.print("debug ");
    Serial.println(counttelltale);
  }//end if, timer
  
}//end loop

void telltale() { //prints a sequence of characters, to verify program execution.
  char telltaleMessage[4] = { '<' , '-' , '>' , '-' };
  if (counttelltale > 3){counttelltale = 0;}
  lcd.setCursor(19, 0);
  lcd.print(telltaleMessage[counttelltale]);
  //lcd.print((char)0); // Custom char
  counttelltale++;
}
