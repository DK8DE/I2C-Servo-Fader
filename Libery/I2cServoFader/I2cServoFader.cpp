#include "I2cServoFader.h"

I2cServoFader::I2cServoFader(uint8_t i2cAddress) {
    _i2cAddress = i2cAddress;
}

void I2cServoFader::begin() {
    Wire.begin();
}

void I2cServoFader::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    setRegister(REGISTER_RED, red);
    setRegister(REGISTER_GREEN, green);
    setRegister(REGISTER_BLUE, blue);
}

void I2cServoFader::setBrightness(uint8_t brightness) {
    setRegister(REGISTER_BRIGHTNESS, brightness);
}

void I2cServoFader::setOnOff(bool on) {
    setRegister(REGISTER_ON, on ? 1 : 0);
}

void I2cServoFader::setRegister(Register reg, uint8_t value) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(static_cast<uint8_t>(reg));
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t I2cServoFader::readRegister(Register reg) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(static_cast<uint8_t>(reg));
    Wire.endTransmission();
    
    Wire.requestFrom(static_cast<int>(_i2cAddress), static_cast<int>(1));
    if (Wire.available()) {
        return Wire.read();
    }
    return 0; // Return 0 if no value was read
}

uint8_t I2cServoFader::readAnalogValue() {
    return readRegister(REGISTER_ANALOG_VALUE);
}

uint8_t I2cServoFader::readTouch() {
    return readRegister(REGISTER_TOUCH);
}

uint8_t I2cServoFader::readOptio() {
    return readRegister(REGISTER_OPTIO_READ);
}

void I2cServoFader::setPosition(uint8_t position) {
    setRegister(REGISTER_SET_POSITION, position);
}
