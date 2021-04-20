/*
  This example shows how to turn on the two onboard voltage regulators.

  V1 can be set to 5V or 3.3V
  V2 can be set to 5, 4.2, 3.7, and 3.3V. This is handy to simulate Lipo power or charging.

  Pete Lewis, started on 11/3/2016
  Contributions by NES November 15th, 2016
  Modified for FJ2 by Paul Clark, April 19th, 2021

  Select Mega2560 from the boards list
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2

//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino

FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

void setup()
{
  Serial.begin(115200);
  Serial.println("Power Control Example");

  //V1 can be set to 5V or 3.3V
  FJ2.setVoltageV1(3.3); //Set V1 to 3.3V. V1 isn't enabled until we call enableV1

  //V2 can be set to 5, 4.2, 3.7, and 3.3V. This is handy to simulate Lipo power or charging.
  FJ2.setVoltageV2(3.3); //Set V2 to 3.3V. V2 isn't enabled until we call enableV2
}

void loop()
{
  if (FJ2.isProgramAndTestPressed() == true)
  {
    Serial.println("Powering up V1!");

    FJ2.enableV1(); //Turn on V1
    FJ2.disableV2(); //Turn off V2
    
    FJ2.dash(FJ2_LED_PROGRAM_AND_TEST_PASS);
  }
  else if (FJ2.isTestPressed() == true)
  {
    Serial.println("Powering up V2!");
    
    FJ2.disableV1(); //Turn off V1
    FJ2.enableV2(); //Turn on V2
    
    FJ2.dash(FJ2_LED_TEST_PASS);
  }
}
