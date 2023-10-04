# ESE Engineering Project 5 Mobile Robot

## Description
This project is part of Conestoga Colleges Electronic Systems Engineering degree 
program. The project builds on the previous project where we designed our own power 
and OBC PCBs and then wrote all the code to bring the mobile robot platform to life.  

The goal of this project is to communicate wirelessly with our robot via a raspberry 
pi and wifi. The robot can then be controlled via a joystick controller and doesn't 
have to be teathered to a computer.

## Setup
Run the setup.sh script to download and setup all project dependencies.

## Build
- Run 'make' to compile to project.
- Run 'make clean' to remove compiled files.
- Run 'make flash' to flash the STM32 device.
- Run 'make rebuild' to compile project from scratch.
- Run 'make reflash' to compile project from scratch and flash the STM32 device.