# prop-bomb
This repo contains the control software and build instructions for a device that simulates the look of a briefcase style bomb/remote detonator. 

🚨 WARNING! Don't do anything illegal! Check the laws in your jurisdiction before building or transporting this device! 🚨  
⚠️ CAUTION! Included code is an older copy and is out of date with final specs listed below. ⚠️

Use cases:
* Escape rooms
* Paintball CTF
* Employee team building
* Family game night

## How it works
[]()


### Prerequisites
* Install or reset the "cut wires" before turning on device.

### Initialization
1. Device is powered on by the keyed switch
1. "Cut wires" are detected
1. Arduino pins are set
1. hd44780 displays setup prompt
1. Device waits for key input to start setup

### Setup
1. User selects "cut wire"
1. User sets 4 digit pin for keypad
1. User sets timer (1-99 minute(s)) 



### Features
* Tamper proof keyed power switch
* Fully(almost) configurable options on start up
* Cut a wire just like in the movies 📽
* Three "lives"
* Sassy eccentric dialog

## How it's made

### Software
Built in Arduino C. 

### Hardware

#### Electrical
<!-- add board revision model -->
* Arduino Mega 2560 
* Hitachi 16x4 LCD
* 4 digit timer LCD
* Belkin
* 10 key keypad
* 4x cherry long throw rocker switches
* 4x buttons off an old shirt or jacket
* Belkin USB power bank
* LED Rocker Switch
* Larson Scanner Kit
* 1x Red LED
* 1x Green LED



## Future Optimizations
This is some of the messiest code I have ever written. 🍝 

Companion website/app to generate storyline with presets for game logic. 

### Finale

Larger LCD for countdown timer. 
Battery system to allow higher current draw for a vibrating motor on "detonation"
Upgraded sound system (louder and with wav file playback support.)
A modular repeatable design for production units.
Intrusion switch detection.
Use off the shelf i2c solutions to simplify layout and cabl

## Installation
Download all files and libraries and flash to Arduino over USB.
