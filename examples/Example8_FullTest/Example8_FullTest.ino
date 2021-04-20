/*
  This example shows how to run a normal test procedure

  Pete Lewis, started on 11/3/2016
  Contributions by NES November 15th, 2016
  Modified for FJ2 by Paul Clark, April 19th, 2021

  Select Mega2560 from the boards list
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2

//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino

FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.


int failures = 0; //Number of failures by the main test routine

boolean targetPowered = false; //Keeps track of whether power supplies are energized

long preTestButton = 0; //Cap sense values for two main test buttons
long testButton = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Full test example.");

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

  FJ2.enableI2CBuffer(); //Enable the I2C buffer

  Serial.println("Press the Program&Test button to begin.");
}

void loop()
{
  if (FJ2.isProgramAndTestPressed() == true)
  {
    FJ2.dot(); //Blink status LED to indicate button press

    if (targetPowered == true) 
    {
      power_down(); //Power down the test jig (power_down is defined below)
    }
    else
    {
      //Check V1 and V2 for shorts to ground
      //Note: calling isV1Shorted or isV2Shorted _disables_ both V1 and V2
      if ((FJ2.isV1Shorted() == false) && (FJ2.isV2Shorted() == false))
      {
        Serial.println("Pre-test PASS, powering up...\n\r");

        FJ2.setVoltageV1(3.3); //Turn on power supply V1 at 3.3V
        FJ2.enableV1();
        
        delay(500);

        if(FJ2.testVoltage(1) == false) // Test V1 is within bounds
        {
          Serial.println("Whoa! V1 is out of bounds! Disabling it...");

          failures++; //Fail - heavy load on V1
          
          FJ2.disableV1();
          
          targetPowered = false;

          digitalWrite(FJ2_LED_FAIL, HIGH);
          digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, LOW);

          delay(500); // debounce touching
        }
        else
        {
          targetPowered = true;

          digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, HIGH);
          digitalWrite(FJ2_STAT_LED, LOW);

          delay(50); //Debounce
        }
      }
      else
      {
        Serial.println("Short detected on power rail V1 or V2\n\r");
        
        failures++; //Fail - short on V1 or V2

        FJ2.disableV1(); //Turn off power supply 1
        
        targetPowered = false;
        
        digitalWrite(FJ2_LED_FAIL, HIGH);
        digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, LOW);

        delay(500); // debounce touching
      }
    }
  }
  else if ((FJ2.isTestPressed() == true) && (targetPowered == true))
  {
    Serial.println("Running test");
    
    //Begin main test
    
    FJ2.statOn(); // Turn on the STAT LED

    digitalWrite(FJ2_LED_TEST_PASS, LOW);
    digitalWrite(FJ2_LED_FAIL, LOW);

    //if(test_3V3() == true) //Run main test code
    if(test() == true) //Run main test code
    {
      Serial.println("Test complete");
      digitalWrite(FJ2_LED_TEST_PASS, HIGH);
    }
    else
    {
      digitalWrite(FJ2_LED_FAIL, HIGH);
    }

    FJ2.disableV1(); //Turn off power supply 1
    Serial.println("Power rail V1 disabled");
    targetPowered = false;

    FJ2.statOff();
  }
}

boolean test()
{
  // add in your test code here
  return(true); //If we passed
}

//This is an example of testing a 3.3V output from the board sent to A2.
void test_3V3()
{
  Serial.println("Testing 3.3V output on board");

  //pin = pin to test
  //expectedVoltage = voltage we expect. 0.0 to 5.0 (float)
  //allowedPercent = allowed window for overage. 0 to 100 (int) (default 10%)
  //debug = print debug statements (default false)
  boolean result = FJ2.verifyVoltage(A2, 1.65, 10, true); // Voltage on A2 is 3.3V split by two 10Ks. Should be 1.65V. Check for 10%

  if (result == true) 
    Serial.println("Test success!");
  else
  {
    Serial.println("Test failure! See debug messages for details.");
    failures++;
  }
}

//Turn off all regulators
void power_down()
{
  Serial.println("Powering down target");

  FJ2.disableV1(); //Turn off power supply 1, but leave voltage selection at 3.3V
  FJ2.disableV2(); //Turn off power supply 1, but leave voltage selection at 4.2V

  targetPowered = false;

  //Turn off all LEDs
  digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, LOW);
  digitalWrite(FJ2_LED_TEST_PASS, LOW);
  digitalWrite(FJ2_LED_FAIL, LOW);

  failures = 0;
}

//Demonstrates how to modify the regulator voltage to mimic a battery getting up to
//a fully charged voltage level (4.2V). The charge LED should turn off.
void charge_led_blink()
{
  FJ2.setVoltageV2(3.7); // We need to set the voltage for V2 _before_ we call enableV2
  FJ2.enableV2();
  
  for (int i = 0 ; i < 3 ; i++)
  {
    FJ2.setVoltageV2(3.7); // charge led ON
    delay(500);
    FJ2.setVoltageV2(4.2); // charge led OFF
    delay(500);
  }
  
  FJ2.disableV2(); //For good measure, turn off this regulator
}
