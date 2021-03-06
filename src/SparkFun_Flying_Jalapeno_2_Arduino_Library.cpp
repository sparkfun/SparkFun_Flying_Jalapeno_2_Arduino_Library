/*
  FlyingJalapeno.cpp - Library for SparkFun Qc's general testbed board, The Flying Jalapeno
  Created by Pete Lewis, June 29th, 2015
  FJ2 support added by Paul Clark, April 19th, 2021
  Released into the public domain.
*/

#include "SparkFun_Flying_Jalapeno_2_Arduino_Library.h"

// ***** The FJ2 Class *****


//Given a pin, use that pin to blink error messages
FlyingJalapeno2::FlyingJalapeno2(int statLED, float FJ_VCC, bool useCapSense)
{
  _statLED = statLED;
  _FJ_VCC = FJ_VCC;
  _useCapSense = useCapSense;

  // ***** FJ2 Buttons *****
  //CapacitiveSensor(byte sendPin, byte receivePin)
  //The receive pin is the one connected directly to the touch pad
  //The send pin is connected to the pad via the large resistor
  //So on FJ2, the CS_RETURN pin is actually the send pin
  //Note: CapacitiveSensor::CapacitiveSensor configures the send pin as an output and pulls it low
  if (_useCapSense)
  {
    FJ2button1 = new CapacitiveSensor(FJ2_CAP_SENSE_RETURN, FJ2_CAP_SENSE_BUTTON_1);
    FJ2button2 = new CapacitiveSensor(FJ2_CAP_SENSE_RETURN, FJ2_CAP_SENSE_BUTTON_2);
  }

  reset(); // Reset everything

  if (((FJ_VCC >= 3.29) && (FJ_VCC <= 3.31)) || ((FJ_VCC >= 4.99) && (FJ_VCC <= 5.01)))
  {
    // FJ_VCC is OK
  }
  else
  {
    // We can not use Serial prints here as Serial will not have been begun at this point
    //Instead, let's blink SOS on statLED
    SOS(statLED);
    SOS(statLED);
    SOS(statLED);
  }
}

void FlyingJalapeno2::enableDebugging(Stream &debugPort)
{
  _debugSerial = &debugPort; //Grab which port the user wants us to use for debugging
  _printDebug = true; //Should we print the commands we send? Good for debugging
}
void FlyingJalapeno2::disableDebugging()
{
  _printDebug = false; //Turn off extra print statements
}

//Reset the FJ2 to a safe state. Turn everything off (except the LEDs if desired).
//This function also calls userReset. userReset can be overwritten by the user.
//The user can add any board-specific reset functionality into their own userReset.
//E.g. setting other FJ2 pins back to their default state.
//
//The only pins we don't touch in here are the SPI pins. Very bad things happen to SdFat in particular if you
//change FJ2_TARGET_CS to an INPUT after the microSD has been begun (even though the microSD uses FJ2_MICROSD_CS).
//There is a cryptic note about this in the Arduino SD documentation: https://www.arduino.cc/en/reference/SD
//"Note that even if you don't use the hardware SS pin, it must be left as an output or the SD library won't work."
void FlyingJalapeno2::reset(boolean resetLEDs)
{

  // Turn all the LEDs off - if resetLEDs is true
  if (resetLEDs)
  {
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
  }

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

  //We do not need to worry about the SPI pins providing parasitic power to the board under test
  //The SPI buffer prevents that as soon as FJ2_SPI_EN is low

  // Set up the optional pins
  pinMode(FJ2_BRAIN_VCC_A0, INPUT);
  digitalWrite(FJ2_I2C_EN, LOW); // Make sure the I2C buffer is disabled by pulling FJ2_I2C_EN low
  pinMode(FJ2_I2C_EN, OUTPUT);
  digitalWrite(FJ2_SERIAL_EN, LOW); // Make sure the Serial buffer is disabled by pulling FJ2_SERIAL_EN low
  pinMode(FJ2_SERIAL_EN, OUTPUT);
  digitalWrite(FJ2_SPI_EN, LOW); // Make sure the SPI buffer is disabled by pulling FJ2_SPI_EN low
  pinMode(FJ2_SPI_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_PWR_EN, LOW); // Make sure the microSD power is disabled by pulling FJ2_MICROSD_PWR_EN low
  pinMode(FJ2_MICROSD_PWR_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_EN, LOW); // Make sure the microSD buffer is disabled by pulling FJ2_MICROSD_EN low
  pinMode(FJ2_MICROSD_EN, OUTPUT);
  digitalWrite(FJ2_MICROSD_CS, HIGH); // Get ready to deselect the microSD card
  pinMode(FJ2_MICROSD_CS, INPUT);

  // If _useCapSense is false, configure the cap sense pins as inputs
  // (Don't use INPUT_PULLUP or you'll see the 15us HeatBeat pulses)
  // (Pull CAP_SENSE_RETURN low to avoid it acting as a pull-up)
  if (!_useCapSense)
  {
    pinMode(FJ2_CAP_SENSE_BUTTON_1, INPUT);
    pinMode(FJ2_CAP_SENSE_BUTTON_2, INPUT);
    pinMode(FJ2_CAP_SENSE_RETURN, OUTPUT);
    digitalWrite(FJ2_CAP_SENSE_RETURN, LOW);
  }

  // Call userReset - which can be overwritten by the user

  userReset(resetLEDs); // Do any board-specific resety stuff in userReset
}
void FlyingJalapeno2::userReset(boolean resetLEDs) // Declared __attribute__((weak)) in the header file so the user can overwrite it. YOU CAN IGNORE THE COMPILER WARNING: unused parameter 'resetLEDs'
{
  // Do not use Serial prints here as Serial will not have been begun at this point

  // You will see a compiler warning saying resetLEDs is unused... Just roll with it...
}

//Allow the user to override the default cap sense threshold
void FlyingJalapeno2::setCapSenseThreshold(long threshold)
{
  if (threshold > 0) // threshold must be greater than zero. Typical value is 5000
    _capSenseThreshold = threshold;
}

//Allow the user to override the default cap sense samples
void FlyingJalapeno2::setCapSenseSamples(uint8_t samples)
{
  if (samples > 0) // samples must be greater than zero. Typical value is 30
    _capSenseSamples = samples;
}

//Returns true if value is over threshold
//Threshold is optional. _capSenseThreshold will be used if threshold is not provided (zero)
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
  if (_useCapSense)
  {
    long preTestButton = FJ2button1->capacitiveSensor(_capSenseSamples);
    if ((_printDebug == true) && (preTestButton < 0))
    {
      _debugSerial->print(F("FlyingJalapeno2::isPretestPressed: FJ2button1.capacitiveSensor returned "));
      _debugSerial->println(preTestButton);
    }
    if (threshold == 0) threshold = _capSenseThreshold;
    if (preTestButton > threshold)
      return(true);
    return(false);
  }
  else
  {
    // Check that the button signal is high for > 15us (just in case the AT42QT1011 HeartBeat is detected)
    // Take six samples five microseconds apart. Return true if all six are high
    int counter = 0;
    for (int c = 0; c < 6; c++)
    {
      if (digitalRead(FJ2_CAP_SENSE_BUTTON_1) == HIGH)
        counter++;
      delayMicroseconds(5);
    }
    return (counter == 6);
  }
}

//Returns true if value is over threshold
//Threshold is optional. _capSenseThreshold will be used if threshold is not provided (zero)
boolean FlyingJalapeno2::isButton2Pressed(long threshold)
{
  return(isTestPressed(threshold));	
}
boolean FlyingJalapeno2::isTestPressed(long threshold)
{
  if (_useCapSense)
  {
    long preTestButton = FJ2button2->capacitiveSensor(_capSenseSamples);
    if ((_printDebug == true) && (preTestButton < 0))
    {
      _debugSerial->print(F("FlyingJalapeno2::isPretestPressed: FJ2button2.capacitiveSensor returned "));
      _debugSerial->println(preTestButton);
    }
    if (threshold == 0) threshold = _capSenseThreshold;
    if (preTestButton > threshold)
      return(true);
    return(false);
  }
  else
  {
    // Check that the button signal is high for > 15us (just in case the AT42QT1011 HeartBeat is detected)
    // Take six samples five microseconds apart. Return true if all six are high
    int counter = 0;
    for (int c = 0; c < 6; c++)
    {
      if (digitalRead(FJ2_CAP_SENSE_BUTTON_2) == HIGH)
        counter++;
      delayMicroseconds(5);
    }
    return (counter == 6);
  }
}

//Blocking wait-for-a-button-press functions
//These functions return:
//  0 if no button was pressed (and the function timed out)
//  1 if button 1 (PROGRAM_AND_TEST) was pressed
//  2 if button 2 (TEST) was pressed
//minimumHoldMillis acts as a debounce. The button must be held for at least this many millis to register as a press
//timeoutMillis defines the timeout for the function. The function will return zero after this many millis if the button was not pressed
//waitForButtonPress will return 1 or 2 if the button is held for at least minimumHoldMillis. 1 takes priority over 2 (if both are being pressed)
//waitForButtonPressRelease will return 1 or 2 after the button has been pressed and released for minimumReleaseMillis
//waitForButtonReleasePressRelease will only return 1 or 2 if neither button was pressed initially (when the function was called)
int FlyingJalapeno2::waitForButtonPress(unsigned long timeoutMillis, unsigned long minimumHoldMillis, unsigned long overrideStartMillis)
{
  unsigned long startMillis; // Record millis when the function was called
  if (overrideStartMillis > 0)
  {
    startMillis = overrideStartMillis;
  }
  else
  {
    startMillis = millis();
  }
  boolean keepGoing = true; // keepGoing if true
  boolean timedOut = false; // Indicate if we timed out
  int result = 0; // Return: 0 = no button; 1 = button 1; 2 = button 2
  unsigned long latestButtonPress = 0; // Record the time of the latest button press

  while (keepGoing)
  {
    if (result == 0) //If we have not yet recorded a button press
    {
      if (isButton1Pressed()) // Check if button 1 is pressed. 1 takes priority over 2
      {
        latestButtonPress = millis(); // Record the time of the latest button press
        result = 1; // Indicate button 1 is being pressed
      }
      else if (isButton2Pressed()) // Check if button 2 is pressed
      {
        latestButtonPress = millis(); // Record the time of the latest button press
        result = 2; // Indicate button 2 is being pressed
      }
      else
      {
        // Neither button is pressed
      }
    }
    else if (result == 1) // Button 1 has been pressed. Check if it is still being pressed
    {
      if (isButton1Pressed()) // Is button 1 still being pressed?
      {
        // Button is still being pressed so check if it has been held for minimumHoldMillis
        if (millis() > (latestButtonPress + minimumHoldMillis))
        {
          keepGoing = false; // Button has been held for long enough. Time to leave the loop
        }
      }
      else
      {
        // Button 1 has been released so reset result back to zero and go back to looking for a fresh press
        result = 0;
      }
    }
    else // if (result == 2) // Button 2 has been pressed. Check if it is still being pressed
    {
      if (isButton2Pressed()) // Is button 2 still being pressed?
      {
        // Button is still being pressed so check if it has been held for minimumHoldMillis
        if (millis() > (latestButtonPress + minimumHoldMillis))
        {
          keepGoing = false; // Button has been held for long enough. Time to leave the loop
        }
      }
      else
      {
        // Button 2 has been released so reset result back to zero and go back to looking for a fresh press
        result = 0;
      }
    }

    // Check for a timeout
    // Check if millis is greater than timeoutMillis plus minimumHoldMillis
    //   just in case minimumHoldMillis is > timeoutMillis
    if (millis() > (startMillis + timeoutMillis + minimumHoldMillis))
    {
      if (_printDebug == true)
      {
        _debugSerial->println(F("FlyingJalapeno2::waitForButtonPress: timed out!"));
      }
      keepGoing = false; // Timeout. Time to leave the loop
      timedOut = true;
    }
  }

  // keepGoing is false
  // If timedOut is true, return zero
  if (timedOut)
    return (0);
  
  // timedOut is false, so we must have recorded a valid button press

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::waitForButtonPress: button "));
    _debugSerial->print(result);
    _debugSerial->println(F(" pressed"));
  }

  return (result);
}

int FlyingJalapeno2::waitForButtonPressRelease(unsigned long timeoutMillis, unsigned long minimumHoldMillis, unsigned long minimumReleaseMillis, unsigned long overrideStartMillis)
{
  unsigned long startMillis; // Record millis when the function was called
  if (overrideStartMillis > 0)
  {
    startMillis = overrideStartMillis;
  }
  else
  {
    startMillis = millis();
  }
  boolean keepGoing = true; // keepGoing if true
  boolean timedOut = false; // Indicate if we timed out
  unsigned long latestButtonRelease = 0; // Record the time of the latest button release

  if (_printDebug == true)
  {
    _debugSerial->println(F("FlyingJalapeno2::waitForButtonPressRelease: calling waitForButtonPress"));
  }

  //Begin by checking for a valid button press
  int result = waitForButtonPress(timeoutMillis, minimumHoldMillis, startMillis);

  //If no button press was recorded, return zero now
  if (result == 0)
    return (0);
  
  //A valid press was recorded on button 1 or button 2
  //Now check that the button is released
  while (keepGoing)
  {
    if (result == 1) // Button 1 was pressed. Check if it is still being pressed
    {
      if (isButton1Pressed()) // Is button 1 still being pressed?
      {
        // Button is still being pressed
        latestButtonRelease = 0;
      }
      else
      {
        // Button 1 has been released

        // Check if this is a fresh release (latestButtonRelease == 0)
        if (latestButtonRelease == 0)
        {
          latestButtonRelease = millis(); // Record the time of the release
        }

        else if (millis() > (latestButtonRelease + minimumReleaseMillis))
        {
          if (_printDebug == true)
          {
            _debugSerial->println(F("FlyingJalapeno2::waitForButtonPressRelease: button 1 has been released"));
          }
          keepGoing = false; // Button has been released for long enough. Time to leave the loop
        }
      }
    }
    else // if (result == 2) // Button 2 was pressed. Check if it is still being pressed
    {
      if (isButton2Pressed()) // Is button 2 still being pressed?
      {
        // Button is still being pressed
        latestButtonRelease = 0;
      }
      else
      {
        // Button 2 has been released

        // Check if this is a fresh release (latestButtonRelease == 0)
        if (latestButtonRelease == 0)
        {
          latestButtonRelease = millis(); // Record the time of the release
        }

        else if (millis() > (latestButtonRelease + minimumReleaseMillis))
        {
          if (_printDebug == true)
          {
            _debugSerial->println(F("FlyingJalapeno2::waitForButtonPressRelease: button 2 has been released"));
          }
          keepGoing = false; // Button has been released for long enough. Time to leave the loop
        }
      }
    }

    // Check for a timeout
    // Check if millis is greater than timeoutMillis plus minimumHoldMillis plus minimumReleaseMillis
    //   just in case: minimumHoldMillis or minimumReleaseMillis is > timeoutMillis
    if (millis() > (startMillis + timeoutMillis + minimumHoldMillis + minimumReleaseMillis))
    {
      if (_printDebug == true)
      {
        _debugSerial->println(F("FlyingJalapeno2::waitForButtonPressRelease: timed out!"));
      }
      keepGoing = false; // Timeout. Time to leave the loop
      timedOut = true;
    }
  }

  // keepGoing is false
  // If timedOut is true, return zero
  if (timedOut)
    return (0);
  
  // timedOut is false, so we must have recorded a valid button press and release
  return (result);
}

int FlyingJalapeno2::waitForButtonReleasePressRelease(unsigned long timeoutMillis, unsigned long minimumPreReleaseMillis, unsigned long minimumHoldMillis, unsigned long minimumPostReleaseMillis)
{
  unsigned long startMillis = millis(); // Record millis when the function was called
  boolean keepGoing = true; // keepGoing if true
  boolean timedOut = false; // Indicate if we timed out
  unsigned long latestButtonRelease = 0; // Record the time of the latest button release

  //Check that neither button is pressed - for at least minimumPreReleaseMillis
  while (keepGoing)
  {
    if ((isButton1Pressed()) || (isButton2Pressed())) // Is either being pressed?
    {
      // At least one button is being pressed
      latestButtonRelease = 0;
    }
    else
    {
      // Neither button is being pressed

      // Check if this is a fresh release (latestButtonRelease == 0)
      if (latestButtonRelease == 0)
      {
        latestButtonRelease = millis(); // Record the time of the release
      }

      else if (millis() > (latestButtonRelease + minimumPreReleaseMillis))
      {
        if (_printDebug == true)
        {
          _debugSerial->println(F("FlyingJalapeno2::waitForButtonReleasePressRelease: neither button pressed. Calling waitForButtonPressRelease"));
        }
        keepGoing = false; // Buttons have been released for long enough. Time to leave the loop
      }
    }

    // Check for a timeout
    // Check if millis is greater than timeoutMillis plus minimumPreReleaseMillis plus minimumHoldMillis plus minimumPostReleaseMillis
    //   just in case: minimumPreReleaseMillis or minimumHoldMillis or minimumPostReleaseMillis is > timeoutMillis
    if (millis() > (startMillis + timeoutMillis + minimumPreReleaseMillis + minimumHoldMillis + minimumPostReleaseMillis))
    {
      if (_printDebug == true)
      {
        _debugSerial->println(F("FlyingJalapeno2::waitForButtonReleasePressRelease: timed out!"));
      }
      keepGoing = false; // Timeout. Time to leave the loop
      timedOut = true;
    }
  }

  // keepGoing is false
  // If timedOut is true, return zero
  if (timedOut)
    return (0);
  
  //Now start checking for a valid button press and release
  int result = waitForButtonPressRelease(timeoutMillis, minimumHoldMillis, minimumPostReleaseMillis, startMillis);

  return (result);
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
  int reading = averagedAnalogRead(read_pin);

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::PreTest_Custom: jumper test reading: "));
    _debugSerial->println(reading);
  }

  digitalWrite(control_pin, LOW);
  pinMode(control_pin, INPUT);

  float jumper_val = 486;

  if ((((float)reading) < (jumper_val * 1.03)) && (((float)reading) > (jumper_val * 0.97))) return false; // jumper detected!!
  else return true;
}

// GENERIC PRE-TEST for shorts to GND on power rails, returns FALSE if all is good, returns TRUE if there is short detected
boolean FlyingJalapeno2::isShortToGround_Custom(byte control_pin, byte read_pin)
{
  pinMode(control_pin, OUTPUT);
  pinMode(read_pin, INPUT);

  digitalWrite(control_pin, HIGH);
  delay(200);
  int reading = averagedAnalogRead(read_pin);

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::isShortToGround_Custom: jumper test reading: "));
    _debugSerial->println(reading);
  }

  digitalWrite(control_pin, LOW);
  pinMode(control_pin, INPUT);

  float jumper_val = 486;

  if ((((float)reading) < (jumper_val * 1.03)) && (((float)reading) > (jumper_val * 0.97)))
		return true; // jumper detected!!
  return false;
}

//Test power circuit to see if there is a short on the target
//Returns true if there is a short
boolean FlyingJalapeno2::isV1Shorted(int shortThreshold)
{
  return (powerTest(1, shortThreshold) == false); // Test V1
}

boolean FlyingJalapeno2::isV2Shorted(int shortThreshold)
{
  return (powerTest(2, shortThreshold) == false); // Test V2
}

//PRIVATE: Test target board for shorts to GND
//Called by isV1Shorted() and isV2Shorted()
//Returns true if all is good, returns false if there is short detected
boolean FlyingJalapeno2::powerTest(byte select, int shortThreshold) // select is either "1" or "2"
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
    if (_printDebug == true)
    {
      _debugSerial->println(F("FlyingJalapeno2::powerTest: Error! select must be 1 or 2."));
    }
    return (false);
  }

  //Now setup the control pin
  pinMode(FJ2_POWER_TEST_CONTROL, OUTPUT);
  digitalWrite(FJ2_POWER_TEST_CONTROL, HIGH);

  pinMode(read_pin, INPUT);

  delay(200); //Wait for voltage to settle before taking a ADC reading

  int reading = averagedAnalogRead(read_pin);

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::powerTest: power test reading: "));
    _debugSerial->println(reading);
  }

  //Release the control pin
  digitalWrite(FJ2_POWER_TEST_CONTROL, LOW);
  pinMode(FJ2_POWER_TEST_CONTROL, INPUT);

  //Actual readings taken with the FJ2:
  //
  //When VCC is 3.3V:
  //  Open circuit on V1/V2 reads 680
  //  Short circuit on V1/V2 reads 410
  //When VCC is 5.0V:
  //  Open circuit on V1/V2 reads 620
  //  Short circuit on V1/V2 reads 430
  //
  //So, to check for a short, we should check if reading is lower than ~550

  if (reading < shortThreshold)
    return false; // jumper detected!!
  return true;
}

//Set the number of analog reads to average
void FlyingJalapeno2::setAnalogReadSamples(long samples)
{
  _numAnalogSamples = samples;
}

//Average the analog reading to minimise noise
int FlyingJalapeno2::averagedAnalogRead(byte analogPin)
{
  long runningTotal = 0;
  for (long i = 0; i < _numAnalogSamples; i++)
  {
    runningTotal += analogRead(analogPin);
    delay(1);
  }
  return ((int)(runningTotal / _numAnalogSamples));
}

//Test a pin to see what voltage is on the pin.
//Returns true if pin voltage is within a given window of the value we are looking for
//pin = pin to test
//expectedVoltage = voltage we expect. 0.0 to 5.0 (float)
//allowedPercent = allowed window for overage. 0 to 100 (int) (default 10%)
boolean FlyingJalapeno2::verifyVoltage(int pin, float expectedVoltage, int allowedPercent)
{
  //float allowanceFraction = map(allowedPercent, 0, 100, 0, 1.0); //Scale int to a fraction of 1.0
  //Grrrr! map doesn't work with floats at all

  float allowanceFraction = allowedPercent / 100.0; //Scale the allowedPercent to a float

  pinMode(pin, INPUT); //Make sure pin is an input

  delay(200); //Wait for voltage to settle before taking a ADC reading

  int reading = averagedAnalogRead(pin);

  //Convert reading to voltage
  float readVoltage = _FJ_VCC / 1023 * reading;

  boolean result = ((readVoltage <= (expectedVoltage * (1.0 + allowanceFraction))) && (readVoltage >= (expectedVoltage * (1.0 - allowanceFraction))));

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::verifyVoltage: expectedVoltage: "));
    _debugSerial->println(expectedVoltage, 2);

    _debugSerial->print(F("FlyingJalapeno2::verifyVoltage: allowanceFraction: "));
    _debugSerial->println(allowanceFraction, 2);

    _debugSerial->print(F("FlyingJalapeno2::verifyVoltage: reading: "));
    _debugSerial->println(reading);
    
    _debugSerial->print(F("FlyingJalapeno2::verifyVoltage: voltage: "));
    _debugSerial->println(readVoltage, 2);

    _debugSerial->print(F("FlyingJalapeno2::verifyVoltage: result: "));
    _debugSerial->println(result);
  }

  return (result);
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
    if (_printDebug == true)
    {
      _debugSerial->println(F("FlyingJalapeno2::enableV1: setVoltageV1 has not been called. Aborting..."));
    }
    return;
  }

  digitalWrite(FJ2_V1_POWER_CONTROL, HIGH); // turn on the high side switch
  pinMode(FJ2_V1_POWER_CONTROL, OUTPUT);
  _V1_actual = _V1_setting;
  if (_printDebug == true)
  {
    _debugSerial->println(F("FlyingJalapeno2::enableV1: V1 enabled!"));
  }
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
    if (_printDebug == true)
    {
      _debugSerial->println(F("FlyingJalapeno2::enableV2: setVoltageV2 has not been called. Aborting..."));
    }
    return;
  }

  digitalWrite(FJ2_V2_POWER_CONTROL, HIGH); // turn on the high side switch
  pinMode(FJ2_V2_POWER_CONTROL, OUTPUT);
  _V2_actual = _V2_setting;
  if (_printDebug == true)
  {
    _debugSerial->println(F("FlyingJalapeno2::enableV2: V2 enabled!"));
  }
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
    if (_printDebug == true)
    {
      _debugSerial->print(F("FlyingJalapeno2::setVoltageV1: invalid voltage specified: "));
      _debugSerial->print(voltage, 2);
      _debugSerial->println(F(". Defaulting to 3.3V"));
    }
    pinMode(FJ2_V1_CONTROL_TO_3V3, OUTPUT); // default to 3.3V - even when the high side switch is turn off.
    digitalWrite(FJ2_V1_CONTROL_TO_3V3, LOW);
    _V1_setting = 3.3;
  }

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::setVoltageV1: V1 will be "));
    _debugSerial->print(_V1_setting, 1);
    _debugSerial->println(F("V when enabled"));
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
    if (_printDebug == true)
    {
      _debugSerial->print(F("FlyingJalapeno2::setVoltageV2: invalid voltage specified: "));
      _debugSerial->print(voltage, 2);
      _debugSerial->println(F(". Defaulting to 3.3V"));
    }
    pinMode(FJ2_V2_CONTROL_TO_3V3, OUTPUT); // default to 3.3V
    digitalWrite(FJ2_V2_CONTROL_TO_3V3, LOW);
    _V2_setting = 3.3;
  }

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::setVoltageV2: V2 will be "));
    _debugSerial->print(_V2_setting, 1);
    _debugSerial->println(F("V when enabled"));
  }
}

//Return _V1_setting - i.e. what V1 will be when enabled
float FlyingJalapeno2::getVoltageSettingV1()
{
  return (_V1_setting);
}

//Return _V2_setting - i.e. what V2 will be when enabled
float FlyingJalapeno2::getVoltageSettingV2()
{
 return (_V2_setting);
}


//Test if the voltage on V1/V2 is OK. Returns false if the voltage is out of range
//Note: due to the 10k/11k divider on the PT_READ pins, we can only verify voltages which are lower than VCC * 0.9
boolean FlyingJalapeno2::testVoltage(byte select) // select is either "1" or "2"
{
  //Specify the read_pin and expected voltage
  byte read_pin;
  float expectedVoltage;
  if (select == 1)
  {
    read_pin = FJ2_PT_READ_V1;
    expectedVoltage = _V1_actual * 10.0 / 11.0; // Compensate for resistor divider
    //If VCC is 5.0V and V1/V2 are also 5.0V, the ADC reading is ~950
    // which converts to 4.64V. So, for 5V, the fiddle factor should be 1.02
    //If VCC is 3.3V and V1/V2 are also 3.3V, the ADC reading is ~970
    // which converts to 3.13V. So, for 3.3V, the fiddle factor should be 1.04
    //Let's split the difference and use a fiddle factor of 1.03
    expectedVoltage *= 1.03; // Fiddle factor - from FJ2 testing
  }
  else if (select == 2)
  {
    read_pin = FJ2_PT_READ_V2;
    expectedVoltage = _V2_actual * 10.0 / 11.0; // Compensate for resistor divider
    expectedVoltage *= 1.03; // Fiddle factor - from FJ2 testing
  }
  else
  {
    if (_printDebug == true)
    {
      _debugSerial->println(F("FlyingJalapeno2::testVoltage: Error! select must be 1 or 2."));
    }
    return (false);
  }

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::testVoltage: Testing V"));
    _debugSerial->print(select);
    _debugSerial->print(F(". The expected voltage (from the resistor divider) is "));
    _debugSerial->print(expectedVoltage, 2);
    _debugSerial->println(F("V"));
  }

  //Verify the voltage is within 5%
  return (verifyVoltage(read_pin, expectedVoltage, 5));
}

//Test if the FJ2 VCC has been set correctly (using the 3.3V Zener diode on FJ2_BRAIN_VCC_A0)
//Return true if FJ2_BRAIN_VCC_A0 matches _FJ_VCC
boolean FlyingJalapeno2::testVCC()
{
  //Check VCC by reading the 3.3V zener connected to A0
  //If VCC is 3.3V, the signal on A0 will be close to full range
  //If VCC is 5V, the signal on A0 will be (roughly) 3.3V/5V * 1023 = 675

  int val = averagedAnalogRead(FJ2_BRAIN_VCC_A0);

  if (_printDebug == true)
  {
    _debugSerial->print(F("FlyingJalapeno2::testVCC: VCC should be "));
    _debugSerial->print(_FJ_VCC, 2);
    _debugSerial->println(F("V"));
    _debugSerial->print(F("FlyingJalapeno2::testVCC: val is: "));
    _debugSerial->println(val);
  }

  if ((_FJ_VCC >= 3.29) && (_FJ_VCC <= 3.31)) // Is VCC supposed to be 3.3V?
  {
    // val should be close to 1023. Return false if it isn't (i.e. VCC is higher than 3.3V!)
    // Note: on the one FJ2 I have tested so far, val is: ~900 for 3.3V; and ~700 for 5.0V
    if (val < 800)
    {
      if (_printDebug == true)
      {
        _debugSerial->println(F("FlyingJalapeno2::testVCC: PANIC! VCC appears to be higher than 3.3V!"));
      }
      return false;
    }
    return true;
  }

  // else: _FJ_VCC must be 5.0V so check diode voltage reads as 3.3V
  boolean result = verifyVoltage(FJ2_BRAIN_VCC_A0, 3.3, 10);

  if ((!result) && (_printDebug == true))
  {
    _debugSerial->println(F("FlyingJalapeno2::testVCC: PANIC! VCC appears to be out of bounds!"));
  }

  return (result);
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
  digitalWrite(FJ2_MICROSD_CS, HIGH); // Get ready to deselect the microSD
  pinMode(FJ2_MICROSD_CS, INPUT);
}

//Enable the microSD power by pulling FJ2_MICROSD_PWR_EN high
void FlyingJalapeno2::enableMicroSDPower()
{
  pinMode(FJ2_MICROSD_PWR_EN, OUTPUT); // Pull FJ2_MICROSD_PWR_EN high
  digitalWrite(FJ2_MICROSD_PWR_EN, HIGH);
}
//Disable the microSD power by pulling FJ2_MICROSD_PWR_EN low
void FlyingJalapeno2::disableMicroSDPower()
{
  digitalWrite(FJ2_MICROSD_PWR_EN, LOW); // Make sure the microSD power is disabled by pulling FJ2_MICROSD_PWR_EN low
  pinMode(FJ2_MICROSD_PWR_EN, OUTPUT);
}

//Verify the address of an I2C device
//If address is zero, do a full scan
//Return true if the specified address pings correctly
boolean FlyingJalapeno2::verifyI2Cdevice(byte address)
{
  byte error;
  boolean result = false;

  for (int device = 1; device < 127; device++) // Step through all devices
  {
    if ((address == 0) || (device == address)) // Check if we should ping this one
    {
      if (_printDebug == true)
      {
        _debugSerial->print(F("FlyingJalapeno2::verifyI2Cdevice: Pinging address 0x"));
        if (device < 16) _debugSerial->print(F("0"));
        _debugSerial->print(device, HEX);
      }

      Wire.beginTransmission(device); // Ping this device
      error = Wire.endTransmission();

      if (error == 0)
      {
        if (_printDebug == true)
        {
          _debugSerial->println(F("... Found!"));
        }
        result = true;
      }
      else if (error == 4)
      {
        if (_printDebug == true)
        {
          _debugSerial->println(F("... Unknown error!"));
        }
      }
      else
      {
        if (_printDebug == true)
        {
          _debugSerial->println();
        }
      }
    }
  }

  return (result);
}

