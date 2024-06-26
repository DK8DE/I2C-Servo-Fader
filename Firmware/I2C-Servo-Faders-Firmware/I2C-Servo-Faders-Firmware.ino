#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define DEFAULT_I2C_ADDRESS 0x08  //!< Default I2C address of the client
#define EEPROM_I2C_ADDRESS 0      //!< EEPROM address for storing the I2C address
#define NEOPIXEL_PIN 4            //!< Data pin for the NeoPixel LED
#define NUMPIXELS 1               //!< Number of LEDs
#define STBY 7                    //!< Standby pin
#define ANALOG_PIN A0             //!< Pin for analogRead
#define TOUCH_PIN 2               //!< Signal from the touch controller when the button is touched
#define INTERRUPT 8               //!< Pin opposite to TOUCH_PIN
#define PWM_PIN 3                 //!< Pin for PWM signal
#define DIR_PIN1 9                //!< Pin for motor direction
#define DIR_PIN2 10               //!< Pin for motor direction
#define OPTIO_PIN 11              //!< Optional IO pin

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Register addresses
enum Register {
  REGISTER_ON = 0,          //!< Register for turning the LED on or off
  REGISTER_RED,             //!< Register for setting the red color value
  REGISTER_GREEN,           //!< Register for setting the green color value
  REGISTER_BLUE,            //!< Register for setting the blue color value
  REGISTER_BRIGHTNESS,      //!< Register for setting the brightness value
  REGISTER_ANALOG_VALUE,    //!< Register for reading the analog value
  REGISTER_TOUCH,           //!< Register for reading the touch state
  REGISTER_OPTIO_READ,      //!< Register for reading the state of OPTIO_PIN
  REGISTER_SET_POSITION,    //!< Register for setting the target position of the servo
  REGISTER_COUNT            //!< Total number of registers
};

// Function declarations
void setLED();
void receiveEvent(int howMany);
void requestEvent();
void updateAnalogValue();
void stopMotor();
void shortBrake();
void standby();
void rotateMotorClockwise(uint8_t pwm);
void rotateMotorCounterclockwise(uint8_t pwm);
void moveMotorToPosition();

uint8_t registers[REGISTER_COUNT] = { 0,    //!< LED ON OFF 1=ON 0=OFF
                                      0,    //!< LED RED
                                      0,    //!< LED GREEN
                                      0,    //!< LED BLUE
                                      0,    //!< LED Brightness
                                      0,    //!< Initial value for ANALOG_VALUE register
                                      0,    //!< Initial value for TOUCH register
                                      0,    //!< Initial value for OPTIO_READ register
                                      0 };  //!< Initial value for SET_POSITION register

uint8_t i2cAddress;
bool touchOccurred = false; // Flag to determine if a touch has been detected
bool optioOccurred = false; // Flag to determine if an optio event has been detected

void setup() {
  Serial.begin(115200);

  // Load the stored I2C address from the EEPROM
  i2cAddress = EEPROM.read(EEPROM_I2C_ADDRESS);
  if (i2cAddress == 0xFF || i2cAddress < 0x08) {  // No valid address stored or address less than 0x08
    i2cAddress = DEFAULT_I2C_ADDRESS;
  }

  // I2C setup
  Wire.begin(i2cAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // NeoPixel setup
  pixels.begin();
  pixels.setBrightness(255);

  // Initialize the analog pin
  pinMode(ANALOG_PIN, INPUT);

  // Initialize the STBY pin
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW); // Default state

  // Initialize the TOUCH_PIN and INTERRUPT_PIN
  pinMode(TOUCH_PIN, INPUT);
  pinMode(INTERRUPT, OUTPUT);
  digitalWrite(INTERRUPT, HIGH); // Default value for INTERRUPT_PIN

  // Initialize the PWM_PIN and direction pins
  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);

  // Initialize the OPTIO_PIN
  pinMode(OPTIO_PIN, INPUT_PULLUP);

  // Default state of the motor
  stopMotor();

  // Display the current I2C address
  Serial.print(F("I2C Addr: 0x"));
  Serial.println(i2cAddress, HEX);

  Serial.println(F("Enter new I2C addr in HEX (e.g., 0x09) and press Enter:"));

  // Turn on the LED to green for 1 second on startup
  pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
  pixels.show();
  delay(1000);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // Off
  pixels.show();
}

void loop() {
  // Regularly update the analog value
  updateAnalogValue();

  // Check the state of the OPTIO_PIN
  if (digitalRead(OPTIO_PIN) == LOW) {
    if (!optioOccurred) {
      registers[REGISTER_OPTIO_READ] = 1;
      optioOccurred = true;
      digitalWrite(INTERRUPT, LOW);
    }
  } else {
    registers[REGISTER_OPTIO_READ] = 0;
  }

  // Check the state of the TOUCH_PIN
  if (digitalRead(TOUCH_PIN) == LOW) {
    if (!touchOccurred) {
      registers[REGISTER_TOUCH] = 1;
      touchOccurred = true;
      digitalWrite(INTERRUPT, LOW);
    }
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove whitespace and newline characters

    if (input.startsWith("0x")) {
      uint8_t newAddress = strtol(input.c_str(), NULL, 16);
      if (newAddress >= 0x08 && newAddress <= 127) {  // Valid I2C addresses: 0x08-0x7F
        EEPROM.write(EEPROM_I2C_ADDRESS, newAddress);
        Serial.print(F("New I2C Addr: 0x"));
        Serial.println(newAddress, HEX);
        Serial.println(F("Restart Arduino for new addr."));
      } else {
        Serial.println(F("Addr must be 0x08-0x7F."));
      }
    }
  }

  delay(10);  // Update every 10 ms
}

/************************************************************************************/
/*!
@brief Sets the color of the LED based on the values in the registers
@return none
*/
/************************************************************************************/
void setLED() {
  if (registers[REGISTER_ON] == 1) {
    // Calculate the color based on the brightness
    uint8_t green = registers[REGISTER_GREEN];
    uint8_t red = registers[REGISTER_RED];
    uint8_t blue = registers[REGISTER_BLUE];
    uint8_t brightness = registers[REGISTER_BRIGHTNESS];

    // Scale the color values according to the brightness
    red = (red * brightness) / 255;
    green = (green * brightness) / 255;
    blue = (blue * brightness) / 255;

    pixels.setPixelColor(0, pixels.Color(green, red, blue));
    pixels.show();
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

/************************************************************************************/
/*!
@brief Processes the received I2C data
@param howMany Number of received bytes
@return none
*/
/************************************************************************************/
void receiveEvent(int howMany) {
  if (howMany < 2) return;  // Expect at least 2 bytes: register address and value

  uint8_t reg = Wire.read();
  if (reg < REGISTER_COUNT) {
    uint8_t value = Wire.read();
    registers[reg] = value;
    setLED();  // Set the LED after each register update

    // Only move motor when REGISTER_SET_POSITION is updated
    if (reg == REGISTER_SET_POSITION) {
      moveMotorToPosition();
    }
  }
}

/************************************************************************************/
/*!
@brief Processes the I2C data requests
@return none
*/
/************************************************************************************/
void requestEvent() {
  uint8_t reg = Wire.read();
  if (reg < REGISTER_COUNT) {
    Wire.write(registers[reg]);
    
    if (reg == REGISTER_ANALOG_VALUE && touchOccurred) {
      // Set TOUCH register to 0 after ANALOG_VALUE has been read
      registers[REGISTER_TOUCH] = 0;
      touchOccurred = false;
    }

    if (reg == REGISTER_OPTIO_READ && optioOccurred) {
      // Set OPTIO_READ register to 0 after it has been read
      registers[REGISTER_OPTIO_READ] = 0;
      optioOccurred = false;
    }

    // Reset the interrupt pin if no events are pending
    if (!touchOccurred && !optioOccurred) {
      digitalWrite(INTERRUPT, HIGH);
    }
  }
}

/************************************************************************************/
/*!
@brief Updates the value of the analog input and stores it in the corresponding register
@return none
*/
/************************************************************************************/
void updateAnalogValue() {
  uint16_t analogValue = analogRead(ANALOG_PIN);
  registers[REGISTER_ANALOG_VALUE] = map(analogValue, 0, 1023, 0, 255);
}

/************************************************************************************/
/*!
@brief Stops the motor by first applying a short brake, then switching to standby
@return none
*/
/************************************************************************************/
void stopMotor() {
  shortBrake();
  standby();
}

/************************************************************************************/
/*!
@brief Applies a short brake by setting both direction pins to HIGH and the PWM signal to 255
@return none
*/
/************************************************************************************/
void shortBrake() {
  digitalWrite(DIR_PIN1, HIGH);
  digitalWrite(DIR_PIN2, HIGH);
  analogWrite(PWM_PIN, 255);
  digitalWrite(STBY, HIGH);
}

/************************************************************************************/
/*!
@brief Sets the motor to standby by setting both direction pins and the PWM signal to LOW
@return none
*/
/************************************************************************************/
void standby() {
  digitalWrite(DIR_PIN1, LOW);
  digitalWrite(DIR_PIN2, LOW);
  analogWrite(PWM_PIN, 0);
  digitalWrite(STBY, LOW);
}

/************************************************************************************/
/*!
@brief Rotates the motor clockwise by setting DIR_PIN1 to HIGH, DIR_PIN2 to LOW, and setting the PWM signal to the specified value
@param pwm The PWM value to set (0-255)
@return none
*/
/************************************************************************************/
void rotateMotorClockwise(uint8_t pwm) {
  digitalWrite(DIR_PIN1, HIGH);
  digitalWrite(DIR_PIN2, LOW);
  analogWrite(PWM_PIN, pwm);
  digitalWrite(STBY, HIGH);
}

/************************************************************************************/
/*!
@brief Rotates the motor counterclockwise by setting DIR_PIN1 to LOW, DIR_PIN2 to HIGH, and setting the PWM signal to the specified value
@param pwm The PWM value to set (0-255)
@return none
*/
/************************************************************************************/
void rotateMotorCounterclockwise(uint8_t pwm) {
  digitalWrite(DIR_PIN1, LOW);
  digitalWrite(DIR_PIN2, HIGH);
  analogWrite(PWM_PIN, pwm);
  digitalWrite(STBY, HIGH);
}

/************************************************************************************/
/*!
@brief Moves the motor to the position specified in REGISTER_SET_POSITION
@return none
*/
/************************************************************************************/
void moveMotorToPosition() {
  uint8_t targetPosition = registers[REGISTER_SET_POSITION];
  uint8_t currentPosition = registers[REGISTER_ANALOG_VALUE];
  uint8_t pwm = 0;

  while (currentPosition != targetPosition) {
    currentPosition = registers[REGISTER_ANALOG_VALUE];
    int16_t difference = targetPosition - currentPosition;
    if (abs(difference) > 30) {
      pwm = 255;  // Full speed
    } else if (abs(difference) > 10) {
      pwm = 128;  // Medium speed
    } else {
      pwm = 64;  // Low speed
    }

    if (currentPosition < targetPosition) {
      rotateMotorClockwise(pwm);
    } else if (currentPosition > targetPosition) {
      rotateMotorCounterclockwise(pwm);
    }

    updateAnalogValue();
  }

  stopMotor();
}
