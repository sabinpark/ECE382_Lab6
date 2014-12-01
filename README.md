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
 P2OUT &= ~BIT3		// right motor turns clockwise
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
#define LEFT_ENABLE	P2OUT |= BIT0	// enable left motor
```

Although I had to create a lot of these definitions, it was definitely worth it in the end (and while continuing to improve my code) because it made debugging so much easier.

I input a parameter in my functions that would allow my movement functions to delay for a user-designated amount of time. For instance, the rotateLeft() method may take in the parameter value of 180. The robot will rotate counter-clockwise with a delay that corresponds with the 180 degree rotation. Here is the rotateLeft() method:

```
/*
 * turns the robot CCW
 */
void rotateLeft(int deg_delay) {
	go();

	LEFT_OM_LO;
	RIGHT_OM_HI;

	// LEFT MOTOR
	LEFT_SET_CW;
	// RIGHT MOTOR
	RIGHT_SET_CW;

	RED_ON;			// red LED ON
	GREEN_OFF; 		// green LED OFF

	int i;

	switch(deg_delay) {
	case 15:
		_delay_cycles(DELAY_15);
		break;
	case 45:
		_delay_cycles(DELAY_45);
		break;
	case 90:
		_delay_cycles(DELAY_90);
		break;
	case 180:
		_delay_cycles(DELAY_180);
		break;
	case 360:
		_delay_cycles(DELAY_360);
		break;
	default:
		for(i=0; i<deg_delay; i++) {
		_delay_cycles(DELAY_360/DEG_360);
		}
	}
}
```

I created this function with the appropriate delays based on the delay required for a full 360 degree rotation. I picked arbitrary values for the delay, debugged, and then tested the delay with the robot for both directions. Rinse and repeat until satisfied. One thing I noticed was that the imperfections of the robot caused the rotations to be different every time. One rotation may have been be slightly greater than 360 degrees, another may have been slightly less, and some were pretty much exactly 360 degrees. Since I could not get the precise 360 degrees that I wanted, I was satisfied enough with if the robot's rotations averaged 360 degrees in the long run. Using the 360 degrees delay as the basis of the function, I used simple algebra to rotate the robot various other degrees based on the user input.

And of course, I ran into another problem. It turned out that the delay changes were not proportional to the change in degrees (input parameter). Therefore, if a *1000000* delay worked fine for 360 degrees, a delay of *500000* did not necessarily give me the 180 degrees I expected. In general, I got back less degrees than expected.

I countered this problem by creating some case-switch statements with predefined and precallibrated delay values for a couple of important degree rotations. Specifically, I wanted my robot to be able to rotate 360, 180, 90, 45, and 15 degrees. I ended the case-switch statement with the original implementation of the function, which allows the user to input any value (other than the predefined ones mentioned above) and have the robot turn appropriately. I realized that this last part was not going to be too accurate, but I decided that there was no immediate downside of including this little bit of code. On the contrary, I could use this for various purposes such as having my robot rotate for a very long duration of time (but not infinitely) before stopping. 

In the end, my robot successfully achieved the required functionality.

### A Functionality

A functionality required me to control my robot using an IR remote controller. At a minimum, I needed to make the robot move forward, backward, turn right, and turn left. I decided to use the same controller from the last lab especially since I already had the IR codes for the various buttons I would be using to control the robot. I used the APEX remote #8.

All I had to do for the A functionality was to copy and paste my code from the previous lab. I retrieved code for initiating the MSP430 and the code for the interrupts.

I hooked up the IR sensor just like in the previous lab as well.

[INSERT SCHEMATIC OF IR SENSOR]

With the proper connections and coding, I built the code and ran the debugger. As expected, the robot read in the IR packets and moved corresponding to the the remote controller input. However, I did have a slight problem with the consistency of responses. Only about 1 out of 10 button inputs seemed to be reading correctly. Trying two different solutions, I was eventually able to solve this problem (for details please go to the debugging section).

In the end, my robot did achieve A functionality with smooth and responsive control inputs from the remote controller.

## Debugging
### Required Functionality (debugging)
I initially had an issue with the coding aspect of this lab in that no matter how much I changed the direction of the motors, I was not getting any change. Furthermore, when I tried to disable the motors, the motors kept on rotating. Messing around with the code, I discovered that I had not initialized the P2OUT bits to 1 (1 = output). Because of this, the appropriate bits did not output any of the changes I made earlier. After initializing the necessary bits as outputs, I was then able to change the direction of the wheels and toggle the enablers as well. 

### A Functionality (debugging)
One of the main issues I had was that the robot was not consistently receiving the IR packets sent from the remote controller. 

Believing that the issue was with the noise generated by the motors, I added in more capacitors and even an extra IR sensor (facing the opposite direction of the first IR sensor). This seemed to help out slightly since the robot responded once or twice more than expected. However, I knew that there was a bigger problem that had nothing to do with the noise.

Messing around with the code, I changed the way my robot received its data packets as such:
```
	if ((pulseDuration >= minLogic1Pulse) && (pulseDuration <= maxLogic1Pulse)){
		irPacket = (irPacket<<1) | 1;
	}
	if ((pulseDuration >= minLogic0Pulse) && (pulseDuration <= maxLogic0Pulse)){
		irPacket = (irPacket<<1) | 0;
	}
```

This seemed to work well and my robot became much more responsive. The real problem was that the initial code did not allow the IR sensor to properly read in the IR packet data.

## Documentation
#### Prelab
* None
#### Lab
* C2C Arneberg gave me the idea to create a parameter in my movement functions that would allow me to have more flexibility in determining the magnitude of the movement.
* Dr. Coulston helped me identify an issue with the code that reads in the data packets using the remote controller; he also helped me realize that my issues with the IR sensor was not necessarily caused by noise from the motors
