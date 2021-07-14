/*
  This example shows how to run a normal test procedure

  Pete Lewis, started on 11/3/2016
  Contributions by NES November 15th, 2016
  Modified for FJ2 by Paul Clark, April 19th, 2021

  Select Mega2560 from the boards list
*/

// ************************************************************************************************
// ----- Includes -----

//The FJ2 library includes Wire.h for us. We don't need to include it again here

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2
//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino
FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

// ************************************************************************************************
// ----- Globals -----

// Loop Steps - these are used by the switch/case in the main loop
// This structure makes it easy to jump between any of the steps
typedef enum {
  step1,  // Step 1: check the FJ2 VCC voltage has been set to 3.3V
  getButton, // Wait for a cap button press
  step2,  // Step 2: test for a short on V1
  step3,  // Step 3: test for a short on V2
  step4,  // Step 4: power up V1 at 3.3V and check if it is OK
  step5,  // Step 5: toggle V2 between 
  step6,  // Step 6: enable the I2C buffer and do an I2C scan
  step7,  // Step 7: check the voltage on the interrupt pin
  pass,   // PASS
  fail    // FAIL
} loop_steps;

loop_steps loop_step = step1; // Make sure loop_step is set to step1

bool ProgramAndTestPressed; // Record which button was pressed to start the test (true = ProgramAndTest; false = Test)

int failures = 0; //Number of failures by the main test routine

// ************************************************************************************************
// ----- Define any extra FJ2 Pins which are connected to the board under test -----

int interrupt_pin = A1;  // The board interrupt pin - connected to A1 on the FJ2

// ************************************************************************************************
// ----- User Reset -----

// This function overwrites userReset in the FJ2 library
// We can use it to initialize the extra FJ2 pins used on our test jig
// It will be called automatically whenever we call FJ2.reset()
// Do not use Serial prints in userReset as Serial may not have been begun when FJ2.reset is called during the class instantiation
void FlyingJalapeno2::userReset(boolean resetLEDs) // YOU CAN IGNORE THE COMPILER WARNING: unused parameter 'resetLEDs'
{
  pinMode(interrupt_pin, INPUT); // Make the FJ2 pin conected to the board's interrupt pin an input
}

// ************************************************************************************************
// ----- setup -----

void setup()
{
  Serial.begin(115200);
  Serial.println("FJ2 full test example.");

  //FJ2.enableDebugging(); //Uncomment this line to enable helpful debug messages on Serial

  //FJ2.setCapSenseThreshold(1000); //Uncomment this line to set the cap sense threshold to 1000. Default is 2000

  //FJ2.setCapSenseSamples(20); //Uncomment this line to set the number of cap sense samples to 20. Default is 30

  //FJ2.setAnalogReadSamples(50); //Uncomment this line to set the number of analog reads for averaging. Default is 25

  FJ2.reset(); // Set up the FJ2 pins. Turn everything off - including the LEDs. This will call userReset too
}

// ************************************************************************************************
// ----- loop -----

void loop()
{
  // loop is one large switch/case that controls the sequencing of the code
  switch (loop_step) {

    // ************************************************************************************************
    // Step 1: check the FJ2 VCC voltage has been set to 3.3V
    case step1:
    {
      FJ2.reset(false); // Turn everything off except the LEDs

      delay(100);
      Serial.println();
      Serial.println(F("Step 1: Check VCC"));
      Serial.println();

      //Check VCC (by reading the 3.3V zener connected to A0 on the FJ2)
      if (!FJ2.testVCC())
      {
        Serial.println(F("Step 1: FAIL! VCC appears to be 5V! Please change VCC to 3.3V. Sending an SOS..."));
        FJ2.SOS(FJ2_LED_FAIL); // Blink SOS on the FJ2_LED_FAIL
        //The code will keep going around step1 until VCC is changed
      }
      else
      {
        Serial.println(F("Step 1: pass. VCC is OK"));
        Serial.println();
        Serial.println(F("getButton: waiting for a button press"));
        Serial.println();
        loop_step = getButton;
      }
    }
    break;

    // ************************************************************************************************
    // getButton: wait for a cap button press
    case getButton:
    {
      int button = 0;
      while (button == 0) // Keep checking the buttons until we get a valid press and release
      {
        button = FJ2.waitForButtonPressRelease();
      }
      
      if (button == 1) // Was the Program And Test Button pressed?
      {
        ProgramAndTestPressed = true; // Record that the Program And Test Button was pressed
        Serial.println(F("getButton: Program And Test selected"));
      }
      else // The Test Button was pressed
      {
        ProgramAndTestPressed = false; // Record that the Test Button was pressed
        Serial.println(F("getButton: Test selected"));
      }

      FJ2.dash(); // Blink the FJ2_STAT_LED
      loop_step = step2; // Move on
    }
    break;

    // ************************************************************************************************
    // Step 2: check for a short on V1
    case step2:
    {
      FJ2.reset(); // Turn everything off including the LEDs
      
      Serial.println();
      Serial.println(F("Step 2: Checking for a short on V1"));
      Serial.println();

      if (FJ2.isV1Shorted() == true)
      {
        Serial.println(F("Step 2:  FAIL! Short detected on V1. Check for shorts around <insert your debug message here>"));
        loop_step = fail;
      }
      else
      {
        loop_step = step3;
      }
    }
    break;

    // ************************************************************************************************
    // Step 3: check for a short on V2
    case step3:
    {
      Serial.println();
      Serial.println(F("Step 3: Checking for a short on V2"));
      Serial.println();

      if (FJ2.isV2Shorted() == true)
      {
        Serial.println(F("Step 3:  FAIL! Short detected on V2. Check for shorts around <insert your debug message here>"));
        loop_step = fail;
      }
      else
      {
        if (ProgramAndTestPressed == true)
          loop_step = step4; // Jump to this step if Program_And_Test was pressed
        else
          loop_step = step4; // Jump to this step if Test was pressed
      }
    }
    break;

    // ************************************************************************************************
    // Step 4: power up V1 at 3.3V and check if it is OK
    case step4:
    {
      Serial.println();
      Serial.println(F("Step 4: Power on V1"));
      Serial.println();

      FJ2.setVoltageV1(3.3); // Get ready to set V1 to 3.3V
      FJ2.enableV1(); // Enable V1

      //Note: due to the 10k/11k divider on the PT_READ pins, we can only verify voltages which are lower than VCC * 0.9
      //      The next line will fail if VCC is 3.3V and V1 is 5.0V
      if (FJ2.testVoltage(1) == false) // Test V1
      {
        Serial.println(F("Step 4:  FAIL! V1 is not 3.3V. Check for shorts around <insert your debug message here>"));
        loop_step = fail;
      }
      else
      {
        loop_step = step5; // Move on
      }
    }
    break;

    // ************************************************************************************************
    // Step 5: toggle V2 between 3.7V and 4.2V. This will make a battery charger LED flash
    case step5:
    {
      static int numStep5s = 0; // Keep track of how many times we have done step5 (making numStep5s static stops it being reset to zero each time)
      
      Serial.println();
      if (FJ2.getVoltageSettingV2() < 4.15) // If we have not yet called FJ2.setVoltage(2), getVoltageSettingV2() will return zero
      {
        Serial.println(F("Step 5: Setting V2 to 4.2V"));
        FJ2.setVoltageV2(4.2); // Get ready to set V2 to 4.2V
      }
      else
      {
        Serial.println(F("Step 5: Setting V2 to 3.7V"));
        FJ2.setVoltageV2(3.7); // Get ready to set V2 to 3.7V
      }
      Serial.println();

      FJ2.enableV2(); // Enable V2

      //Note: due to the 10k/11k divider on the PT_READ pins, we can only verify voltages which are lower than VCC * 0.9
      //      The next line would fail if VCC is 3.3V and V2 is 3.7V or higher...
      //if (FJ2.testVoltage(2) == false) // Test V2
      //{
      //  Serial.println(F("Step 5:  FAIL! V2 is not correct. Check for shorts around the battery charger chip"));
      //  loop_step = fail;
      //}
      //else
      {
        numStep5s++; // Increment the step count
        if (numStep5s >= 10) // Have we done step5 10 times?
        {
          numStep5s = 0; // Reset the number of Step5's now
          loop_step = step6; // Move on
        }
        else
        {
          delay(500); // Delay and repeat this step
        }
      }
    }
    break;

    // ************************************************************************************************
    // Step 6: enable the I2C buffer. Do an I2C scan
    case step6:
    {
      Serial.println();
      Serial.println(F("Step 6: Enabling the I2C buffer"));
      Serial.println();

      FJ2.enableI2CBuffer();

      Wire.begin(); // Begin the I2C bus

      delay(1000); // Give the I2C bus time to power up

      //Do a full scan of all I2C addresses to see what is connected
      //(If the address parameter is zero, verifyI2Cdevice scans all addresses from 1 to 126 inclusive)
      //verifyI2Cdevice will return true if a device was found
      if (FJ2.verifyI2Cdevice(0))
      {
        Serial.println("Step 6: I2C device found!");
        loop_step = step7; // Move on
      }
      else
      {
        Serial.println("Step 6: FAIL! I2C device not found!");
        loop_step = fail;
      }
    }
    break;

    // ************************************************************************************************
    // Step 7: check the voltage on the interrupt pin
    case step7:
    {
      Serial.println();
      Serial.println(F("Step 7: Checking the voltage on the interrupt pin"));
      Serial.println();

      //pin = pin to test
      //expectedVoltage = voltage we expect. 0.0 to 5.0 (float)
      //allowedPercent = allowed window for overage. 0 to 100 (int) (default 10%)
      boolean result = FJ2.verifyVoltage(interrupt_pin, 1.65, 10); // Voltage on the interrupt pin is 3.3V split by two 10Ks. Should be 1.65V. Check for 10%

      if (result)
      {
        Serial.println("Step 7: interrupt pin voltage is OK");
        loop_step = pass; // PASS
      }
      else
      {
        Serial.println("Step 7: FAIL! interrupt pin voltage is out of range");
        loop_step = fail;
      }
    }
    break;

    // ************************************************************************************************
    // pass - Tests complete!
    case pass:
    {
      Serial.println();
      Serial.println(F("*** Tests complete - PASS ***"));
      Serial.println();
      Serial.println();
      Serial.println();

      FJ2.reset(false); // Turn everything off except the LEDs
      Wire.end(); //Stop I2C
    
      if (ProgramAndTestPressed) // All tests passed. Turn the green PASS LED on
      {
        digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, HIGH); // Turn on the PROGRAM_AND_PASS LED
      }
      else
      {
        digitalWrite(FJ2_LED_TEST_PASS, HIGH); // Turn on the PASS LED
      }

      loop_step = step1; // Start over
    }
    break;

    // ************************************************************************************************
    // fail - Stop the test
    case fail:
    {
      Serial.println();
      Serial.println(F("*** TEST FAILED! ***")); // Do not use !!! - it crashes avrdude!
      Serial.println();

      failures++; // Increment the number of failures

      Serial.print(F("*** The number of failures so far is "));
      Serial.print(failures);
      Serial.println(F(" ***"));
      Serial.println();
      Serial.println();

      FJ2.reset(false); // Turn everything off except the LEDs
      Wire.end(); //Stop I2C
    
      digitalWrite(FJ2_LED_FAIL, HIGH); // Turn on the FAIL LED

      loop_step = step1; // Start over
    }
    break;
  }
}
