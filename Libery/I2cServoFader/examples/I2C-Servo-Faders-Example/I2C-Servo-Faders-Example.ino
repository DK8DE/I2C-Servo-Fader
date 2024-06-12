/************************************************************************************/
/*!
@brief Example Code for Controlling I2C Servo Faders

This example code demonstrates how to control I2C servo faders using an Arduino.
The code supports one fader and can be extended to two faders by uncommenting the
relevant sections. The faders can be controlled via serial commands, and their 
positions and LED colors can be adjusted based on analog values.

## Serial Commands:
- `faderpos1=<value>`: Sets the position of fader 1 to the specified value (0-255).
- `faderpos2=<value>`: Sets the position of fader 2 to the specified value (0-255).
- `fadertest=1`: Initiates a demonstration of fader 1's movement through a series of positions.
- `fadertest=2`: Initiates a demonstration of fader 2's movement through a series of positions.

## Interrupt Handling:
When an interrupt occurs, the code reads the analog value and optio state from the faders,
prints these values to the serial console, and updates the LED colors based on the analog values.

The interrupt is triggered by a change in the state of the connected pin (e.g., pin 2).

## LED Color Representation:
- Green (analog value = 0)
- Yellow (analog value ≈ 128)
- Red (analog value = 255)
The LED color transitions smoothly between these values based on the analog value.
*/
/************************************************************************************/

#include <I2cServoFader.h>

// I2C addresses of the faders
#define FADER1_I2C_ADDRESS 0x08
#define FADER2_I2C_ADDRESS 0x09

I2cServoFader fader1(FADER1_I2C_ADDRESS);
I2cServoFader fader2(FADER2_I2C_ADDRESS);  // Commented out for now

volatile bool interruptOccurred = false;

/************************************************************************************/
/*!
@brief Interrupt Service Routine for handling interrupt from the faders
*/
/************************************************************************************/
void IRAM_ATTR handleInterrupt() {
  interruptOccurred = true;
}

/************************************************************************************/
/*!
@brief Sets the LED color based on the analog value
@param fader The fader object to set the color for
@param analogValue The analog value to determine the color
*/
/************************************************************************************/
void updateLEDColor(I2cServoFader &fader, uint8_t analogValue) {
  uint8_t red = map(analogValue, 0, 255, 0, 255);
  uint8_t green = map(analogValue, 0, 255, 255, 0);
  uint8_t blue = 0;

  fader.setColor(red, green, blue);
  fader.setBrightness(60);
}

/************************************************************************************/
/*!
@brief Demonstrates changing the position of the fader
@param fader The fader object to demonstrate
*/
/************************************************************************************/
void demonstrateFaderMovement(I2cServoFader &fader) {
  Serial.println("Start fader movement demonstration.");
  delay(500);

  // Demonstrate changing the position
  fader.setPosition(0);
  delay(1000);
  updateLEDColor(fader, fader.readAnalogValue());
  Serial.println("Fader movement position 0");

  fader.setPosition(128);
  delay(1000);
  updateLEDColor(fader, fader.readAnalogValue());
  Serial.println("Fader movement position 128");

  fader.setPosition(255);
  delay(1000);
  updateLEDColor(fader, fader.readAnalogValue());
  Serial.println("Fader movement position 255");

  fader.setPosition(0);
  delay(1000);
  updateLEDColor(fader, fader.readAnalogValue());
  Serial.println("Fader movement position 0");

  Serial.println("Fader movement demonstration complete.");
}

/************************************************************************************/
/*!
@brief Setup function for initializing faders and serial communication

- Initializes the I2C communication for both faders
- Attaches interrupt service routine for handling fader interrupts
*/
/************************************************************************************/
void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the I2C communication for both faders
  fader1.begin();
  // fader2.begin();  // Commented out for now

  // Set the interrupt pin
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);

  Serial.println("Ready to receive commands...");
}

/************************************************************************************/
/*!
@brief Main loop function for handling fader operations

- Reads the analog value and optio state from faders upon interrupt
- Prints the read values to the serial console
- Allows setting fader positions via serial console commands
- Updates LED colors based on analog values from faders
*/
/************************************************************************************/
void loop() {
  // Handle interrupt
  if (interruptOccurred) {
    interruptOccurred = false;

    // Read analog value and optio register from fader1
    uint8_t analogValue1 = fader1.readAnalogValue();
    uint8_t optioState1 = fader1.readOptio();

    // Print the read values to the serial console
    if (Serial.available()) {
      Serial.print("Fader 1 - Analog value: ");
      Serial.println(analogValue1);
      Serial.print("Fader 1 - OPTIO state: ");
      Serial.println(optioState1);
    }
    
    // Read analog value and optio register from fader2 (Commented out for now)
    /*
    uint8_t analogValue2 = fader2.readAnalogValue();
    uint8_t optioState2 = fader2.readOptio();
    
    // Print the read values to the serial console
    if (Serial.available()) {
      Serial.print("Fader 2 - Analog value: ");
      Serial.println(analogValue2);
      Serial.print("Fader 2 - OPTIO state: ");
      Serial.println(optioState2);
    }

    // Update LED colors based on analog values
    updateLEDColor(fader2, analogValue2);
    */

    // Update LED colors based on analog values
    updateLEDColor(fader1, analogValue1);
  }

  // Read and set the position from the serial console
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove whitespace and newline characters

    if (input.startsWith("faderpos1=")) {
      int position = input.substring(10).toInt();
      if (position >= 0 && position <= 255) {
        fader1.setPosition(position);
        Serial.print("Fader 1 position set to: ");
        Serial.println(position);
        updateLEDColor(fader1, fader1.readAnalogValue());
      }
    }
    /*
    else if (input.startsWith("faderpos2=")) {
      int position = input.substring(10).toInt();
      if (position >= 0 && position <= 255) {
        fader2.setPosition(position);
        Serial.print("Fader 2 position set to: ");
        Serial.println(position);
        updateLEDColor(fader2, fader2.readAnalogValue());
      }
    }
    */

    else if (input.startsWith("fadertest=")) {
      int faderNumber = input.substring(10).toInt();
      if (faderNumber == 1) {
        demonstrateFaderMovement(fader1);
      }
      /*
      else if (faderNumber == 2) {
        demonstrateFaderMovement(fader2);
      }
      */
    }
  }

  delay(100); // Small delay to prevent overwhelming the I2C bus
}
