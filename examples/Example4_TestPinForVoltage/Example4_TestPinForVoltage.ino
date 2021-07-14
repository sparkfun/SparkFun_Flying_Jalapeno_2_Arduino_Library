/*
  This example shows how to test a pin for a specific voltage.

  Here's the function:
  verifyVoltage(pin to test, expected voltage (float), percent of variance allowed (default 10 = 10%))

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
  Serial.println("Test pin for voltage example");

  //FJ2.enableDebugging(); //Uncomment this line to enable helpful debug messages on Serial

  //FJ2.setCapSenseThreshold(1000); //Uncomment this line to set the cap sense threshold to 1000. Default is 2000

  //FJ2.setCapSenseSamples(20); //Uncomment this line to set the number of cap sense samples to 20. Default is 30

  //FJ2.setAnalogReadSamples(50); //Uncomment this line to set the number of analog reads for averaging. Default is 25

  FJ2.setVoltageV1(3.3); //Get ready to set V1 to 3.3V
  FJ2.setVoltageV2(3.3); //Get ready to set V2 to 3.3V

  if(FJ2.isV1Shorted() == true)
  {
    Serial.println("Whoa! Short on power rail V1");
    while(1); // Do nothing more
  }

  if(FJ2.isV2Shorted() == true)
  {
    Serial.println("Whoa! Short on power rail V2");
    while(1); // Do nothing more
  }

  Serial.println("No shorts detected!");

  //Now power up the target
  FJ2.enableV1(); //Turn on V1
  FJ2.enableV2(); //Turn on V2
}

void loop()
{
  //This demonstrates the extended call to the function
  //You can omit the tolerance is you want to
  boolean result = FJ2.verifyVoltage(A0, 3.3, 15); //A0, 3.3V, within 15%
  
  if(result == true)
  {
    Serial.println("Pin is at 3.3V!");
  }
  else
  {
    Serial.println("Pin is NOT at 3.3V!");
  }

  delay(1000);  
}
  
