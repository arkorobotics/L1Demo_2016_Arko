#include "xc.h"
#include "audio.h"
#include "music.h"

void config_timer(void) 
{
	PR1 = 0xFF;
	_T1IP = 5;	// set interrupt priority
	_TON  = 1;	// turn on the timer
	_T1IF = 0;	// reset interrupt flag
	_T1IE = 1;	// turn on the timer1 interrupt
}

//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
	static unsigned short sample_1 = 0;
	static unsigned short ch1_idx = 0;

	static unsigned short sample_2 = 0;
	static unsigned short ch2_idx = 0;

	static unsigned short idx = 0;
	static unsigned short duration = 0;
	
	// CHANNEL 1
	if(ch1_idx > (song[idx])>>1)
	{
		if(song[idx] != 0)
		{
			sample_1 = ~sample_1;
		}
		ch1_idx = 0;
	}	
	else
	{
		ch1_idx += 1;
	}

	// CHANNEL 2
	if(ch2_idx > (song_bass[idx])>>1)
	{
		if(song_bass[idx] != 0)
		{
			sample_2 = ~sample_2;
		}
		ch2_idx = 0;
	}	
	else
	{
		ch2_idx += 1;
	}

	// DURATION
	if(duration < 0x3d09)
	{
		duration++;
	}
	else
	{
		idx++;

		if(idx == sizeof(song) / sizeof(song[0]) ) /* loop it! */
		{
			idx = 0;
		}

		duration = 0;
	}

	if(duration%2 == 0)
	{
		PORTB = sample_1;
	}
	else
	{
		PORTB = sample_2>>2;
	}
	
	_T1IF = 0;
}

