# Arduino-Time-Lapse
An Arduino project to create a simple time-lapse dolly

## Components List:
* Arduino Nano
* EasyDriver Stepper Board
* 20x4 LCD with I2C board
* Stepper Motor
* 5 buttons
* Speaker

## Wiring Diagram
* Maybe one day

## Usage
* Move the stepper to start location, location A
* Set Location A
* Move the stepper to end location, location B
* Set Location B
* Set duration of time-lapse in hours, minutes, or seconds
* See overview of time-lapse
* Begin time-lapse

## Setting up Build Environment
* Install [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* Clone this repo in the Arudino folder
* Open the Library Manager in Arduino IDE: Tools -> Manager Libraries...
* Install the LiquidCrystal_I2C library by Frank de Brabander V1.1.2
* Open code/time_lapse/time_lapse.ino
