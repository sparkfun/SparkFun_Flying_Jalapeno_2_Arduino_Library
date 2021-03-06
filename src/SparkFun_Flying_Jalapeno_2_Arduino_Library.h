
#ifndef _SPAPKFUN_FLYING_JALAPENO_ARDUINO_LIBRARY_H_
#define _SPAPKFUN_FLYING_JALAPENO_ARDUINO_LIBRARY_H_

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include <CapacitiveSensor.h> //Click here to get the library: http://librarymanager/All#CapacitiveSensor_Arduino

// ***** FJ2 Pin Definitions *****

//These are the four LEDs on the test jig
//Setting these pins high turn on a given LED
#define FJ2_LED_PT_PASS 40 //PT = Pre-test or Program_&_Test
#define FJ2_LED_PRETEST_PASS 40 //Duplicate but more descriptive
#define FJ2_LED_PROGRAM_AND_TEST_PASS 40 //Duplicate but more descriptive
#define FJ2_LED_TEST_PASS 42
#define FJ2_LED_FAIL 43
#define FJ2_STAT_LED 41

// These lines are connected to different resistors off the adj line
// Pulling pins low enables the resistors
// Turning pins to input disables the resistors
#define FJ2_V1_CONTROL_TO_3V3 33
#define FJ2_V1_CONTROL_TO_5V0 35

#define FJ2_V2_CONTROL_TO_3V3 36
#define FJ2_V2_CONTROL_TO_3V7 37
#define FJ2_V2_CONTROL_TO_4V2 38
#define FJ2_V2_CONTROL_TO_5V0 39

// Set this pin high to enable power supply
#define FJ2_V1_POWER_CONTROL 48
#define FJ2_V2_POWER_CONTROL 4

//Setting this pin high puts a small voltage between two resistors on VCC and Ground
//If the ADC value is too low or too high you know there's a short somewhere on target board
#define FJ2_POWER_TEST_CONTROL 44
#define FJ2_PT_READ_V1 A14 
#define FJ2_PT_READ_V2 A15

//Cap Sense Buttons
#define FJ2_CAP_SENSE_BUTTON_1 45 //Pre-test or Program_&_Test Button
#define FJ2_CAP_SENSE_BUTTON_2 46 //Test Button
#define FJ2_CAP_SENSE_RETURN 31

//3.3V Zener Diode - used to check Brain VCC
#define FJ2_BRAIN_VCC_A0 A0

//The following pins are optional and might not be used on a particular test jig
#define FJ2_TX1 18 // Serial1 TX
#define FJ2_RX1 19 // Serial1 TX
#define FJ2_SDA 20 // I2C SDA
#define FJ2_SCL 21 // I2C SCL
#define FJ2_I2C_EN 22 // 74LVC4066 I2C buffer enable
#define FJ2_SERIAL_EN 23 // 74LVC4066 Serial buffer enable
#define FJ2_SPI_EN 24 // 74LVC4066 SPI buffer enable
#define FJ2_MICROSD_PWR_EN 30 // microSD power enable (power is switched by a small P-MOSFET)
#define FJ2_MICROSD_EN 47 // 74LVC4066 microSD buffer enable
#define FJ2_MICROSD_CS 49 // 74LVC4066 SPI buffer enable

//The SPI pins
#define FJ2_CIPO 50
#define FJ2_COPI 51
#define FJ2_SCK 52
#define FJ2_TARGET_CS 53


// ***** The FJ2 Class *****

class FlyingJalapeno2
{
  public:

    FlyingJalapeno2(int statLED, float FJ_VCC = 3.3, bool useCapSense = true);

    void enableDebugging(Stream &debugPort = Serial); // Enable helpful debug messages on the chosen serial port
    void disableDebugging(); // Turn off debug messages

    void reset(boolean resetLEDs = true); //Reset the FJ2. Turn everything off. Also calls userReset
    void userReset(boolean resetLEDs = true) __attribute__((weak)); //The user can overwrite this with a custom reset function for the board being tested

    // ***** FJ2 Buttons *****
    CapacitiveSensor *FJ2button1;
    CapacitiveSensor *FJ2button2;

    long _capSenseThreshold = 2000; // The user can change the default threshold by calling setCapSenseThreshold
    void setCapSenseThreshold(long threshold = 2000); //Allow the user to override the default cap sense threshold

    uint8_t _capSenseSamples = 30; // The user can change the number of samples by calling setCapSenseSamples
    void setCapSenseSamples(uint8_t samples = 30); //Allow the user to override the number of cap sense samples
	
    boolean isPretestPressed(long threshold = 0); //Returns true if cap sense button 1 is being pressed. _capSenseThreshold is used if threshold is 0
    boolean isProgramAndTestPressed(long threshold = 0); //Helper function: calls isPretestPressed. _capSenseThreshold is used if threshold is 0
    boolean isButton1Pressed(long threshold = 0); //Helper function: calls isPretestPressed. _capSenseThreshold is used if threshold is 0

    boolean isTestPressed(long threshold = 0); //Returns true if cap sense button 2 is being pressed. _capSenseThreshold is used if threshold is 0
    boolean isButton2Pressed(long threshold = 0); //Helper function: calls isTestPressed. _capSenseThreshold is used if threshold is 0
    
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
    int waitForButtonPress(unsigned long timeoutMillis = 5000, unsigned long minimumHoldMillis = 50, unsigned long overrideStartMillis = 0);
    int waitForButtonPressRelease(unsigned long timeoutMillis = 5000, unsigned long minimumHoldMillis = 50, unsigned long minimumReleaseMillis = 100, unsigned long overrideStartMillis = 0);
    int waitForButtonReleasePressRelease(unsigned long timeoutMillis = 5000, unsigned long minimumPreReleaseMillis = 100, unsigned long minimumHoldMillis = 50, unsigned long minimumPostReleaseMillis = 100);

    void statOn(); //Turn the stat LED on
    void statOff();

    long _numAnalogSamples = 25; //The user can change this by calling setAnalogReadSamples
    void setAnalogReadSamples(long samples = 25); //Set the number of analog reads to average
    int averagedAnalogRead(byte analogPin); //Average the analog reading to minimise noise

    //Returns true if pin voltage is within a given window of the value we are looking for
    boolean verifyVoltage(int pin, float expectedVoltage, int allowedPercent = 10); 
    
    boolean verifyValue(float input_value, float correct_val, float allowance_percent);

    boolean PreTest_Custom(byte control_pin, byte read_pin);
    
    boolean isV1Shorted(int shortThreshold = 550); //Test V1 for shorts. Returns true if short detected.
    boolean isV2Shorted(int shortThreshold = 550); //Test V2 for shorts. Returns true if short detected.
    boolean isShortToGround_Custom(byte control_pin, byte read_pin); // test for a short to gnd on a custom set of pins

    void setVoltageV1(float voltage); //Set V1 voltage (5 or 3.3V)
    void setVoltageV2(float voltage); //Set V2 voltage (3.3, 3.7, 4.2, or 5V)
    float getVoltageSettingV1(); //Return _V1_setting - i.e. what V1 will be when enabled
    float getVoltageSettingV2(); //Return _V2_setting - i.e. what V2 will be when enabled

    boolean testVoltage(byte select); //Test if the voltage on V1/V2 is OK. Returns false if the voltage is out of range

    boolean testVCC(); //Test if the FJ2 VCC has been set correctly (using the 3.3V Zener diode on FJ2_BRAIN_VCC_A0)

    //Enable or disable the power regulators
    void enableV1();
    void disableV1();
    void enableV2();
    void disableV2();

    void dot(int pin = -1); // If pin is -1, _statLED is blinked
    void dash(int pin = -1); // If pin is -1, _statLED is blinked
    void SOS(int pin = -1); // If pin is -1, _statLED is blinked

    void enableI2CBuffer(); //Enable the I2C buffer by pulling FJ2_I2C_EN high
    void disableI2CBuffer(); //Disable the I2C buffer by pulling FJ2_I2C_EN low
    void enableSerialBuffer(); //Enable the Serial buffer by pulling FJ2_SERIAL_EN high
    void disableSerialBuffer(); //Disable the Serial buffer by pulling FJ2_SERIAL_EN low
    void enableSPIBuffer(); //Enable the SPI buffer by pulling FJ2_SPI_EN high
    void disableSPIBuffer(); //Disable the SPI buffer by pulling FJ2_SPI_EN low
    void enableMicroSDBuffer(); //Enable the microSD buffer by pulling FJ2_MICROSD_EN high
    void disableMicroSDBuffer(); //Disable the microSD buffer by pulling FJ2_MICROSD_EN low
    void enableMicroSDPower(); //Enable the microSD power by pulling FJ2_MICROSD_PWR_EN high
    void disableMicroSDPower(); //Disable the microSD power by pulling FJ2_MICROSD_PWR_EN low

    boolean verifyI2Cdevice(byte address = 0); // If address is zero, do a full scan

  private:

  	Stream *_debugSerial;			//The stream to send debug messages to if enabled
  	boolean _printDebug = false;		//Flag to print the serial commands we are sending to the Serial port for debug

    int _statLED; // Define which status LED to use. Usually FJ2_STAT_LED, but can be custom if needed
	  float _FJ_VCC; // The FJ2 VCC. Used in A2D voltage calculations
    float _V1_actual = 0.0; // The actual V1 voltage. Used by testVoltage
    float _V2_actual = 0.0; // The actual V2 voltage. Used by testVoltage
    float _V1_setting = 0.0; // What V1 will be when enabled
    float _V2_setting = 0.0; // What V2 will be when enabled
    bool _useCapSense = true; // True: use CapacitiveSensor. False: use (e.g.) external AT42QT1011 buttons

    boolean powerTest(byte select, int shortThreshold = 550); //Test if V1/V2 pin is OK. Returns false if a short is detected
};

#endif