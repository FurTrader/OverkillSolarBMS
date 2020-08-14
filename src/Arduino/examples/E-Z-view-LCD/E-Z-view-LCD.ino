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

//This Project as of V0.1 uses a 20x4 LCD with I2C backpack and a Pro-Micro processor.
//The backlight is controlled by a PWM pin instead of the backpack.


#include "bms.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

OverkillSolarBms bms = OverkillSolarBms(); //library object for the BMS

LiquidCrystal_I2C lcd(0x3F,20,4); //Library object for the LCD
//set the LCD address to 0x3F for a 20 chars and 4 line display
//your LCD might have a differnt address, check your documentation.

//global variables
long lastmillis;
bool heartbeatflag;
int BackLightBrightness = 100; //set the backlight level. 0-255
//Limit this to 100 for power managment! The BMS cannot supply any more power on the comm port.

//map of a custom lcd character
uint8_t smile[8] = { 0x04, 0x02, 0x12, 0x01, 0x01, 0x12, 0x02, 0x04 }; 

void setup() {
  delay(500);
  pinMode(10, OUTPUT); //This pin is used to dim the backlight
  
  //initialize the serial ports. 
  //Written for a board with a UART on Serial1 such as Mega, Leonoardo, Pro-Micro.
  Serial.begin(115200);
  Serial1.begin(9600);

  //the promicro hates this
  //while (!Serial) { }  // Wait for the debug serial port to initialize
    
  while (!Serial1) {  // Wait for the BMS serial port to initialize
  }

  bms.begin(&Serial1); //Initialize the BMS library object
  
  lcd.init(); // initialize the lcd library object
  
  //print a Splash Screen on the LCD
  lcd.backlight();
  analogWrite(10, BackLightBrightness);
  lcd.home();
  lcd.noBlink();
  lcd.noCursor();
  lcd.clear();
  lcd.print("Overkill Solar");
  lcd.setCursor(0, 2);
  lcd.print("E-Z BMS Monitor v0.1"); 

  //now wait while running the BMS library to establish communication
  lastmillis = millis();
  while ((millis() - lastmillis) < 4000){ // timer
    bms.main_task(); //call the BMS library every loop.
  }//end timer
  
  analogWrite(0, BackLightBrightness);
  analogWrite(10, 0);
  lcd.clear();
  analogWrite(10, BackLightBrightness);
  lcd.createChar(0, smile); // Sends the custom char to lcd to store in memory


  //now display the model and cell count
  //if any
  lcd.setCursor(0, 0);
  lcd.print("Found "); 
  lcd.print(bms.get_num_cells()); 
  lcd.print(" cell BMS");
  lcd.setCursor(0, 2);
  String bmsname = bms.get_bms_name();
  bmsname.remove(19, 5);
  lcd.print(bmsname);
  Serial.println(bmsname);
  //print BMS model number
  delay(3000); //delay is ok here
  
}//end setup

void loop() {
  bms.main_task(); //call the BMS library every loop.

  //Print to the LCD, avoiding the use of delay()
  if ((millis() - lastmillis) > 1000){ //1 second timer
    lastmillis = millis();


Serial.println(bms.get_num_cells());
//bms.debug();
ProtectionStatus foo = bms.get_protection_status();
//Then foo.whateve
Serial.println(foo.discharge_low_temperature_protection, DEC);

    //check communication status
    if (bms.get_comm_error_state()){ //returns false if comm is ok, true if it fails.
      //print error message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Communication"); 
      lcd.setCursor(0, 1);
      lcd.print("      Failure"); 
      lcd.setCursor(0, 3);
      lcd.print(" No reply from BMS"); 
    }else{    
      //normal display
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
      //loop prints each cell voltage. This version is only for 4 cell BMSs
      for (int i = 0; i <= 3; i++) {
        lcd.print((bms.get_cell_voltage(i) *1000), 0);
        lcd.print(" ");
      }
      AlarmMsg();
    }
    
    heartbeat(); //prints an indicator to verify the program is running

  }//end timer
  
}//end loop()

void AlarmMsg(){
  lcd.setCursor(0, 1);
  if(!bms.get_discharge_mosfet_status()){
    lcd.print("Discharge OFF");
  }else{
    lcd.print("No Alarms");
  }
  
}

void heartbeat() { //prints a heartbeat, to verify program execution.
  heartbeatflag = !heartbeatflag;
  lcd.setCursor(19, 0);
  if (heartbeatflag){lcd.print((char)0);}else{lcd.print(" ");}
}//end heartbeat()
