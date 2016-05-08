/*
 Event>>  Layerone Demo Party 2016
 Title>>  !S!p!a!c!e!
 By>>     Arko
 [ INSERT ASCII ART HERE ]
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "gpu.h"
#include "music.h"
#include "sprites.h"
 
#define  FCY    16000000UL    // Instruction cycle frequency, Hz
#include <libpic30.h>
 
#pragma config FWDTEN = OFF , GWRP = OFF , GCP = OFF , JTAGEN = OFF
#pragma config POSCMOD = HS , FCKSM = CSDCMD , FNOSC = PRIPLL , PLL96MHZ = ON , PLLDIV = DIV2
#pragma config ALTPMP = ALTPMPEN , SOSCSEL = EC

void config_timer() 
{
	PR1 = 0;
	_T1IP = 5;	// set interrupt priority
	_TON  = 1;	// turn on the timer
	_T1IF = 0;	// reset interrupt flag
	_T1IE = 1;	// turn on the timer1 interrupt

	/* set up timer for stepping through song */
	PR2 = 0x3d09; // reaaal sloooow
	_T2IP = 6;
	_T2IF = 0;
	/* no nice macros for T2CON :( */
	T2CON = 0b1000000000110000; // set T2 on with max prescaler (256)
	_T2IE = 1;
}

//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__)) _T1Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
	static unsigned char idx = 0;
	PORTB = sinetable[idx++];
	//idx += 1;
	_T1IF = 0;
}

//_T2Interrupt() is the T2 interrupt service routine (ISR).
void __attribute__((__interrupt__)) _T2Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
	static unsigned short idx = 0;
	PR1 = song[idx++];

	//idx++;
	if(idx == sizeof(song) / sizeof(song[0]) ) /* loop it! */
	{
		idx = 0;
	}
	_T2IF = 0;
}


int main(void) 
{
 
	ANSB = 0x0000;
	ANSC = 0x0000;
	ANSD = 0x0000;
	ANSF = 0x0000;
	ANSG = 0x0000;
	TRISB = 0x0000;
 
	config_graphics();
	config_chr();
	config_timer();
	
 
	// clear buffers
	rcc_setdest(GFXDisplayBuffer[0]);
	blank_background();
	rcc_setdest(GFXDisplayBuffer[1]);
	blank_background();
 
	_VMRGNIF = 0;
	_HMRGNIF = 0;
	_HMRGNIE = 1;
	_VMRGNIE = 1;
	_GFX1IE = 1;
 
	loadAllSprites();

	int next_fb = 1;

	while (1) 
	{
		rcc_setdest(GFXDisplayBuffer[next_fb]);
 
		blank_background();

		// DRAW HERE
		rcc_color(rand());
		fast_pixel(rand() % (HOR_RES-2) , 1+(rand() % (VER_RES-7)));

		rcc_color(0);

		drawBorder(0x92);
		cleanup();

		while(!_CMDMPT) continue;  // Wait for GPU to finish drawing
		gpu_setfb(GFXDisplayBuffer[next_fb]);
		_VMRGNIF = 0;
		while(!_VMRGNIF) continue; // wait for vsync
		next_fb = !next_fb;
		frames++;
	}

	return 0;
}
