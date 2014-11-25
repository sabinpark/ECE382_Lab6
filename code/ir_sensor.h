/*
 * ir_sensor.h
 *
 *  Created on: Nov 24, 2014
 *      Author: C16Sabin.Park
 */

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE				1
#define		FALSE				0

#define		FORWARD				1
#define		STOP				0
#define		BACKWARD			-1

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6

#define		averageLogic0Pulse	544
#define		averageLogic1Pulse	1615
#define		averageStartPulse	4340
#define		minLogic0Pulse		averageLogic0Pulse - 100
#define		maxLogic0Pulse		averageLogic0Pulse + 100
#define		minLogic1Pulse		averageLogic1Pulse - 100
#define		maxLogic1Pulse		averageLogic1Pulse + 100
#define		minStartPulse		averageStartPulse - 100
#define		maxStartPulse		averageStartPulse + 100

// constants for the remote control buttons
#define		BTN_PWR		0x30DFA857		//
#define		BTN_2		0x30DF609F		// Forward
#define		BTN_4		0x30DF10EF		// Rotate Left
#define		BTN_5		0x30DF906F		// Stop
#define		BTN_6		0x30DF50AF		// Rotate Right
#define		BTN_8		0x30DF30CF		// Backward
#define		BTN_CH_HI	0x30DF40BF		// Increase duty cycle
#define		BTN_CH_LO	0x30DFC03F		// Decrease duty cycle
#define		BTN_1		0x30DFA05F		// turn NW
#define		BTN_3		0x30DFE01F		// turn NE
#define		BTN_7		0x30DFD02F		// turn SW
#define		BTN_9		0x30DFB04F		// turn SE
#define		BTN_0		0x30DF20DF		// continuous CW rotation
#define		BTN_DASH	0x30DF6C93		// continuous CCW rotation
#define		BTN_OK		0x30DF18E7		//

// constants for motor delays
#define SHORT_DELAY					5000000
#define LONG_DELAY					15000000

#define DELAY_360					9000000			// the delay it takes for (approximately) 360 degrees of rotation
#define DELAY_180					4800000			// for (appx) 180 degrees
#define DELAY_90					2700000			// for (appx) 90 degrees
#define DELAY_45					1400000			// for (appx) 45 degrees
#define DELAY_15					800000			// for (appx) 15 degrees

#define DEG_360						360				// constant to define 360 degrees

// sets the PWN direction bits to output
#define LEFT_PWN_ENABLE_SET_OUTPUT	P2DIR |= BIT2
#define RIGHT_PWN_ENABLE_SET_OUTPUT	P2DIR |= BIT4
// chooses the proper function for use with the timers
#define LEFT_P2SEL_SET				P2SEL |= BIT2
#define RIGHT_P2SEL_SET				P2SEL |= BIT4

// left motor enable
#define LEFT_ENABLE_SET_OUTPUT		P2DIR |= BIT0	// sets direction pin to output
#define LEFT_ENABLE					P2OUT |= BIT0	// enable left motor
#define LEFT_DISABLE				P2OUT &= ~BIT0	// disable left motor
// right motor enable
#define RIGHT_ENABLE_SET_OUTPUT		P2DIR |= BIT5	// sets direction pin to output
#define RIGHT_ENABLE				P2OUT |= BIT5	// enable right motor
#define RIGHT_DISABLE				P2OUT &= ~BIT5	// disable right motor

// left motor direction
#define LEFT_SET_OUTPUT				P2DIR |= BIT1	// sets direction pin to output
#define LEFT_SET_CW					P2OUT &= ~BIT1	// left motor turns clockwise
#define	LEFT_SET_CCW				P2OUT |= BIT1	// left motor turuns counter-clockwise
// right motor direction
#define RIGHT_SET_OUTPUT			P2DIR |= BIT3	// sets direction pin to output
#define RIGHT_SET_CW				P2OUT &= ~BIT3	// right motor turns clockwise
#define RIGHT_SET_CCW				P2OUT |= BIT3	// right motor turns counter-clockwise

// left motor outmode
#define LEFT_OM_HI					TA1CCTL1 = OUTMOD_3;
#define LEFT_OM_LO					TA1CCTL1 = OUTMOD_7;
// right motor outmode
#define RIGHT_OM_HI					TA1CCTL2 = OUTMOD_3;
#define RIGHT_OM_LO					TA1CCTL2 = OUTMOD_7;

#define GREEN_ON					P1OUT |= BIT6
#define GREEN_OFF					P1OUT &= ~BIT6
#define RED_ON						P1OUT |= BIT0
#define RED_OFF						P1OUT &= ~BIT0
#define LEDS_OFF					P1OUT &= ~(BIT0 | BIT6)

/*
 * stops the motors
 */
void stop() {

	LEFT_DISABLE;
	RIGHT_DISABLE;

	LEDS_OFF;

	_delay_cycles(SHORT_DELAY);
}

/*
 * enables the left and right PWMs
 */
void go() {
	LEFT_ENABLE;
	RIGHT_ENABLE;
}

/*
 * moves the robot forward
 */
void moveForward(int dist_delay) {

	go();

	LEFT_OM_HI;
	RIGHT_OM_HI;

	// LEFT MOTOR
	LEFT_SET_CCW;	// set the direction of left motor (CCW)

	// RIGHT MOTOR
	RIGHT_SET_CW;

	GREEN_ON;						// green LED ON
	RED_OFF;						// red LED OFF

	int i;

	switch(dist_delay) {
	case 1:
		_delay_cycles(SHORT_DELAY);
		break;
	case 100:
		_delay_cycles(LONG_DELAY);
		break;
	default:
		for(i=0; i<dist_delay; i++) {
			_delay_cycles(SHORT_DELAY);
		}
		break;
	}
}

/*
 * moves the robot backward
 */
void moveBackward(int dist_delay) {
	go();

	LEFT_OM_LO;
	RIGHT_OM_LO;

	// LEFT MOTOR
	LEFT_SET_CW;

	// RIGHT MOTOR
	RIGHT_SET_CCW;

	RED_ON;							// red LED ON
	GREEN_OFF;						// green LED OFF

	int i;

	switch(dist_delay) {
	case 1:
		_delay_cycles(SHORT_DELAY);
		break;
	case 100:
		_delay_cycles(LONG_DELAY);
		break;
	default:
		for(i=0; i<dist_delay; i++) {
			_delay_cycles(SHORT_DELAY);
		}
		break;
	}
}

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

	RED_ON;							// red LED ON
	GREEN_OFF; 						// green LED OFF

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

/*
 * turns the robot CW
 */
void rotateRight(int deg_delay) {
	go();

	LEFT_OM_HI;
	RIGHT_OM_LO;

	// LEFT MOTOR
	LEFT_SET_CCW;
	// RIGHT MOTOR
	RIGHT_SET_CCW;

	GREEN_ON;						// green LED ON
	RED_OFF;						// red LED OFF

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

/*
 * NW turn
 */
void driveNW() {
	go();

	// LEFT MOTOR
	LEFT_SET_CCW;
	LEFT_OM_LO;

	// RIGHT MOTOR
	RIGHT_SET_CW;
	RIGHT_OM_HI;

	RED_ON;							// red LED ON
	GREEN_OFF;						// green LED OFF

	_delay_cycles(LONG_DELAY/2);
}

/*
 * NE turn
 */
void driveNE() {
	go();

	// LEFT MOTOR
	LEFT_SET_CCW;
	LEFT_OM_HI;

	// RIGHT MOTOR
	RIGHT_SET_CW;
	RIGHT_OM_LO;

	GREEN_ON;						// green LED ON
	RED_OFF;						// red LED OFF

	_delay_cycles(LONG_DELAY/2);
}

/*
 * SW turn
 */
void driveSW() {
	go();

	// LEFT MOTOR
	LEFT_SET_CW;
	LEFT_OM_HI;

	// RIGHT MOTOR
	RIGHT_SET_CCW;
	RIGHT_OM_LO;

	RED_ON;							// red LED ON
	GREEN_OFF;						// green LED OFF

	_delay_cycles(LONG_DELAY/2);
}

/*
 * SE turn
 */
void driveSE() {
	go();

	// LEFT MOTOR
	LEFT_SET_CW;
	LEFT_OM_LO;

	// RIGHT MOTOR
	RIGHT_SET_CCW;
	RIGHT_OM_HI;

	GREEN_ON;						// green LED ON
	RED_OFF;						// red LED OFF

	_delay_cycles(LONG_DELAY/2);
}

/*
 * updates the timer signal lengths
 */
void updateSignal() {
	TA1CCR2 = TA1CCR0-TA1CCR1;
}

/*
 * increase the duty cycle
 */
void increaseDC() {
	if(TA1CCR1 < 90) {
		TA1CCR1 += 5;
	}
	updateSignal();

	GREEN_ON;						// green LED ON
	RED_OFF;						// red LED OFF
}

/*
 * decrease the duty cycle
 */
void decreaseDC() {
	if(TA1CCR1 > 40) {
		TA1CCR1 -= 5;
	}
	updateSignal();

	GREEN_OFF;						// green LED OFF
	RED_ON	;						// red LED ON
}

/*
 * reset the duty cycle back to the original
 */
void resetSignal() {
	TA1CCR1 = 60;
	TA1CCR2 = TA1CCR0-TA1CCR1;

	GREEN_ON;						// green LED ON
	RED_ON;							// red LED ON
}

void checkDirection(int direction) {
	if(direction == FORWARD) {
		moveForward(1);
	} else if(direction == BACKWARD) {
		moveBackward(1);
	} else {
		stop();
	}
}
