/*
  This example shows how to test if V1 and V2 are correct (or if something is dragging them down)

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
  Serial.println("Test V1 and V2 example");

  //FJ2.enableDebugging(); // Enable helpful debug messages on Serial

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
  FJ2.enableV1(); //Turn on V1
  FJ2.enableV2(); //Turn on V2

  //isV1Shorted() and isV2Shorted() check for an actual short on V1 and V2 - before the power is turned on.
  //But what if there is a heavy active load on V1 or V2? Or a fault in the FJ2 MicroPower section?
  //testVoltage will tell us if V1/V2 are OK (based on what we expect them to be from setVoltageV1/setVoltageV2)
  //testVoltage returns true if the voltage is OK
  //
  //Note: due to the 10k/11k divider on the PT_READ pins, we can only verify voltages which are lower than VCC * 0.9
  //
  if(FJ2.testVoltage(1) == false) // Test V1
  {
    Serial.println("Whoa! V1 is out of bounds");
    while(1); // Do nothing more
  }

  if(FJ2.testVoltage(2) == false) // Test V2
  {
    Serial.println("Whoa! V2 is out of bounds");
    while(1); // Do nothing more
  }

  Serial.println("V1 and V2 are OK!");
}

void loop()
{

}
  
