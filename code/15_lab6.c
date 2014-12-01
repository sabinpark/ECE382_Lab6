/*
 * ir_sensor.h
 *
 *  Created on: Nov 24, 2014
 *      Author: C16Sabin.Park
 */

#include <msp430g2553.h>
#include "ir_sensor.h"

// IR defined constants
int8	newIrPacket = FALSE;	// flag to check if there is a new IR packet
int16	packetData[48];			// array to hold packet data
int8	packetIndex = 0;
int32	irPacket;

int		direction = FORWARD;

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

//	WDTCTL=WDTPW+WDTHOLD; 					// stop WD

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
//    	moveForward(1);
//    	stop();
//    	moveBackward(1);
//    	stop();
//    	rotateRight(45);
//    	stop();
//    	rotateLeft(45);
//    	stop();
//    	rotateLeft(45);
//    	stop();
//    	rotateRight(45);
//    	stop();
//    	rotateRight(120);
//    	stop();
//    	rotateLeft(120);
//    	stop();
//    	rotateLeft(120);
//    	stop();
//    	rotateRight(120);
//    	stop();

    	// A Functionality (with remote controller)
		if(newIrPacket) {

			_disable_interrupt();
			newIrPacket = FALSE;

			switch(irPacket) {
				case BTN_PWR:		// Signal ready to use
					GREEN_ON;
					RED_ON;
					break;
				case BTN_2:			// Move Forward
					direction = FORWARD;
					moveForward(1);
					break;
				case BTN_4:			// Rotate LEFT
					rotateLeft(45);
					checkDirection(direction);
					break;
				case BTN_5:			// Stop
					direction = STOP;
					stop();
					break;
				case BTN_6:			// Rotate RIGHT
					rotateRight(45);
					checkDirection(direction);
					break;
				case BTN_8:			// Move Backward
					direction = BACKWARD;
					moveBackward(1);
					break;
				case BTN_1:			// Move NW
					driveNW();
					checkDirection(direction);
					break;
				case BTN_3:			// Move NE
					driveNE();
					checkDirection(direction);
					break;
				case BTN_7:			// Move SW
					driveSW();
					checkDirection(direction);
					break;
				case BTN_9:			// Move SE
					driveSE();
					checkDirection(direction);
					break;
				case BTN_0:			// continuously rotates CCW
					rotateLeft(45);
					break;
				case BTN_DASH:		// continuously rotates CW
					rotateRight(45);
					break;
				case BTN_CH_HI:		// increase duty cycle
					increaseDC();
					break;
				case BTN_CH_LO:		// decrease duty cycle
					decreaseDC();
					break;
				case BTN_OK:		// resets the duty cycle back to normal
					resetSignal();
				default:
					// do nothing
					break;
			}

			initMSP430();
		} // end A functionality

    } // end loop
} // end main

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
	P1DIR |= BIT0 | BIT6;				// Enable updates to the LED
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

