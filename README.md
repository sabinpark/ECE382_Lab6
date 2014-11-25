ECE382_Lab6
===========
Pulse Width Modulation (PWM) - "Robot Motion"

## Lab Overview
* Use the pulse-width modulation features of the MSP430. 
* Program the MSP430 to generate pulse-width-modulated waveforms to control the speed / direction of the robot's motors. 
* Make the robot move (minimum requirement):
  * Forward
  * Backward 
  * Small (< 45 degree) turn left/right
  * Large (> 45 dgree) turn left/right

### Functionality Update
|Functionality|Status|Date|Instructor Check|
|:-:|:-:|:-:|:-:|
|Required|Complete|21 NOV 2014|Capt Trimble and Dr. Coulston 24 Nov 14 (6th period)|
|A|Complete|24 NOV 2014|Capt Trimble and Dr. Coulston 24 Nov 14 (6th period)|

## Prelab

### Schematic
**UPDATE SCHEMATIC WITH PROPER CHANGES**
![alt test](https://github.com/sabinpark/ECE382_Lab6/blob/master/images/overall_schematic.jpg "overall schematic")

Considering the hardware (timer subsystems, chip pinout, etc.)...
* How will use the hardware (timer subsystems, chip pinout, etc.) to achieve robot control?
  * I will use the timer subsystems and PWM to send the appropriate signals to the motor driver chip. 
* Which pins will output which signals you need? 
  * LEFT Motor:
    * P2.0 - enable
    * P2.1 - motor direction
  * RIGHT Motor:
    * P2.5 - enable
    * P2.3 - motor direction
  * Given Dr. Coulston's code and diagram for the LEFT motor, I then arbritrarily chose the pins for the RIGHT motor to match
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
    * For whatever direction the robot moves for one configuration (let's say it moves CCW), simply make the motors run in the opposite direction to get the other direction (CW), and vice versa.

Consider how you will set up the PWM subsytem to achieve this control...
* What are the registers you'll need to use? 
  * The register corresponding to port 2 SELECT
* Which bits in those registers are important?
  * I will need to use bits 2 and 4 (2 for the LEFT motor and 4 for the RIGHT motor)
* What's the initialization sequence you'll need?
  * ```P2SEL |= BIT2```
  * ```P2SEL |= BIT4```

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

### Required Functionality

#### Beginning Approach
For required functionality, I started off with Dr. Coulston's class-provided diagram and .c file. I hooked up my robot according to the prelab circuit diagram shown above and then ran Dr. Coulston's base code. 

This in itself rotated my LEFT motor CCW (counter-clockwise). Just to experiment, I connected the enabler of the LEFT motor with the RIGHT motor. This resulted in the RIGHT motor spinning CW at the same rate as the LEFT motor. 

I then proceeded to hook up the appropriate *enable* pins to my RIGHT motor. Now both motors ran independently of each other. 
Here is an image of how I hooked up my robot's circuit:

[INSERT PICTURE OF BREADBOARD]

For simplicity, I started off with `TA1CCR0` of 100 and corresponding `TA1CCR1` and `TA1CCR2` values of 50. This gave me a 50% duty cycle and I did not have to worry about flipping the outmod.

#### Controlling the motors
Next, I wanted to make my motors rotate in the reverse direction. Initially, I struggled to get any results at all and could not even get my motors to stop running despite taking out its enable connections. Thus, I knew that there was a problem within the code itself. I originally believed that toggling P2SEL would turn the motor ON and OFF. After all, it seemed to be responding to the toggles. However, after talking with Dr. Coulston and referencing the datasheets for the MSP430, I realized that P2SEL simply chose which type of function that will be used for P2. Unlike the other pins which were set to be GPIO pins, P2SEL had to be specifically designated and associated with the Timer A functions.

Now back to my solution for the motor directions...
Lo and behold, I eventually discovered that I needed to set the P2DIR (with the corresponding BIT) to an output before I could manipulate that particular bit. 

```
// motor enables
P2DIR |= BIT0	 // sets direction pin to output
P2DIR |= BIT5	 // sets direction pin to output

// motor directions
P2DIR |= BIT1	 // sets direction pin to output
P2DIR |= BIT3	 // sets direction pin to output
```

The above code simply sets P2DIR to a 1, which corresponds with OUTPUT. After setting these bits to read as an output, I was then able to adjust my motor's enable signal and direction.

For my enable signals, 0 meant disable whereas 1 meant enable. 
```
// enable
P2OUT |= BIT0	// enable left motor
P2OUT |= BIT5	// enable right motor

// disable
P2OUT &= ~BIT0	// disable left motor
P2OUT &= ~BIT5	// disable right motor
```

For my motor directions, 0 meant CW and 1 meant CCW.
```
// LEFT motor
P2OUT &= ~BIT1 // left motor turns clockwise
P2OUT |= BIT1  // left motor turuns counter-clockwise

// RIGHT motor
P2OUT &= ~BIT3	// right motor turns clockwise
P2OUT |= BIT3	 // right motor turns counter-clockwise
```

*NOTE*: You can hard-wire your robot's motors to turn in the reverse direction by simply switching the black and red inputs for a particular motor. This reverse the voltage difference and forces the current to flow in the opposite direction. Not too surprising, but interesting to know.

After successfully getting my motors to turn in both directions, I then played around with the delay and began to compose my robot's dance routi...I mean, its required functionality demonstration.

#### Creating methods for the required movements

At the minimum, my robot needed to demonstrate that it can move forward, backward, and turn less than 45 degrees and greater than 45 degrees in each direction. Simple.

I initially created the following methods:
* moveForward()
* moveBackward()
* rotateLeft()
* rotateRight()
* stop()

These methods are fairly self-explanatory, only requiring a few lines of code to enable the motors and choose which directions the motors turn based on where you want it to go. Here's the moveForward() method:
```
void moveForward() {

 P2OUT |= BIT0	 // enable left motor
 P2OUT |= BIT5	 // enable right motor
 
 P2OUT |= BIT1	 // left motor turuns counter-clockwise
 P2OUT &= ~BIT3	// right motor turns clockwise
}
```

Since duty cycles for each motor was at 50%, I did not have to worry about adjusting the out_mod. One thing that did catch my attention was the fact that the robot would perform all of its maneuvers instantaneously and then move onto the next instruction. This resulted in a crazy, jerky robot that seemed high on battery juice... So I temporarily decommissioned it until it learned to control its movements with more class. The solution lies within the use of the delay.

```
		_delay_cycles(SOME_VALUE);
```

I played around with various delay values and inserted this one-liner between each movement function call. After a bit of tweaking, the robot proceeded back onto the dance floor and performed the proper movements much smoother than before. Here is the required functionality manuever:
```
// Required Functionality Maneuver (without remote controller)
    	moveForward(1);
    	stop();
    	moveBackward(1);
    	stop();
    	rotateRight(45);
    	stop();
    	rotateLeft(45);
    	stop();
    	rotateLeft(45);
    	stop();
    	rotateRight(45);
    	stop();
    	rotateRight(120);
    	stop();
    	rotateLeft(120);
    	stop();
    	rotateLeft(120);
    	stop();
    	rotateRight(120);
    	stop();
```

#### Cleaning up the code

At this point I realized that my code was really messy and could be improved greatly in terms of readability and efficiency. First, I made various #define statements that translated the not-as-clear bit statements into more readable one-liners. For instance, instead of always calling `P2OUT |= BIT0` to enable to left motor, I would instead type `LEFT_ENABLE`. Here is the actual #define statement used for this particular example:
```
#define LEFT_ENABLE					P2OUT |= BIT0	// enable left motor
```

Although I had to create a lot of these definitions, it was definitely worth it in the end (and while continuing to improve my code) because it made debugging so much easier.

Inspired by C2



### A Functionality

## Debugging
### Required Functionality (debugging)
### A Functionality (debugging)

## Documentation
None
