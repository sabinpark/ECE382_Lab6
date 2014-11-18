ECE382_Lab6
===========
Pulse Width Modulation (PWM) - "Robot Motion"

## Lab Overview
* Use the pulse-width modulation features of the MSP430. 
* Program the MSP430 to generate pulse-width-modulated waveforms to control the speed / direction of the robot's motors. 
* Make the robot move:
  * Forward
  * Backward 
  * Small (< 45 degree) turn left/right
  * Large (> 45 dgree) turn left/right

## Prelab

Considering the hardware (timer subsystems, chip pinout, etc.)...
* How will use the hardware (timer subsystems, chip pinout, etc.) to achieve robot control?
* Which pins will output which signals you need? 
* Which side of the motor will you attach these signals to? 
* How will you use these signals to achieve forward / back / left / right movement?

Consider how you will setup the PWM subsytem to achieve this control...
* What are the registers you'll need to use? 
* Which bits in those registers are important?
* What's the initialization sequence you'll need?

Consider what additional hardware you'll need (regulator, motor driver chip, decoupling capacitor, etc.) and how you'll configure / connect it.

Consider the interface you'll want to create to your motors. 
* Do you want to move each motor invidiually (moveLeftMotorForward()) or do you want to move them together (moveRobotForward())?

## Lab

## Documentation
None
