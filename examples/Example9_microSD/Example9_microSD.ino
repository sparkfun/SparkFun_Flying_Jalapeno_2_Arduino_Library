/*
  This example shows how to read data from a microSD card on the test jig

  Written by: Paul Clark, April 19th, 2021
  Based on earlier work by: Pete Lewis and NES

  Select Mega2560 from the boards list
*/

// ************************************************************************************************
// ----- Includes -----

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Jalapeno_2
//The FJ library depends on the CapSense library that can be obtained here: http://librarymanager/All#CapacitiveSensor_Arduino
FlyingJalapeno2 FJ2(FJ2_STAT_LED, 3.3); //Blink status msgs on STAT LED. Board should have VCC jumper set to 3.3V.

//The FJ2 library includes Wire.h for us. We don't need to include it again here

#include <SPI.h> // Needed for microSD

#include <SdFat.h> // Needed for microSD. Click here to get the latest library: http://librarymanager/All#sdFat_exFAT
#define SD_FAT_TYPE 1 // SD_FAT_TYPE = 0 for SdFat/File, 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_CONFIG SdSpiConfig(FJ2_MICROSD_CS, SHARED_SPI, SD_SCK_MHZ(2))
#if SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else // SD_FAT_TYPE == 0
SdFat sd;
File file;
#endif  // SD_FAT_TYPE

// ************************************************************************************************
// ----- Globals -----

// Loop Steps - these are used by the switch/case in the main loop
// This structure makes it easy to jump between any of the steps
typedef enum {
  step1,  // Step 1: check the FJ2 VCC voltage has been set to 3.3V
  getButton, // Wait for a cap button press
  step2,  // Step 2: test for a short on V1
  step3,  // Step 3: test for a short on V2
  step4,  // Step 4: enable the microSD buffers; check the size of a file on microSD; open it
  step5,  // Step 5: print the file contents
  pass,   // PASS
  fail    // FAIL
} loop_steps;

loop_steps loop_step = step1; // Make sure loop_step is set to step1

boolean ProgramAndTestPressed; // Record which button was pressed to start the test (true = ProgramAndTest; false = Test)

int failures = 0; //Number of failures by the main test routine

char fileName[] = "README.txt"; // This is the name of the file stored on the microSD card
unsigned long fileSize; //The size of the file
unsigned long filePosition = 0; //The file position currently being read

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
    // Step 4: enable the microSD buffers; check the size of a file on microSD; open it
    case step4:
    {
      Serial.println();
      Serial.println(F("Step 4: microSD. The microSD card should have been inserted and should have a file called "));
      Serial.print(fileName);
      Serial.println(F(" on it"));
      Serial.println();

      FJ2.enableMicroSDBuffer(); //Enable the microSD buffer by pulling FJ2_MICROSD_EN high
      delay(100);

      boolean sdStarted = sd.begin(SD_CONFIG); // Try to begin the SD card using the correct chip select
      
      boolean fileOpen = file.open(fileName, FILE_READ); // Try to open the file

      if ((!fileOpen) || (!sdStarted)) // Check if the file was opened successfully
      {
        Serial.print(F("Step 4:  FAIL! "));
        if (!fileOpen)
        {
          Serial.print(fileName);
          Serial.print(F(" could not be opened"));
          if (!sdStarted)
          {
            Serial.print(F(" and "));
          }
        }
        if (!sdStarted)
        {
          Serial.print(F("sd did not begin successfully"));
        }
        
        loop_step = fail;
      }
      else
      {
        fileSize = file.fileSize(); // Grab the size of the file
        Serial.print(F("Step 4: The size of the file is "));
        Serial.print(fileSize);
        Serial.println(F(" bytes"));
        filePosition = 0; //Make sure the file position is reset to zero
        file.rewind(); //Make sure we are pointing at the start of the file (should be redundant)
        
        loop_step = step5; // Move on
      }
    }
    break;

    // ************************************************************************************************
    // Step 5: read the file one byte at a time
    case step5:
    {
      uint8_t fileBuffer[16]; // Create a buffer to hold the file data as we read it from microSD
      
      Serial.print(F("Step 5: Reading from file position 0x"));
      if (filePosition < 0x1000) Serial.print(F("0")); // Pad the zeros
      if (filePosition < 0x100) Serial.print(F("0"));
      if (filePosition < 0x10) Serial.print(F("0"));
      Serial.print(filePosition, HEX);
      Serial.print(F(" : "));

      size_t bytesToRead;
      if (fileSize <= 16)
      {
        bytesToRead = fileSize;
      }
      else if (filePosition < (fileSize - 16)) // Calculate how many bytes to read from file
      {
        bytesToRead = 16;
      }
      else
      {
        bytesToRead = fileSize - filePosition; // Final read
      }

      size_t result = file.read(fileBuffer, bytesToRead); // Read the data from microSD
      if (result != bytesToRead) // Check the data was read correctly
      {
        Serial.println(F(" *** BAD READ ***"));
      }
      else
      {
        for (unsigned long thisByte = 0; thisByte < bytesToRead; thisByte++)
        {
          if (isPrintable(fileBuffer[thisByte])) // Print the data (if it is printable)
            Serial.write(fileBuffer[thisByte]);
        }
        Serial.println();
      }

      filePosition += bytesToRead; // Increment filePosition

      if (filePosition == fileSize) // Check if we have reached the end of the file
      {
        Serial.println();
        Serial.print(F("Step 5: File read is complete"));
        Serial.println();

        file.close(); // Close the file now we are done with it
        
        loop_step = pass; // We are done
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
      Serial.println(F("*** TEST FAILED! ***")); // Don't use !!! - it crashes avrdude
      Serial.println();

      failures++; // Increment the number of failures

      Serial.print(F("*** The number of failures so far is "));
      Serial.print(failures);
      Serial.println(F(" ***"));
      Serial.println();
      Serial.println();

      FJ2.reset(false); // Turn everything off except the LEDs
    
      digitalWrite(FJ2_LED_FAIL, HIGH); // Turn on the FAIL LED

      loop_step = step1; // Start over
    }
    break;
  }
}
