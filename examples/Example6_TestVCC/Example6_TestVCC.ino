/*
  This example shows how to test if the FJ2 VCC has been set correctly.

  Written by Paul Clark, April 20th, 2021
  Based on earlier work by Pete Lewis and NES

  Select Mega2560 from the boards list
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2

//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino

FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

void setup()
{
  Serial.begin(115200);
  Serial.println("Test VCC example");

  //FJ2.enableDebugging(); //Enable helpful debug messages on Serial

  //We told the FJ2 library that we expect VCC to be 3.3V when we instantiated FJ2
  //We can check that VCC really is 3.3V by calling testVCC()
  //testVCC() checks the VCC by reading the voltage of the 3.3V Zener diode on FJ2_BRAIN_VCC_A0
  //testVCC() returns true if VCC is OK
  
  if(FJ2.testVCC() == false)
  {
    while(1) // Stay in this loop until reset
    {
      Serial.println("Whoa! VCC is wrong! Reset me when you have corrected VCC...");
      FJ2.SOS(); // Blink SOS on STAT LED
    }
  }

  Serial.println("VCC is OK!");

  FJ2.setVoltageV1(3.3); //Get ready to set V1 to 3.3V
  FJ2.setVoltageV2(3.3); //Get ready to set V2 to 3.3V

  if(FJ2.isV1Shorted() == true)
  {
    Serial.println("Whoa! Short detected on power rail V1");
    while(1); // Do nothing more
  }

  if(FJ2.isV2Shorted() == true)
  {
    Serial.println("Whoa! Short detected on power rail V2");
    while(1); // Do nothing more
  }

  Serial.println("No shorts detected!");

  //Now power up the target
  //FJ2.enableV1(); //Turn on V1
  //FJ2.enableV2(); //Turn on V2
}

void loop()
{

}
  
