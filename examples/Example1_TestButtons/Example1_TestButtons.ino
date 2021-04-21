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
  //The library conatins a number of button-press functions:
  //
  //isPretestPressed()  isProgramAndTestPressed()  isButton1Pressed()
  //  will return true if button 1 is being pushed right now
  //
  //isTestPressed()  isButton2Pressed()
  //  will return true if button 2 is being pushed right now
  //
  //waitForButtonPress(timeoutMillis, minimumHoldMillis)
  //  will return 1 or 2 if a button has been held continuously for at least minimumHoldMillis
  //  otherwise it will return zero
  //
  //waitForButtonPressRelease(timeoutMillis, minimumHoldMillis, minimumReleaseMillis)
  //  will return 1 or 2 if a button has been held continuously for at least minimumHoldMillis
  //  and then has been released for at least minimumReleaseMillis
  //  otherwise it will return zero. Button 1 takes priority over button 2 (if both are pressed).
  //  I.e. it provides full debounce.
  //  This function is handy for test loops where you might need to check if the button has been pressed
  //    _and_ released before moving on
  //
  //waitForButtonReleasePressRelease(timeoutMillis, minimumPreReleaseMillis, minimumHoldMillis, minimumPostReleaseMillis)
  //  will return 1 or 2 if:
  //  - Neither button is pressed initially (for at least minimumPreReleaseMillis).
  //  - Then a button has been held for at least minimumHoldMillis
  //    and then has been released for at least minimumPostReleaseMillis.
  //  Otherwise it will return zero. Button 1 takes priority over button 2 (if both are pressed).
  //  I.e. it provides full debounce.
  //  This function is handy for test loops where you might need to check if the previous button press has completed
  //    before responding to a new one
  
  
  //Is user pressing the Program & Test (PreTest) button right now?
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

/*
  //Or you could do it this way:
  int button = FJ2.waitForButtonPressRelease(); // Check for a button press using the default timeout, hold and release times
  if (button == 1)
  {
    Serial.println("You pressed Program_&_Test!");
    
    FJ2.dash(FJ2_LED_PROGRAM_AND_TEST_PASS);
  }
  else if (button == 2)  
  {
    Serial.println("You pressed Test!");
    
    FJ2.dash(FJ2_LED_TEST_PASS);
  }
*/
}
