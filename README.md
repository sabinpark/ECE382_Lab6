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
  * Voltage regulator: this will be used to convert the 5V (from Vdd) to 3.3V used to power the MSP430
  * Motor Driver Chip (SN754410): this will be used to convert the signals from the MSP430 and then amplify it for use with the motors
  * ...
* Which pins will output which signals you need? 
  * Motor A:
    * P1.0
    * P1.1
  * Motor B:
    * P2.0
    * P2.1
  * These pins were chosen arbitrarily
* Which side of the motor will you attach these signals to? 
  * It does not matter--but I will keep the sides consistent for both motors
* How will you use these signals to achieve forward / back / left / right movement?
  * Forward:
   * Run the motors in opposite directions
   * Since the motors are facing each other, this this will result in a forward movement
  * Backward:
   * Switch the power source and ground for both motors from *Forward*
   * This is essentially the opposite of the *Forward* movement
  * Left/Right:
   * Run the motors in the same direction
   * This results in the robot turning in place like a tank
   * For whatever direction the robot moves for one configuration (let's say it moves LEFT), simply make the motors run in the opposite direction to get the other direction (RIGHT), and vice versa.

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
