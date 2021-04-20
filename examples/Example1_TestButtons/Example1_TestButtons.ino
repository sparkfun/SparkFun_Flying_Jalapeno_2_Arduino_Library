/*
  This example shows how to read the two capacitive touch buttons on every FJ2 test jig.

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
  Serial.println("Reading the cap sense buttons example");
}

void loop()
{
  //Is user pressing the Program & Test (PreTest) button?
  if (FJ2.isProgramAndTestPressed() == true)
  {
    Serial.println("You pressed Program_&_Test!");
    
    FJ2.dash(FJ2_LED_PROGRAM_AND_TEST_PASS);
  }
  else if (FJ2.isTestPressed() == true)
  {
    Serial.println("You pressed Test!");
    
    FJ2.dash(FJ2_LED_TEST_PASS);
  }
}
