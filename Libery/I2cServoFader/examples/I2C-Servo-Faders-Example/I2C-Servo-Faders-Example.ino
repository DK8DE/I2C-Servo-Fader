/************************************************************************************/
/*!
@brief Example Code for Controlling I2C Servo Faders

This example code demonstrates how to control I2C servo faders using an Arduino.
The code supports one fader and can be extended to two faders by uncommenting the
relevant sections. The faders can be controlled via serial commands, and their 
positions and LED colors can be adjusted based on analog values.

## Serial Commands:
- `sliderpos1=<value>`: Sets the position of fader 1 to the specified value (0-255).
- `sliderpos2=<value>`: Sets the position of fader 2 to the specified value (0-255).
- `slidertest=1`: Initiates a demonstration of fader 1's movement through a series of positions.
- `slidertest=2`: Initiates a demonstration of fader 2's movement through a series of positions.

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
#define SLIDER1_I2C_ADDRESS 0x08
#define SLIDER2_I2C_ADDRESS 0x09

I2cServoFader slider1(SLIDER1_I2C_ADDRESS);
I2cServoFader slider2(SLIDER2_I2C_ADDRESS);  // Commented out for now

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
@param slider The slider object to set the color for
@param analogValue The analog value to determine the color
*/
/************************************************************************************/
void updateLEDColor(I2cServoFader &slider, uint8_t analogValue) {
  uint8_t red = map(analogValue, 0, 255, 0, 255);
  uint8_t green = map(analogValue, 0, 255, 255, 0);
  uint8_t blue = 0;

  slider.setColor(red, green, blue);
  slider.setBrightness(60);
}

/************************************************************************************/
/*!
@brief Demonstrates changing the position of the slider
@param slider The slider object to demonstrate
*/
/************************************************************************************/
void demonstrateSliderMovement(I2cServoFader &slider) {
  Serial.println("Start slider movement demonstration.");
  delay(500);

  // Demonstrate changing the position
  slider.setPosition(0);
  delay(1000);
  updateLEDColor(slider, slider.readAnalogValue());
  Serial.println("Slider movement position 0");

  slider.setPosition(128);
  delay(1000);
  updateLEDColor(slider, slider.readAnalogValue());
  Serial.println("Slider movement position 128");

  slider.setPosition(255);
  delay(1000);
  updateLEDColor(slider, slider.readAnalogValue());
  Serial.println("Slider movement position 255");

  slider.setPosition(0);
  delay(1000);
  updateLEDColor(slider, slider.readAnalogValue());
  Serial.println("Slider movement position 0");

  Serial.println("Slider movement demonstration complete.");
}

/************************************************************************************/
/*!
@brief Setup function for initializing sliders and serial communication

- Initializes the I2C communication for both sliders
- Attaches interrupt service routine for handling slider interrupts
*/
/************************************************************************************/
void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the I2C communication for both sliders
  slider1.begin();
  // slider2.begin();  // Commented out for now

  // Set the interrupt pin
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);

  Serial.println("Ready to receive commands...");
}

/************************************************************************************/
/*!
@brief Main loop function for handling slider operations

- Reads the analog value and optio state from sliders upon interrupt
- Prints the read values to the serial console
- Allows setting slider positions via serial console commands
- Updates LED colors based on analog values from sliders
*/
/************************************************************************************/
void loop() {
  // Handle interrupt
  if (interruptOccurred) {
    interruptOccurred = false;
    
    // Read analog value and optio register from slider1
    uint8_t analogValue1 = slider1.readAnalogValue();
    uint8_t optioState1 = slider1.readOptio();
    
    // Print the read values to the serial console
    Serial.print("Slider 1 - Analog value: ");
    Serial.println(analogValue1);
    Serial.print("Slider 1 - OPTIO state: ");
    Serial.println(optioState1);
    
    // Read analog value and optio register from slider2 (Commented out for now)
    /*
    uint8_t analogValue2 = slider2.readAnalogValue();
    uint8_t optioState2 = slider2.readOptio();
    
    // Print the read values to the serial console
    Serial.print("Slider 2 - Analog value: ");
    Serial.println(analogValue2);
    Serial.print("Slider 2 - OPTIO state: ");
    Serial.println(optioState2);

    // Update LED colors based on analog values
    updateLEDColor(slider2, analogValue2);
    */

    // Update LED colors based on analog values
    updateLEDColor(slider1, analogValue1);
  }

  // Read and set the position from the serial console
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove whitespace and newline characters

    if (input.startsWith("sliderpos1=")) {
      int position = input.substring(11).toInt();
      if (position >= 0 && position <= 255) {
        slider1.setPosition(position);
        Serial.print("Slider 1 position set to: ");
        Serial.println(position);
        updateLEDColor(slider1, slider1.readAnalogValue());
      }
    } 
    /*
    else if (input.startsWith("sliderpos2=")) {
      int position = input.substring(11).toInt();
      if (position >= 0 && position <= 255) {
        slider2.setPosition(position);
        Serial.print("Slider 2 position set to: ");
        Serial.println(position);
        updateLEDColor(slider2, slider2.readAnalogValue());
      }
    }
    */

    else if (input.startsWith("slidertest=")) {
      int sliderNumber = input.substring(11).toInt();
      if (sliderNumber == 1) {
        demonstrateSliderMovement(slider1);
      }
      /*
      else if (sliderNumber == 2) {
        demonstrateSliderMovement(slider2);
      }
      */
    }
  }

  delay(50); // Small delay to prevent overwhelming the I2C bus
}
