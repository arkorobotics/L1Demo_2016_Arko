/*
 Event>>  			Layerone Demo Party 2016
 Title>>  			!S!p!a!c!e!
 By>>     			Arko
 Base Libraries>>	Datagram & Hotdogs
 [ INSERT ASCII ART HERE ]
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "gpu.h"
#include "audio.h"
#include "music.h"
#include "sprites.h"
 
#define  FCY    16000000UL    // Instruction cycle frequency, Hz
#include <libpic30.h>
 
#pragma config FWDTEN = OFF , GWRP = OFF , GCP = OFF , JTAGEN = OFF
#pragma config POSCMOD = HS , FCKSM = CSDCMD , FNOSC = PRIPLL , PLL96MHZ = ON , PLLDIV = DIV2
#pragma config ALTPMP = ALTPMPEN , SOSCSEL = EC

int main(void) 
{
    // Configuation
	ANSB = 0x0000;
	ANSC = 0x0000;
	ANSD = 0x0000;
	ANSF = 0x0000;
	ANSG = 0x0000;
	TRISB = 0x0000;

	_VMRGNIF = 0;
	_VMRGNIE = 1;
	_GFX1IE = 1;

	config_graphics();
	config_chr();
	config_timer();
	clearbuffers();
 
    // Load and Initialize
    int d;
	for(d = 0; d < MAX_PARTICLES; d++)
		addParticle();
        
	loadAllSprites();
	
	uint8_t aa = 1;
	
    // Draw
	while (1) 
	{
		swapBuffers();  // Before drawing the next frame, we must swap buffers
		
		// DRAW FRAME
        //----------------------------------------------------------------------
		int c;
		for(c = 0; c < numPart; c++)
		{
            if (p[c].posx + p[c].size >= HOR_RES-1) {
                p[c].posx = rand()%5;
                p[c].posy = 1+(rand()%(VER_RES-6));
                p[c].color = rand() & 0xff;
            }
            p[c].posx += p[c].speedx;
        }
        for(c = 0; c < numPart; c++)
        {
                rcc_color(p[c].color);
                fast_pixel(p[c].posx, p[c].posy);
        }
		
        drawSprite(HOR_RES/2-s[6].width/2, VER_RES/2-(s[6].height*PIX_H), 6,0);
		drawSprite(HOR_RES/2-s[7].width/2, VER_RES/2, 7,0);
        drawSprite(HOR_RES/2-s[7].width/2 - s[2].width - 1, VER_RES/2 + PIX_H*(s[2].width/2), 2+aa, 0);
		drawSprite(HOR_RES/2+s[7].width/2 + 2, VER_RES/2 + PIX_H*(s[3].width/2), 2+!aa, 0);
        if ( frames%4 == 0) 
        {
			aa = !aa;
		}
        //----------------------------------------------------------------------
        
		drawBorder(0x92);       // Draw border around demo
		cleanup();              // Housekeeping for VGA signaling
		waitForBufferFlip();    // For next vsync
		frames++;               // Increment frame count
	}

	return 0;
}
