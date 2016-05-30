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

	static unsigned short sample_3 = 0;
	static unsigned short ch3_idx = 0;

	static unsigned short sample_4 = 0;
	static unsigned short ch4_idx = 0;

	static unsigned int idx = 450;

	static unsigned short duration = 0;

	// CHANNEL 1
	if(ch1_idx > (song_ch1[idx]))
	{
		if(song_ch1[idx] != 1)
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
	if(ch2_idx > (song_ch2[idx]))
	{
		if(song_ch2[idx] != 1)
		{
			sample_2 = ~sample_2;
		}
		ch2_idx = 0;
	}	
	else
	{
		ch2_idx += 1;
	}
	
	// CHANNEL 3
	if(ch3_idx > (song_ch3[idx]))
	{
		if(song_ch3[idx] != 1)
		{
			sample_3 = ~sample_3;
		}
		ch3_idx = 0;
	}	
	else
	{
		ch3_idx += 1;
	}

	// CHANNEL 4
	if(ch4_idx > (song_ch4[idx]))
	{
		if(song_ch4[idx] != 1)
		{
			sample_4 = ~sample_4;
		}
		ch4_idx = 0;
	}	
	else
	{
		ch4_idx += 1;
	}
	
	// DURATION
	if(duration < 0x7A1)
	{
		duration++;
	}
	else
	{
		idx++;

		if(idx == sizeof(song_ch1) / sizeof(song_ch1[0]) ) /* loop it! */
		{
			idx = 0;
		}

		duration = 0;
	}

	// SET SAMPLE OUTPUT
	/*
	if(duration%4 == 0)
	{
		sample = sample_1;
	}
	else if(duration%3 == 0)
	{
		sample = sample_2; //>>1;
	}
	else if(duration%2 == 0)
	{
		sample = sample_3; //>>2;
	}
	else
	{
		sample = sample_4; //>>3;
	}
	*/
	
	// MIX AND SET AUDIO OUTPUT
	PORTB = (sample_1>>2)+(sample_2>>2)+(sample_3>>2); //+(sample_4>>3);

	_T1IF = 0;
}

