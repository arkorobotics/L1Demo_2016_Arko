#include "xc.h"
#include "audio.h"
#include "music.h"

void config_timer(void) 
{
	PR1 = 0;
	_T1IP = 5;	// set interrupt priority
	_TON  = 1;	// turn on the timer
	_T1IF = 0;	// reset interrupt flag
	_T1IE = 1;	// turn on the timer1 interrupt

	/* set up timer for stepping through song */
	PR2 = 0x3d09;
	_T2IP = 6;
	_T2IF = 0;
	T2CON = 0b1000000000110000; // set T2 on with max prescaler (256)
	_T2IE = 1;
}

//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
	static unsigned char idx = 0;
	PORTB = sinetable[idx++];
	_T1IF = 0;
}

//_T2Interrupt() is the T2 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
	static unsigned short idx = 0;
	PR1 = song[idx++];

	if(idx == sizeof(song) / sizeof(song[0]) ) /* loop it! */
	{
		idx = 0;
	}
	_T2IF = 0;
}
