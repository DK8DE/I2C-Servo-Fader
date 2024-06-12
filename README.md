# I2C-Servo-Fader 
## Description
This project consists of a board for a motor fader, the code for the microcontroller on the board, and a library to control this fader with an Arduino or an ESP32. The compatible faders are:
+ Behringer X-Touch Motor-Fader MF100T
+ Soundwell SM10001NKA0X-HA1
+ RSA0N11M9A0J
Of course, the board can also be adapted for other faders by adjusting the footprint.

### Connections on the Board:
* SDA
* SCL
* +5V for the microcontroller
* 8-10V for the motor
* 1x interrupt pin
* 1x Opt button
* 1x Neo-Pixel-LED
* 1x UPDI

All signals are looped through on the board. This means that several boards can be placed next to each other and easily connected to each other.

## Funktion
The board also has the option to control a NeoPixel LED and an additional button. The NeoPixel LED can be fully controlled via the I2C bus, including RGB color, on, off, and brightness. This allows, for example, changing the color based on the set level or displaying notifications.
The fader has a touch function that can also be evaluated and is internally used to trigger an interrupt. You only need to check when the interrupt line is pulled LOW.
The optional button input can be queried and also triggers an interrupt when pressed.
The interrupt is only reset when the value on the fader that caused the interrupt is read. This ensures that no unnecessary queries are made and no changes are lost.
In the example code, the interrupt line is connected to a pin on the microcontroller that can also trigger an interrupt. This ensures that the microcontroller responds immediately to changes. On an Arduino UNO or Nano, pins 2 and 3 can be used.
To set an I2C address for the fader, you need to connect to the fader using the serial console and simply enter the address in the format 0x08. The address is then stored in ROM and used on the next startup. In theory, you can operate 127 faders on one bus ;-).
The functions of the library are thoroughly documented in I2cServoFader.h, and all functions are used in the example code. The example code also shows how to control multiple sliders.

# Example Code Description
This example code demonstrates how to control I2C servo sliders using an Arduino. The code supports one slider and can be extended to two sliders by uncommenting the relevant sections. The sliders can be controlled via serial commands, and their positions and LED colors can be adjusted based on analog values.

### Serial Commands:
+ sliderpos1=<value>: Sets the position of slider 1 to the specified value (0-255).
+ sliderpos2=<value>: Sets the position of slider 2 to the specified value (0-255).
+ slidertest=1: Initiates a demonstration of slider 1's movement through a series of positions.
+ slidertest=2: Initiates a demonstration of slider 2's movement through a series of positions.

### Interrupt Handling:
When an interrupt occurs, the code reads the analog value and optio state from the sliders, prints these values to the serial console, and updates the LED colors based on the analog values.
The interrupt is triggered by a change in the state of the connected pin (e.g., pin 2).

### LED Color Representation:
+ Green (analog value = 0)
+ Yellow (analog value ≈ 128)
+ Red (analog value = 255)
The LED color transitions smoothly between these values based on the analog value.
# Sources
Part of the circuit board layout is from this project:
https://github.com/GadgetReboot/PSM60_Fader_Controller

# Partlist
| Part | Value | Device | Package | 
|:--- |:--- |:--- |:--- | 
| +5V | 150080RS75000 | 150080RS75000 | LEDC2012X80N | 
| +9V | 150080RS75000 | 150080RS75000 | LEDC2012X80N | 
| C1 | 100nF | C0805C104K1RACTU | C0805 | 
| C2 | 10nF | C0805Y103K4RACTU | C0805 | 
| C3 | 100nF | C0805C104K1RACTU | C0805 | 
| C4 | 22nF | C0805C333K4RACTU | C0805 | 
| C5 | 100nF | C0805C104K1RACTU | C0805 | 
| C6 | 100nF | C0805C104K1RACTU | C0805 | 
| C7 | 10uF | EEE-HB1C100R | EEE1AA220WR | 
| C8 | 10uF | EEE-HB1C100R | EEE1AA220WR | 
| IC1 | ATTINY804-SSNR | ATTINY804-SSNR | SOIC127P600X175-14N | 
| IC2 | AT42QT1010 | AT42QT1010-TSHR | SOT95P280X145-6N | 
| IC3 | L293DD | L293DD | SOIC127P1032X265-20N | 
| IC4 | SN74LVC1G14DBVR | SN74LVC1G14DBVR | SOT95P280X145-5N | 
| IO1 | 0 | MA06-1 | MA06-1 | 
| IO2 | 0 | MA06-1 | MA06-1 | 
| LB-LED | 150080GS75000 | 150080GS75000 | LEDM2012X80N | 
| NEO | 0 | MA03-1 | MA03-1 | 
| R1 | 1K | R-EU_R0805 | R0805 | 
| R2 | 1K | R-EU_R0805 | R0805 | 
| R3 | 100K | R-EU_R0805 | R0805 | 
| R4 | 10K | R-EU_R0805 | R0805 | 
| R5 | 1K | R-EU_R0805 | R0805 | 
| R6 | 1K | R-EU_R0805 | R0805 | 
| R7 | 1K | R-EU_R0805 | R0805 | 
| R9 | 470R | R-EU_R0805 | R0805 | 
| TOUCH | 150080YS75000 | 150080YS75000 | LEDC2012X80N | 
| U$1 | MF100T | SM10001NKA0X-HA1 | SM10001NKA0X-HA1 | 
| UPDI | 0 | MA03-1 | MA03-1 | 
| X1 | 22-23-2021 | 22-23-2021 | 22-23-2021 | 
