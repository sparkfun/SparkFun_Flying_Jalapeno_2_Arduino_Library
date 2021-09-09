/*
  This example reads and displays the raw values from the the two capacitive touch buttons on every FJ2 test jig.
  Useful for determining the value for the threshold.

  Program_And_Test is Button_1
        Test       is Button_2

  Pete Lewis, started on 11/3/2016
  Contributions by NES November 15th, 2016
  Modified for FJ2 by Paul Clark, August 31st, 2021

  Select Mega2560 from the boards list
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2

//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino

FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

void setup()
{
  Serial.begin(115200);
  Serial.println("Reading the raw values from the cap sense buttons (1 sample per reading)");
  Serial.println();
  Serial.println("Program_And_Test is Button_1");
  Serial.println("      Test       is Button_2");
  Serial.println();
  Serial.println("   Button_1\t   Button_2");

  //FJ2.setCapSenseSamples(30); // Set the number of cap sense samples (default is 30)

  //FJ2.enableDebugging(); //Uncomment this line to enable helpful debug messages on Serial
}

void loop()
{
  //Read the raw values from the two buttons
  long button1 = FJ2.FJ2button1->capacitiveSensorRaw(FJ2._capSenseSamples);
  long button2 = FJ2.FJ2button2->capacitiveSensorRaw(FJ2._capSenseSamples);
  
  printSpaced(button1); // Print the reading from Button_1
  Serial.print("\t");
  printSpaced(button2); // Print the reading from Button_2

  //Warn the user if either value is less than zero
  if ((button1 < 0) || (button2 < 0))
  {
    Serial.print("\tWarning! Negative values indicate a problem (usually that the send resistor is too large)");
  }
  
  Serial.println();
}

void printSpaced(long val)
{
  if (val >= 0) Serial.print(" "); // Align negative numbers correctly
  if (val < 1000000000) Serial.print(" ");
  if (val < 100000000) Serial.print(" ");
  if (val < 10000000) Serial.print(" ");
  if (val < 1000000) Serial.print(" ");
  if (val < 100000) Serial.print(" ");
  if (val < 10000) Serial.print(" ");
  if (val < 1000) Serial.print(" ");
  if (val < 100) Serial.print(" ");
  if (val < 10) Serial.print(" ");
  Serial.print(val);
}
