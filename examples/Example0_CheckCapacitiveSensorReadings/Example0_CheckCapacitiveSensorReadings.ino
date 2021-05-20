/*
  This example reads and displays the values from the the two capacitive touch buttons on every FJ2 test jig.
  Useful for determining the value for the threshold.

  In your test script, you can call FJ2.setCapSenseThreshold to set the button threshold. The default is 5000
  E.g. calling FJ2.setCapSenseThreshold(2000); will set the threshold to 2000
  
  You can also call FJ2.setCapSenseSamples to set the number of samples taken when reading the button. The default is 30
  E.g. calling FJ2.setCapSenseSamples(20); will set the number of samples to 20
  
  Program_And_Test is Button_1
        Test       is Button_2

  Pete Lewis, started on 11/3/2016
  Contributions by NES November 15th, 2016
  Modified for FJ2 by Paul Clark, May 20th, 2021

  Select Mega2560 from the boards list
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2

//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino

FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

void setup()
{
  Serial.begin(115200);
  Serial.println("Reading the values from the cap sense buttons");
  Serial.println();
  Serial.println("In your test script, you can call FJ2.setCapSenseThreshold to set the button threshold. The default is 5000");
  Serial.println("E.g. calling FJ2.setCapSenseThreshold(2000); will set the threshold to 2000");
  Serial.println();
  Serial.println("You can also call FJ2.setCapSenseSamples to set the number of samples taken when reading the button. The default is 30");
  Serial.println("E.g. calling FJ2.setCapSenseSamples(20); will set the number of samples to 20");
  Serial.println();
  Serial.println("Program_And_Test is Button_1");
  Serial.println("      Test       is Button_2");
  Serial.println();
  Serial.println("   Button_1\t   Button_2");

  //FJ2.enableDebugging(); //Uncomment this line to enable helpful debug messages on Serial
}

void loop()
{
  //Read the values from the two buttons
  long button1 = FJ2.FJ2button1->capacitiveSensor(FJ2._capSenseSamples);
  long button2 = FJ2.FJ2button2->capacitiveSensor(FJ2._capSenseSamples);
  
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
