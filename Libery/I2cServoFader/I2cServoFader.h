#ifndef I2C_SERVO_FADER_H
#define I2C_SERVO_FADER_H

#include <Arduino.h>
#include <Wire.h>

class I2cServoFader {
public:
    /************************************************************************************/
    /*!
    @brief Register addresses for I2C communication
    */
    /************************************************************************************/
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

    /************************************************************************************/
    /*!
    @brief Constructor for I2cServoFader class
    @param uint8_t I2C address of the device
    */
    /************************************************************************************/
    I2cServoFader(uint8_t i2cAddress);

    /************************************************************************************/
    /*!
    @brief Initialize the I2C communication
    @return none
    */
    /************************************************************************************/
    void begin();

    /************************************************************************************/
    /*!
    @brief Set the Color of the LED in RGB
    @param uint8_t 0-255 Red
    @param uint8_t 0-255 Green
    @param uint8_t 0-255 Blue
    @return none
    */
    /************************************************************************************/
    void setColor(uint8_t red, uint8_t green, uint8_t blue);

    /************************************************************************************/
    /*!
    @brief Set the brightness of the LED
    @param uint8_t 0-255 Brightness
    @return none
    */
    /************************************************************************************/
    void setBrightness(uint8_t brightness);

    /************************************************************************************/
    /*!
    @brief Turn the LED on or off
    @param bool true to turn on, false to turn off
    @return none
    */
    /************************************************************************************/
    void setOnOff(bool on);

    /************************************************************************************/
    /*!
    @brief Read a value from a register
    @param Register Register to read from
    @return uint8_t Value read from the register
    */
    /************************************************************************************/
    uint8_t readRegister(Register reg);

    /************************************************************************************/
    /*!
    @brief Read the analog value from the analog value register
    @return uint8_t Value read from the analog value register
    */
    /************************************************************************************/
    uint8_t readAnalogValue();

    /************************************************************************************/
    /*!
    @brief Read the touch state from the touch register
    @return uint8_t Value read from the touch register (0 or 1)
    */
    /************************************************************************************/
    uint8_t readTouch();

    /************************************************************************************/
    /*!
    @brief Read the state of the OPTIO pin
    @return uint8_t State of the OPTIO pin (0 or 1)
    */
    /************************************************************************************/
    uint8_t readOptio();

    /************************************************************************************/
    /*!
    @brief Set the target position of the servo
    @param uint8_t Position (0-255)
    @return none
    */
    /************************************************************************************/
    void setPosition(uint8_t position);

private:
    /************************************************************************************/
    /*!
    @brief Write a value to a register
    @param Register Register to write to
    @param uint8_t Value to write
    @return none
    */
    /************************************************************************************/
    void setRegister(Register reg, uint8_t value);

    uint8_t _i2cAddress; //!< I2C address of the device
};

#endif
