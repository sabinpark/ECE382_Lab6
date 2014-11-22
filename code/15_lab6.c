#include <msp430.h>

#define SHORT_DELAY					500000
#define LONG_DELAY					2000000
#define DELAY_360					1470000
#define FULL_ROTATION				360

#define LEFT_ENABLE_SET_OUTPUT		P2DIR |= BIT0
#define LEFT_ENABLE					P2OUT |= BIT0
#define LEFT_DISABLE				P2OUT &= ~BIT0

#define RIGHT_ENABLE_SET_OUTPUT		P2DIR |= BIT5
#define RIGHT_ENABLE				P2OUT |= BIT5
#define RIGHT_DISABLE				P2OUT &= ~BIT5

#define LEFT_SET_OUTPUT				P2DIR |= BIT1
#define LEFT_SET_CW					P2OUT &= ~BIT1
#define	LEFT_SET_CCW				P2OUT |= BIT1

#define RIGHT_SET_OUTPUT			P2DIR |= BIT3
#define RIGHT_SET_CW				P2OUT &= ~BIT3
#define RIGHT_SET_CCW				P2OUT |= BIT3

/*
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
    WDTCTL = WDTPW|WDTHOLD;                 // stop the watchdog timer

    // LEFT MOTOR PWM is OUTPUT and set to the TIMER mode
    P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
    P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1
    // RIGHT MOTOR PWN is INPUT and set to the TIMER mode
    P2DIR |= BIT4;							// P2.4 is associated with TA1CCR2
    P2SEL |= BIT4;							// P2.4 is associated with TA1CCTL2

    // set the initial direction of the motors (FORWARD)
    // LEFT
    LEFT_SET_OUTPUT;	// set P2DIR BIT1 as an output
    LEFT_SET_CCW;		// CCW
    // RIGHT
    RIGHT_SET_OUTPUT;	// set P2DIR BIT3 as an output
    RIGHT_SET_CW;		// CW

    // disable the enablers (NO MOVEMENT)
    // LEFT
    LEFT_ENABLE_SET_OUTPUT;		// set P2DIR BIT0 as an output
    LEFT_DISABLE;				// ensures that the enable is set to 0 (disable)
    // RIGHT
    RIGHT_ENABLE_SET_OUTPUT;	// same as above
    RIGHT_DISABLE;

    // settings
	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
    TA1CCR0 = 100;							// set signal period

    TA1CCR1 = 50;
    TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

    TA1CCR2 = 50;
    TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

    while (1) {
    	moveForward();
    	stop();
    	moveBackward();
    	stop();
    	rotateRight(180);
    	stop();
    	rotateLeft(180);
    	stop();
    	rotateLeft(180);
    	stop();
    	rotateRight(180);
    	stop();
    	rotateRight(360);
    	stop();
    	rotateLeft(360);
    	stop();
    	rotateLeft(360);
    	stop();

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

	// LEFT MOTOR
	LEFT_SET_CCW;	// set the direction of left motor (CCW)

	// RIGHT MOTOR
	RIGHT_SET_CW;

	_delay_cycles(LONG_DELAY);
}

void moveBackward() {
	go();

	// LEFT MOTOR
	LEFT_SET_CW;

	// RIGHT MOTOR
	RIGHT_SET_CCW;

	_delay_cycles(LONG_DELAY);
}

void rotateRight(int deg) {
	go();

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
