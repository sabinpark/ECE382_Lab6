#include <msp430g2553.h>
//#include <msp430.h>
#include "ir_sensor.h"

#define SHORT_DELAY					500000
#define LONG_DELAY					1000000
#define DELAY_360					1470000			// the delay it takes for (approximately) 360 degrees of rotation
#define FULL_ROTATION				360				// constant to define 360 degrees

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


// IR defined constants
int8	newIrPacket = FALSE;	// flag to check if there is a new IR packet
int16	packetData[48];			// array to hold packet data
int8	packetIndex = 0;
int32	irPacket;


/* PIN USES
 *
 * PWM LEFT:		P2.2
 * PWM RIGHT:		P2.4
 *
 * ENABLE LEFT:		P2.0
 * ENABLE RIGHT:	P2.5
 *
 * DIR LEFT:		P2.1
 * DIR RIGHT:		P2.3
 *
 * P2OUT with BITX is used to turn something ON or OFF
 */

void main(void) {
	initMSP430();							// Setup MSP to process IR and buttons

//	WDTCTL=WDTPW+WDTHOLD; 				// stop WD

    // LEFT MOTOR PWM is OUTPUT and set to the TIMER mode
	LEFT_PWN_ENABLE_SET_OUTPUT;				// P2.2 is associated with TA1CCR1
	LEFT_P2SEL_SET;							// P2.2 is associated with TA1CCTL1		// selects the primary peripheral module function
    // RIGHT MOTOR PWN is INPUT and set to the TIMER mode
    RIGHT_PWN_ENABLE_SET_OUTPUT;			// P2.4 is associated with TA1CCR2
    RIGHT_P2SEL_SET;						// P2.4 is associated with TA1CCTL2

    // set the initial direction of the motors (FORWARD)
    // LEFT
    LEFT_SET_OUTPUT;						// set P2DIR BIT1 as an output
    LEFT_SET_CCW;							// CCW
    // RIGHT
    RIGHT_SET_OUTPUT;						// set P2DIR BIT3 as an output
    RIGHT_SET_CW;							// CW

    // disable the enablers (NO MOVEMENT)
    // LEFT
    LEFT_ENABLE_SET_OUTPUT;					// set P2DIR BIT0 as an output
    LEFT_DISABLE;							// ensures that the enable is set to 0 (disable)
    // RIGHT
    RIGHT_ENABLE_SET_OUTPUT;				// same as above
    RIGHT_DISABLE;

    // SETTINGS
	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
    TA1CCR0 = 100;							// set signal period
    // LEFT motor
    TA1CCR1 = 60;
    TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode
    // RIGHT motor
    TA1CCR2 = TA1CCR0-TA1CCR1;
    TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

    // infinite loop
    while (1) {

    	// Required Functionality Maneuver (without remote controller)
//    	moveForward();
//    	stop();
//    	moveBackward();
//    	stop();
//    	rotateRight(45);
//    	stop();
//    	rotateLeft(45);
//    	stop();
//    	rotateLeft(45);
//    	stop();
//    	rotateRight(45);
//    	stop();
//    	rotateRight(90);
//    	stop();
//    	rotateLeft(90);
//    	stop();
//    	rotateLeft(90);
//    	stop();
//    	rotateRight(90);
//    	stop();

    	// A Functionality (with remote controller)
		if(newIrPacket) {

			_disable_interrupt();
			newIrPacket = FALSE;

			switch(irPacket) {
				case POWER:
					break;
				case UP_1:					// 2
					// green LED ON
					P1OUT |= BIT6;
					// red LED OFF
					P1OUT &= ~BIT0;
					moveForward();
					break;
				case LEFT_1:				// 4
					// red LED ON
					P1OUT |= BIT0;
					// green LED OFF
					P1OUT &= ~BIT6;
					rotateLeft(190);
			    	_delay_cycles(LONG_DELAY);
					stop();
					break;
				case SELECT:				// 5
					stop();
					break;
				case RIGHT_1:				// 6
					// green LED ON
					P1OUT |= BIT6;
					// red LED OFF
					P1OUT &= ~BIT0;
					rotateRight(290);
					//stop();
					break;
				case DOWN_1:				// 8
					// red LED ON
					P1OUT |= BIT0;
					// green LED OFF
					P1OUT &= ~BIT6;
					moveBackward();
					break;
				case UP_2:					// CH +
					// red LED ON
					P1OUT |= BIT0;
					break;
				case DOWN_2:				// CH -
					// red LED OFF
					P1OUT &= ~BIT0;
					break;
				default:
					// do nothing
					break;
			}

			//irPacket = 0;

			initMSP430();
		} // end A functionality

    	_delay_cycles(LONG_DELAY);
    } // end loop
} // end main

void stop() {

	LEFT_DISABLE;
	RIGHT_DISABLE;

	_delay_cycles(SHORT_DELAY);
}

// enables left and right PWM
void go() {
	LEFT_ENABLE;
	RIGHT_ENABLE;
}

void moveForward() {

	go();

	LEFT_OM_HI;
	RIGHT_OM_HI;

	// LEFT MOTOR
	LEFT_SET_CCW;	// set the direction of left motor (CCW)

	// RIGHT MOTOR
	RIGHT_SET_CW;

	_delay_cycles(LONG_DELAY);
}

void moveBackward() {
	go();

	LEFT_OM_LO;
	RIGHT_OM_LO;

	// LEFT MOTOR
	LEFT_SET_CW;

	// RIGHT MOTOR
	RIGHT_SET_CCW;

	_delay_cycles(LONG_DELAY);
}

void rotateRight(int deg) {
	go();

	LEFT_OM_HI;
	RIGHT_OM_LO;

	// LEFT MOTOR
	LEFT_SET_CCW;
	// RIGHT MOTOR
	RIGHT_SET_CCW;

	int i;

	for(i=0; i<deg; i++) {
		_delay_cycles(DELAY_360/FULL_ROTATION);
	}
}

void rotateLeft(int deg) {
	go();

	LEFT_OM_LO;
	RIGHT_OM_HI;

	// LEFT MOTOR
	LEFT_SET_CW;
	// RIGHT MOTOR
	RIGHT_SET_CW;

	int i;

	for(i=0; i<deg; i++) {
		_delay_cycles(DELAY_360/FULL_ROTATION);
	}
}

void turnRight() {
}

void turnLeft() {

}


// -----------------------------------------------------------------------
// In order to decode IR packets, the MSP430 needs to be configured to
// tell time and generate interrupts on positive going edges.  The
// edge sensitivity is used to detect the first incoming IR packet.
// The P2.6 pin change ISR will then toggle the edge sensitivity of
// the interrupt in order to measure the times of the high and low
// pulses arriving from the IR decoder.
//
// The timer must be enabled so that we can tell how long the pulses
// last.  In some degenerate cases, we will need to generate a interrupt
// when the timer rolls over.  This will indicate the end of a packet
// and will be used to alert main that we have a new packet.
// -----------------------------------------------------------------------
void initMSP430() {

	IFG1=0; 							// clear interrupt flag1
	WDTCTL=WDTPW+WDTHOLD; 				// stop WD

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P2SEL  &= ~BIT6;					// Setup P2.6 as GPIO not XIN
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;						// Clear any interrupt flag
	P2IE  |= BIT6;						// Enable PORT 2 interrupt on pin change

	HIGH_2_LOW;
//	P1DIR |= BIT0 | BIT6;				// Enable updates to the LED
//	P1OUT &= ~(BIT0 | BIT6);			// And turn the LED off

	TA0CCR0 = 0x8000;					// create a 16mS roll-over period
	TACTL &= ~TAIFG;					// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:1 presclar off MCLK and enable interrupts

	_enable_interrupt();
}

// -----------------------------------------------------------------------
// Since the IR decoder is connected to P2.6, we want an interrupt
// to occur every time that the pin changes - this will occur on
// a positive edge and a negative edge.
//
// Negative Edge:
// The negative edge is associated with end of the logic 1 half-bit and
// the start of the logic 0 half of the bit.  The timer contains the
// duration of the logic 1 pulse, so we'll pull that out, process it
// and store the bit in the global irPacket variable. Going forward there
// is really nothing interesting that happens in this period, because all
// the logic 0 half-bits have the same period.  So we will turn off
// the timer interrupts and wait for the next (positive) edge on P2.6
//
// Positive Edge:
// The positive edge is associated with the end of the logic 0 half-bit
// and the start of the logic 1 half-bit.  There is nothing to do in
// terms of the logic 0 half bit because it does not encode any useful
// information.  On the other hand, we going into the logic 1 half of the bit
// and the portion which determines the bit value, the start of the
// packet, or if the timer rolls over, the end of the ir packet.
// Since the duration of this half-bit determines the outcome
// we will turn on the timer and its associated interrupt.
// -----------------------------------------------------------------------
#pragma vector = PORT2_VECTOR			// This is from the MSP430G2553.h file

__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;			// The timer is 16-bits

	if (IR_PIN)		pin=1;	else pin=0;

	switch (pin) {					// read the current pin level
		case 0:						// !!!!!!!!!NEGATIVE EDGE!!!!!!!!!!
			pulseDuration = TAR;    // read TAR

			if ((pulseDuration >= minLogic1Pulse) && (pulseDuration <= maxLogic1Pulse)){
				irPacket = (irPacket<<1) | 1;
			}
			if ((pulseDuration >= minLogic0Pulse) && (pulseDuration <= maxLogic0Pulse)){
					irPacket = (irPacket<<1) | 0;
			}

			packetData[packetIndex++] = pulseDuration;     // store TAR in packet Data
				TACTL = 0;
			LOW_2_HIGH; 			// Setup pin interrupt on positive edge  // enable positive edge interrupt
			break;

		case 1:						// !!!!!!!!POSITIVE EDGE!!!!!!!!!!!
			// set TAR = 0
			TAR = 0x0000;			// time measurements are based at time 0

			// turn Timer A on
				//TACTL |= MC_0;
			TACTL &= ~TAIFG;					// clear flag before enabling interrupts = good practice
			TACTL = ID_3 | TASSEL_2 | MC_1 | TAIE;		// Use 1:1 presclar off MCLK and enable interrupts

			HIGH_2_LOW; 			// Setup pin interrupt on neg. edge  // enable neg. edge interrupt
			break;

	} // end switch

	P2IFG &= ~BIT6;			// Clear the interrupt flag to prevent immediate ISR re-entry

} // end pinChange ISR



// -----------------------------------------------------------------------
//			0 half-bit	1 half-bit		TIMER A COUNTS		TIMER A COUNTS
//	Logic 0	xxx
//	Logic 1
//	Start
//	End
//
// -----------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR			// This is from the MSP430G2553.h file
__interrupt void timerOverflow (void) {

	// turn off Timer A interrupt
	TACTL = 0;

	// clear PacketIndex
	packetIndex = 0;

	// set newPacket flag
	newIrPacket = TRUE;

	TACTL &= ~TAIFG;      // clear the flag
}

