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

### Functionality Update
|Functionality|Status|Date|
|:-:-|:-:|:-:|
|Required|Complete|21 November 2014|
|A|In Progress|--|

## Prelab

### Schematic
![alt test](https://github.com/sabinpark/ECE382_Lab6/blob/master/images/overall_schematic.jpg "overall schematic")

Considering the hardware (timer subsystems, chip pinout, etc.)...
* How will use the hardware (timer subsystems, chip pinout, etc.) to achieve robot control?
  * I will use the timer subsystems and PWM to send the appropriate signals to the motor driver chip. 
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
  * Voltage regulator: this will be used to convert the 5V (from Vdd) to 3.3V used to power the MSP430
  * Motor Driver Chip (SN754410): this will be used to convert the signals from the MSP430 and then amplify it for use with the motors
    * ![alt test](https://github.com/sabinpark/ECE382_Lab6/blob/master/images/motor_driver_schematic.PNG "motor driver chip")
  * Decoupling Capacitor: the motors may fluctuate voltages and induce noise on the 5V line, causing the microcontroller to reset. Thus, the capacitors will be used to:
    * ~100 uF across the 12V rail to supplement the current when the motor draw spikes
    * ~0.1 uF across the 5V rail to smooth high frequency noise
    * 10 pF between the RST pin and the GND to smooth noise to the RST pin (extremely sensitive to voltage fluctuations and may reset the MCU)
  * Motor
    * DC motor is a 2-terminal device
      * one terminal receives power
      * the other is grounded

Consider the interface you'll want to create to your motors. 
* Do you want to move each motor individually (moveLeftMotorForward()) or do you want to move them together (moveRobotForward())?
  * I plan to move the two motors together
  * The move methods I may need:
    * moveRobotForward()
    * moveRobotBackward()
    * turnSmallRight()
    * turnSmallLeft()
    * turnBigRight()
    * turnBigLeft()

## Lab

## Documentation
None
