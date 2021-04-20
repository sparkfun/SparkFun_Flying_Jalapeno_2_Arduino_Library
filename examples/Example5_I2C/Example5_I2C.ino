/*
  The Flying Jalapeño test jig can have an I2C buffer on board. This shows how to enable the buffer
  and transmit I2C to the target

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
  Serial.println("I2C Enable Example");

  FJ2.enableI2CBuffer(); //Enable the I2C buffer

  FJ2.setVoltageV1(3.3); //Get ready to set V1 to 3.3V

  if(FJ2.isV1Shorted() == false)
  {
    Serial.println("Whoa! Short on power rail V1");
    while(1); // Do nothing more
  }

  //Now power up the target
  FJ2.enableV1(); //Turn on V1

  //Wait for the power to be stable
  delay(1000);

  //Do a full scan of all I2C addresses with debug enabled to see what is connected
  //(If the address parameter is zero, verifyI2Cdevice scans all addresses from 1 to 126 inclusive)
  FJ2.verifyI2Cdevice(0, true);
}

void loop()
{

}
