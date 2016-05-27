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
    /*
    int d;
	for(d = 0; d < MAX_PARTICLES; d++)
		addParticle();
    */

	loadAllSprites();
	
	uint8_t x_trans = 0;
	uint8_t y_trans = 20;
	uint16_t time = 0;

	char buf[2];
	char greets[] = "          GREETS TO CHARLIEX ~ COINE ~ DATAGRAM ~ FSPHIL ~ JKING ~ JAMIS ~ M M C A ~ MR 1337357                ";
    // Draw
	while (1) 
	{
		swapBuffers();  // Before drawing the next frame, we must swap buffers
		
		// DRAW FRAME
        //----------------------------------------------------------------------
		/*
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
		*/
		
		if(time >= 0 && time < 512)
		{
			drawSprite(17, (sinetable[y_trans]>>9)+10, 9,1);
			sprintf(buf, "FSOCIETY");
			chr_print(buf,22,(sinetable[y_trans]>>9)+10+300);
			y_trans++; 
		}
		else if(time >= 512 && time < 1024)
		{
			drawSprite(17, (sinetable[y_trans]>>9)+10, 10,0);

			static uint8_t i = 1;
			static uint8_t s = 0;
			for(i = 1; i < 16; i++)
			{
				//sprintf(buf, greets[i]);
				buf[0] = greets[i+s];
  				buf[1] = '\0';
				chr_print(buf,i*5,(sinetable[(uint8_t)(y_trans+(i*2))]>>9)+10+300);
			}
			
			y_trans++; 
			

			if(time%5 == 0)
			{
				s++;
			}
			
			if(s > 105)
			{
				s = 0;
				i = 1;
			}
		}

		
        //drawSprite(HOR_RES/2-s[6].width/2, VER_RES/2-(s[6].height*PIX_H), 6,0);
		//drawSprite(HOR_RES/2-s[7].width/2, VER_RES/2, 7,0);
        //drawSprite(HOR_RES/2-s[7].width/2 - s[2].width - 1, VER_RES/2 + PIX_H*(s[2].width/2), 2+aa, 0);
		//drawSprite(HOR_RES/2+s[7].width/2 + 2, VER_RES/2 + PIX_H*(s[3].width/2), 2+!aa, 0);
		time++;

        //----------------------------------------------------------------------
        
		//drawBorder(0x92);       // Draw border around demo
		cleanup();              // Housekeeping for VGA signaling
		waitForBufferFlip();    // For next vsync
		frames++;               // Increment frame count
	}

	return 0;
}
