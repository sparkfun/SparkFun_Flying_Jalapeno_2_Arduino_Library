/*
  This example shows how to test if there are VCC to GND shorts on the target board.

  To do this we push a small current across a 10/100 resistor divider and look at the ADC value.
  If ADC value is near 486 then there is a jumper.
  If ADC value is far from 486, there is no jumper on the target board.
  See FJ2 schematic 'PreTests' area to see the resistor and ADC setup.

  Test for shorts on the two power supplies using:
  isV1Shorted()
  isV2Shorted()

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
  Serial.println("Test for shorts");

  //FJ2.enableDebugging(); //Uncomment this line to enable helpful debug messages on Serial

  FJ2.setVoltageV1(3.3); //Get ready to set V1 to 3.3V
  FJ2.setVoltageV2(3.3); //Get ready to set V2 to 3.3V

  if(FJ2.isV1Shorted() == false)
  {
    Serial.println("Whoa! Short detected on power rail V1");
    while(1); // Do nothing more
  }

  if(FJ2.isV2Shorted() == false)
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
