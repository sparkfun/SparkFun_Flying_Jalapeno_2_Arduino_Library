/*
  FlyingJalapeno.cpp - Library for SparkFun Qc's general testbed board, The Flying Jalapeno
  Created by Pete Lewis, June 29th, 2015
  FJ2 support added by Paul Clark, April 19th, 2021
  Released into the public domain.
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h"

#include <CapacitiveSensor.h> //Click here to get the library: http://librarymanager/All#CapacitiveSensor_Arduino

// ***** FJ2 Buttons *****

//Note: CapacitiveSensor::CapacitiveSensor configures the send pin as an output and pulls it low
CapacitiveSensor FJ2button1 = CapacitiveSensor(FJ2_CAP_SENSE_BUTTON_1, FJ2_CAP_SENSE_RETURN);
CapacitiveSensor FJ2button2 = CapacitiveSensor(FJ2_CAP_SENSE_BUTTON_2, FJ2_CAP_SENSE_RETURN);


// ***** The FJ2 Class *****


//Given a pin, use that pin to blink error messages
FlyingJalapeno2::FlyingJalapeno2(int statLED, float FJ_VCC)
{
  _statLED = statLED;
  _FJ_VCC = FJ_VCC;

  reset(); // Reset everything

  if (((FJ_VCC >= 3.29) && (FJ_VCC <= 3.31)) || ((FJ_VCC >= 4.99) && (FJ_VCC <= 5.01)))
  {
    // FJ_VCC is OK
  }
  else
  {
    // We can not use Serial prints here as Serial will not have been begun at this point
    //Serial.print("FlyingJalapeno2::FlyingJalapeno2: PANIC! FJ_VCC is not valid: ");
    //Serial.println(FJ_VCC, 2);
    //Instead, let's blink SOS on statLED
    SOS(statLED);
    SOS(statLED);
    SOS(statLED);
  }
}

//Reset the FJ2 to a safe state. Turn everything off.
//This function also calls userReset. userReset can be overwritten by the user.
//The user can add any board-specific reset functionality into their own userReset.
//E.g. setting other FJ2 pins back to their default state.
void FlyingJalapeno2::reset()
{

  // Turn all the LEDs off

  // Just in case _statLED is not one of the four regular LEDs
  // (It could be LED_BUILTIN on the FJ2 or a custom LED on the test jig)
  pinMode(_statLED, OUTPUT);
  digitalWrite(_statLED, LOW);

  pinMode(FJ2_LED_PROGRAM_AND_TEST_PASS, OUTPUT);
  pinMode(FJ2_LED_TEST_PASS, OUTPUT);
  pinMode(FJ2_LED_FAIL, OUTPUT);
  pinMode(FJ2_STAT_LED, OUTPUT);

  digitalWrite(FJ2_LED_PROGRAM_AND_TEST_PASS, LOW);
  digitalWrite(FJ2_LED_TEST_PASS, LOW);
  digitalWrite(FJ2_LED_FAIL, LOW);
  digitalWrite(FJ2_STAT_LED, LOW);

  // Disable the power

  disableV1(); // Make sure V1 and V2 are disabled
  disableV2();

  // Turn the V1 voltage control pins off
  digitalWrite(FJ2_V1_CONTROL_TO_3V3, LOW);
  digitalWrite(FJ2_V1_CONTROL_TO_5V0, LOW);
  pinMode(FJ2_V1_CONTROL_TO_3V3, INPUT);
  pinMode(FJ2_V1_CONTROL_TO_5V0, INPUT);

  // Turn the V2 voltage control pins off
  digitalWrite(FJ2_V2_CONTROL_TO_3V3, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_3V7, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_4V2, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_5V0, LOW);
  pinMode(FJ2_V2_CONTROL_TO_3V3, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_3V7, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_4V2, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_5V0, INPUT);

  //Configure the power test pins as inputs to begin with
  pinMode(FJ2_POWER_TEST_CONTROL, INPUT);
  pinMode(FJ2_PT_READ_V1, INPUT);
  pinMode(FJ2_PT_READ_V2, INPUT);

  //Prepare to deselect the SPI target (once the power is enabled)
  digitalWrite(FJ2_TARGET_CS, HIGH);
  pinMode(FJ2_TARGET_CS, INPUT);

  // Set up the optional pins
  pinMode(FJ2_BRAIN_VCC_A0, INPUT);
  digitalWrite(FJ2_I2C_EN, LOW); // Make sure the I2C buffer is disabled by pulling FJ2_I2C_EN low
  pinMode(FJ2_I2C_EN, OUTPUT);
  digitalWrite(FJ2_SERIAL_EN, LOW); // Make sure the Serial buffer is disabled by pulling FJ2_SERIAL_EN low
  pinMode(FJ2_SERIAL_EN, OUTPUT);
  digitalWrite(FJ2_SPI_EN, LOW); // Make sure the SPI buffer is disabled by pulling FJ2_SPI_EN low
  pinMode(FJ2_SPI_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_EN, LOW); // Make sure the microSD buffer is disabled by pulling FJ2_MICROSD_EN low
  pinMode(FJ2_MICROSD_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_CS, HIGH);
  pinMode(FJ2_MICROSD_CS, INPUT);

  // Call userReset - which can be overwritten by the user

  userReset(); // Do any board-specific resety stuff in userReset
}
void userReset() // Declared __attribute__((weak)) in the header file so the user can overwrite it
{
  // Do not use Serial prints here as Serial will not have been begun at this point
}

//Returns true if value is over threshold
//Threshold is optional. 5000 is default
boolean FlyingJalapeno2::isProgramAndTestPressed(long threshold)
{
  return(isPretestPressed(threshold));	
}
boolean FlyingJalapeno2::isButton1Pressed(long threshold)
{
  return(isPretestPressed(threshold));	
}
boolean FlyingJalapeno2::isPretestPressed(long threshold)
{
  long preTestButton = FJ2button1.capacitiveSensor(30);
  if(preTestButton > threshold) return(true);
  return(false);	
}

//Returns true if value is over threshold
//Threshold is optional. 5000 is default
boolean FlyingJalapeno2::isButton2Pressed(long threshold)
{
  return(isTestPressed(threshold));	
}
boolean FlyingJalapeno2::isTestPressed(long threshold)
{
  long testButton = FJ2button2.capacitiveSensor(30);
  if(testButton > threshold) return(true);
  return(false);
}

//Turn stat LED on
void FlyingJalapeno2::statOn()
{
  digitalWrite(_statLED, HIGH);
}	

//Turn stat LED on
void FlyingJalapeno2::statOff()
{
  digitalWrite(_statLED, LOW);
}	

//Blink SOS on chosen LED
//https://en.wikipedia.org/wiki/Morse_code
//The dot duration is the basic unit of time measurement in Morse code transmission.
//The duration of a dash is three times the duration of a dot.
//Each dot or dash within a character is followed by period of signal absence,
//called a space, equal to the dot duration.
//The letters of a word are separated by a space of duration equal to three dots,
//and the words are separated by a space equal to seven dots.
void FlyingJalapeno2::SOS(int pin)
{
  dot(pin);
  dot(pin);
  dot(pin);
  dash(pin);
  dash(pin);
  dash(pin);
  dot(pin);
  dot(pin);
  dot(pin);
  delay(1750);
}

void FlyingJalapeno2::dot(int pin)
{
  if (pin == -1) pin = _statLED;
  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
  delay(250);
}

void FlyingJalapeno2::dash(int pin)
{
  if (pin == -1) pin = _statLED;
  digitalWrite(pin, HIGH);
  delay(750);
  digitalWrite(pin, LOW);
  delay(250);
}

// GENERIC PRE-TEST for shorts to GND on power rails, returns true if all is good, returns false if a short is detected
boolean FlyingJalapeno2::PreTest_Custom(byte control_pin, byte read_pin)
{
  pinMode(control_pin, OUTPUT);
  pinMode(read_pin, INPUT);

  digitalWrite(control_pin, HIGH);
  delay(200);
  int reading = analogRead(read_pin);

  Serial.print("FlyingJalapeno2::PreTest_Custom: jumper test reading: ");
  Serial.println(reading);

  digitalWrite(control_pin, LOW);
  pinMode(control_pin, INPUT);

  float jumper_val = 486;

  if ((((float)reading) < (jumper_val * 1.03)) && (((float)reading) > (jumper_val * 0.97))) return false; // jumper detected!!
  else return true;
}

// GENERIC PRE-TEST for shorts to GND on power rails, returns FALSE if all is good, returns TRUE if there is short detected
boolean FlyingJalapeno2::isShortToGround_Custom(byte control_pin, byte read_pin, boolean debug)
{
  pinMode(control_pin, OUTPUT);
  pinMode(read_pin, INPUT);

  digitalWrite(control_pin, HIGH);
  delay(200);
  int reading = analogRead(read_pin);

  if(debug) Serial.print("FlyingJalapeno2::isShortToGround_Custom: jumper test reading:");
  if(debug) Serial.println(reading);

  digitalWrite(control_pin, LOW);
  pinMode(control_pin, INPUT);

  float jumper_val = 486;

  if ((((float)reading) < (jumper_val * 1.03)) && (((float)reading) > (jumper_val * 0.97)))
	{
		if (!debug) Serial.print("FlyingJalapeno2::isShortToGround_Custom: jumper test reading:"); // check debug, to avoid double printing
		if (!debug) Serial.println(reading);
		return true; // jumper detected!!
	}
  else return false;
}

//Test power circuit to see if there is a short on the target
//Returns true if there is a short
boolean FlyingJalapeno2::isV1Shorted()
{
  return (!powerTest(1)); // Test V1
}

boolean FlyingJalapeno2::isV2Shorted()
{
  return (!powerTest(2)); // Test V2
}

//PRIVATE: Test target board for shorts to GND
//Called by isV1Shorted() and isV2Shorted()
//Returns true if all is good, returns false if there is short detected
boolean FlyingJalapeno2::powerTest(byte select) // select is either "1" or "2"
{
  //Power down regulators
  disableV1();
  disableV2();

  //Specify the read_pin
  byte read_pin;
  if (select == 1) read_pin = FJ2_PT_READ_V1;
  else if (select == 2) read_pin = FJ2_PT_READ_V2;
  else
  {
    Serial.println("FlyingJalapeno2::powerTest: Error! select must be 1 or 2.");
    return (false);
  }

  //Now setup the control pin
  pinMode(FJ2_POWER_TEST_CONTROL, OUTPUT);
  digitalWrite(FJ2_POWER_TEST_CONTROL, HIGH);

  pinMode(read_pin, INPUT);

  delay(200); //Wait for voltage to settle before taking a ADC reading

  int reading = analogRead(read_pin);

  //Serial.print("FlyingJalapeno2::powerTest: power test reading (should >500 or <471): ");
  //Serial.println(reading);

  //Release the control pin
  digitalWrite(FJ2_POWER_TEST_CONTROL, LOW);
  pinMode(FJ2_POWER_TEST_CONTROL, INPUT);

  //FJ2_POWER_TEST_CONTROL feeds the FJ2_PT_READ via a diode and a 100K/110K resistor divider
  //There is a 10K resistor between FJ2_PT_READ and V1/V2
  //The diode is a BAS16J which has a forward voltage of ~0.5V at 0.1mA at 25C
  //The ADC is 10-bit and has a full-range of 1023
  //
  //If there is no load on V1/V2 and VCC is 3.3V then we expect to see:
  //90.9% of ((3.3V - 0.5V) / 3.3V) * 1023 = 789
  //
  //If there is no load on V1/V2 and VCC is 5.0V then we expect to see:
  //90.9% of ((5.0V - 0.5V) / 5.0V) * 1023 = 837
  //
  //If there is a dead short on V1/V2 and VCC is 3.3V then that changes the resistor divider dramatically:
  //10K in parallel with 100K is 9.09K
  //The divider becomes: 9.09K / 19.09K = 47.6%
  //
  //If VCC is 3.3V then we expect to see:
  //47.6% of ((3.3V - 0.5V) / 3.3V) * 1023 = 413
  //
  //If VCC is 5.0V then we expect to see:
  //47.6% of ((5.0V - 0.5V) / 5.0V) * 1023 = 438

  int jumper_val;

  if ((_FJ_VCC >= 3.29) && (_FJ_VCC <= 3.31)) // Is VCC supposed to be 3.3V?
  {
    jumper_val = 710; // 789 * 90%
  }
  else
  {
    jumper_val = 753; // 837 * 90%
  }

  if (reading < jumper_val)
    return false; // jumper detected!!
  return true;
}

//Test a pin to see what voltage is on the pin.
//Returns true if pin voltage is within a given window of the value we are looking for
//pin = pin to test
//expectedVoltage = voltage we expect. 0.0 to 5.0 (float)
//allowedPercent = allowed window for overage. 0 to 100 (int) (default 10%)
//debug = print debug statements (default false)
boolean FlyingJalapeno2::verifyVoltage(int pin, float expectedVoltage, int allowedPercent, boolean debug)
{
  //float allowanceFraction = map(allowedPercent, 0, 100, 0, 1.0); //Scale int to a fraction of 1.0
  //Grrrr! map doesn't work with floats at all

  float allowanceFraction = allowedPercent / 100.0; //Scale the allowedPercent to a float

  pinMode(pin, INPUT); //Make sure pin is an input

  delay(200); //Wait for voltage to settle before taking a ADC reading

  int reading = analogRead(pin);

  //Convert reading to voltage
  float readVoltage = _FJ_VCC / 1024 * reading;

  if (debug)
  {
    Serial.print("FlyingJalapeno2::verifyVoltage: allowanceFraction: ");
    Serial.println(allowanceFraction);

    Serial.print("FlyingJalapeno2::verifyVoltage: reading: ");
    Serial.println(reading);

    Serial.print("FlyingJalapeno2::verifyVoltage: voltage: ");
    Serial.println(readVoltage, 2);
  }

  if ((readVoltage <= (expectedVoltage * (1.0 + allowanceFraction))) && (readVoltage >= (expectedVoltage * (1.0 - allowanceFraction))))
    return true; // good value
  return false;
}

boolean FlyingJalapeno2::verifyValue(float input_value, float correct_val, float allowance_percent)
{
  float allowanceFraction = allowance_percent / 100.0; //Scale the allowedPercent to a float

  if ((input_value <= (correct_val * (1.0 + allowanceFraction))) && (input_value >= (correct_val * (1.0 - allowanceFraction))))
    return true; // good value
  return false;
}

//Enable or disable regulator #1
void FlyingJalapeno2::enableV1(void)
{
  if (_V1_setting == 0.0) // Check if setVoltageV1 has been called
  {
    Serial.print("FlyingJalapeno2::enableV1: setVoltageV1 has not been called. Aborting...");
    return;
  }

  digitalWrite(FJ2_V1_POWER_CONTROL, HIGH); // turn on the high side switch
  pinMode(FJ2_V1_POWER_CONTROL, OUTPUT);
  _V1_actual = _V1_setting;
}

void FlyingJalapeno2::disableV1(void)
{
  //Do not do Serial prints here as disableV1 is called when the class is instantiated - before Serial is begun
  digitalWrite(FJ2_V1_POWER_CONTROL, LOW); // turn off the high side switch
  pinMode(FJ2_V1_POWER_CONTROL, OUTPUT);
  _V1_actual = 0.0;
}

//Enable or disable regulator #2
void FlyingJalapeno2::enableV2(void)
{
  if (_V2_setting == 0.0) // Check if setVoltageV2 has been called
  {
    Serial.print("FlyingJalapeno2::enableV2: setVoltageV2 has not been called. Aborting...");
    return;
  }

  digitalWrite(FJ2_V2_POWER_CONTROL, HIGH); // turn on the high side switch
  pinMode(FJ2_V2_POWER_CONTROL, OUTPUT);
  _V2_actual = _V2_setting;
}

void FlyingJalapeno2::disableV2(void)
{
  //Do not do Serial prints here as disableV2 is called when the class is instantiated - before Serial is begun
  digitalWrite(FJ2_V2_POWER_CONTROL, LOW); // turn off the high side switch
  pinMode(FJ2_V2_POWER_CONTROL, OUTPUT);
  _V2_actual = 0.0;
}

//Setup the first power supply to the chosen voltage level
//Leaves MOSFET off so regulator is configured but not connected to target
void FlyingJalapeno2::setVoltageV1(float voltage)
{
  // Turn the V1 voltage control pins off
  digitalWrite(FJ2_V1_CONTROL_TO_3V3, LOW);
  digitalWrite(FJ2_V1_CONTROL_TO_5V0, LOW);
  pinMode(FJ2_V1_CONTROL_TO_3V3, INPUT);
  pinMode(FJ2_V1_CONTROL_TO_5V0, INPUT);

  if ((voltage >= 3.25) && (voltage <= 3.35))
  {
    pinMode(FJ2_V1_CONTROL_TO_3V3, OUTPUT);
    digitalWrite(FJ2_V1_CONTROL_TO_3V3, LOW);
    _V1_setting = 3.3;
  }
  else if ((voltage >= 4.95) && (voltage <= 5.05))
  {
    pinMode(FJ2_V1_CONTROL_TO_5V0, OUTPUT);
    digitalWrite(FJ2_V1_CONTROL_TO_5V0, LOW);
    _V1_setting = 5.0;
  }
  else
  {
    Serial.print("FlyingJalapeno2::setVoltageV1: invalid voltage specified: ");
    Serial.print(voltage, 2);
    Serial.println(". Defaulting to 3.3V");
    pinMode(FJ2_V1_CONTROL_TO_3V3, OUTPUT); // default to 3.3V - even when the high side switch is turn off.
    digitalWrite(FJ2_V1_CONTROL_TO_3V3, LOW);
    _V1_setting = 3.3;
  }
}

//Setup the second power supply to the chosen voltage level
//Leaves MOSFET off so regulator is configured but not connected to target
void FlyingJalapeno2::setVoltageV2(float voltage)
{
  // Turn the V2 voltage control pins off
  digitalWrite(FJ2_V2_CONTROL_TO_3V3, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_3V7, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_4V2, LOW);
  digitalWrite(FJ2_V2_CONTROL_TO_5V0, LOW);
  pinMode(FJ2_V2_CONTROL_TO_3V3, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_3V7, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_4V2, INPUT);
  pinMode(FJ2_V2_CONTROL_TO_5V0, INPUT);

  if ((voltage >= 3.25) && (voltage <= 3.35))
  {
    pinMode(FJ2_V2_CONTROL_TO_3V3, OUTPUT);
    digitalWrite(FJ2_V2_CONTROL_TO_3V3, LOW);
    _V2_setting = 3.3;
  }
  else if ((voltage >= 3.65) && (voltage <= 3.75))
  {
    pinMode(FJ2_V2_CONTROL_TO_3V7, OUTPUT);
    digitalWrite(FJ2_V2_CONTROL_TO_3V7, LOW);
    _V2_setting = 3.7;
  }
  else if ((voltage >= 4.15) && (voltage <= 4.25))
  {
    pinMode(FJ2_V2_CONTROL_TO_4V2, OUTPUT);
    digitalWrite(FJ2_V2_CONTROL_TO_4V2, LOW);
    _V2_setting = 4.2;
  }
  else if ((voltage >= 4.95) && (voltage <= 5.05))
  {
    pinMode(FJ2_V2_CONTROL_TO_5V0, OUTPUT);
    digitalWrite(FJ2_V2_CONTROL_TO_5V0, LOW);
    _V2_setting = 5.0;
  }
  else
  {
    Serial.print("FlyingJalapeno2::setVoltageV2: invalid voltage specified: ");
    Serial.print(voltage, 2);
    Serial.println(". Defaulting to 3.3V");
    pinMode(FJ2_V2_CONTROL_TO_3V3, OUTPUT); // default to 3.3V
    digitalWrite(FJ2_V2_CONTROL_TO_3V3, LOW);
    _V2_setting = 3.3;
  }
}

//Test if the voltage on V1/V2 is OK. Returns false if the voltage is out of range
boolean FlyingJalapeno2::testVoltage(byte select, boolean debug) // select is either "1" or "2"
{
  //Specify the read_pin and expected voltage
  byte read_pin;
  float expectedVoltage;
  if (select == 1)
  {
    read_pin = FJ2_PT_READ_V1;
    expectedVoltage = _V1_actual * 10.0 / 11.0; // Compensate for resistor divider
  }
  else if (select == 2)
  {
    read_pin = FJ2_PT_READ_V2;
    expectedVoltage = _V2_actual * 10.0 / 11.0; // Compensate for resistor divider
  }
  else
  {
    Serial.println("FlyingJalapeno2::testVoltage: Error! select must be 1 or 2.");
    return (false);
  }

  //Verify the voltage is within 5%
  return (verifyVoltage(read_pin, expectedVoltage, 5, debug));
}

//Test if the FJ2 VCC has been set correctly (using the 3.3V Zener diode on FJ2_BRAIN_VCC_A0)
//Return true if FJ2_BRAIN_VCC_A0 matches _FJ_VCC
boolean FlyingJalapeno2::testVCC()
{
  //Check VCC by reading the 3.3V zener connected to A0
  //If VCC is 3.3V, the signal on A0 will be full range
  //If VCC is 5V, the signal on A0 will be (roughly) 3.3V/5V * 1023 = 675

  int val = analogRead(FJ2_BRAIN_VCC_A0);

  if ((_FJ_VCC >= 3.29) && (_FJ_VCC <= 3.31)) // Is VCC supposed to be 3.3V?
  {
    // val should be max'd out at 1023. Return false if it isn't (i.e. VCC is higher than 3.3V!)
    if (val < 950) 
      return false;
    return true;
  }

  // else: _FJ_VCC must be 5.0V so check diode voltage reads as 3.3V
  return (verifyVoltage(FJ2_BRAIN_VCC_A0, 3.3, 10, true));
}

//Enable the I2C buffer by pulling FJ2_I2C_EN high
void FlyingJalapeno2::enableI2CBuffer()
{
  pinMode(FJ2_I2C_EN, OUTPUT); // Enable the I2C buffer by pulling FJ2_I2C_EN high
  digitalWrite(FJ2_I2C_EN, HIGH);
}
//Disable the I2C buffer by pulling FJ2_I2C_EN low
void FlyingJalapeno2::disableI2CBuffer()
{
  digitalWrite(FJ2_I2C_EN, LOW); // Make sure the I2C buffer is disabled by pulling FJ2_I2C_EN low
  pinMode(FJ2_I2C_EN, OUTPUT);
}

//Enable the Serial buffer by pulling FJ2_SERIAL_EN high
void FlyingJalapeno2::enableSerialBuffer()
{
  pinMode(FJ2_SERIAL_EN, OUTPUT); // Enable the Serial buffer by pulling FJ2_SERIAL_EN high
  digitalWrite(FJ2_SERIAL_EN, HIGH);
}
//Disable the Serial buffer by pulling FJ2_SERIAL_EN low
void FlyingJalapeno2::disableSerialBuffer()
{
  digitalWrite(FJ2_SERIAL_EN, LOW); // Make sure the Serial buffer is disabled by pulling FJ2_SERIAL_EN low
  pinMode(FJ2_SERIAL_EN, OUTPUT);
}

//Enable the SPI buffer by pulling FJ2_SPI_EN high
void FlyingJalapeno2::enableSPIBuffer()
{
  pinMode(FJ2_TARGET_CS, OUTPUT); //Deselect the SPI target
  digitalWrite(FJ2_TARGET_CS, HIGH);
  pinMode(FJ2_SPI_EN, OUTPUT); // Enable the SPI buffer is disabled by pulling FJ2_SPI_EN high
  digitalWrite(FJ2_SPI_EN, HIGH);
}
//Disable the SPI buffer by pulling FJ2_SPI_EN low
void FlyingJalapeno2::disableSPIBuffer()
{
  digitalWrite(FJ2_SPI_EN, LOW); // Make sure the SPI buffer is disabled by pulling FJ2_SPI_EN low
  pinMode(FJ2_SPI_EN, OUTPUT);
  digitalWrite(FJ2_TARGET_CS, HIGH); //Prepare to deselect the SPI target (once the power is enabled)
  pinMode(FJ2_TARGET_CS, INPUT);
}

//Enable the microSD buffer by pulling FJ2_MICROSD_EN high
void FlyingJalapeno2::enableMicroSDBuffer()
{
  pinMode(FJ2_MICROSD_CS, OUTPUT); // Deselect the microSD card
  digitalWrite(FJ2_MICROSD_CS, HIGH);
  pinMode(FJ2_MICROSD_EN, OUTPUT); // Pull FJ2_MICROSD_EN high
  digitalWrite(FJ2_MICROSD_EN, HIGH);
}
//Disable the microSD buffer by pulling FJ2_MICROSD_EN low
void FlyingJalapeno2::disableMicroSDBuffer()
{
  digitalWrite(FJ2_MICROSD_EN, LOW); // Make sure the microSD buffer is disabled by pulling FJ2_MICROSD_EN low
  pinMode(FJ2_MICROSD_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_CS, HIGH);
  pinMode(FJ2_MICROSD_CS, INPUT);
}

//Verify the address of an I2C device
//If address is zero, do a full scan
//Return true if the specified address pings correctly
boolean FlyingJalapeno2::verifyI2Cdevice(byte address, boolean debug)
{
  byte error;
  boolean result = false;

  for (int device = 1; device < 127; device++) // Step through all devices
  {
    if ((address == 0) || (device == address)) // Check if we shoudl ping this one
    {
      if (debug)
      {
        Serial.print("FlyingJalapeno2::verifyI2Cdevice: Pinging address 0x");
        if (device < 16) Serial.print("0");
        Serial.print(device, HEX);
      }

      Wire.beginTransmission(device); // Ping this device
      error = Wire.endTransmission();

      if (error == 0)
      {
        if (debug)
        {
          Serial.println("... Found!");
        }
        else
        {
          Serial.print("FlyingJalapeno2::verifyI2Cdevice: Device found at address 0x");
          if (device < 16) Serial.print("0");
          Serial.println(device, HEX);
        }
        result = true;
      }
      else if (error == 4)
      {
        if (debug)
        {
          Serial.println("... Unknown error!");
        }
      }
    }
  }

  return (result);
}

