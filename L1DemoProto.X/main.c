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
 
_CONFIG1(FWDTEN_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_HS & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2)
_CONFIG3(ALTPMP_ALTPMPEN & SOSCSEL_EC)

struct Sprite {
	 uint8_t width;  // Width (in pixels)
	 uint8_t height; // Height (in pixels)
	 uint8_t bitres; // Bits per Pixel
 	 uint8_t trans;  // Transparency
 	 uint8_t rotate; // Rotation, 0: none, 1: 90 cw, 2: 180, 3: 90 ccw
__prog__ uint8_t *data;  // Pointer to sprite pixel data
};
 
struct Sprite s[MAX_SPRITES];

void loadAllSprites(void) {
	uint16_t id, off;
	off = 0;

	for (id=0; id < MAX_SPRITES; id++) {
		s[id].width  =  SpriteMap[off + SPR_OFF_WIDTH];
		s[id].height =  SpriteMap[off + SPR_OFF_HEIGHT];
		s[id].bitres =  SpriteMap[off + SPR_OFF_BITRES];
		s[id].trans  =  SpriteMap[off + SPR_OFF_TRANS];
		s[id].rotate =  SpriteMap[off + SPR_OFF_ROTATE];
		s[id].data   = &SpriteMap[off + SPR_OFF_DATA];

		off += s[id].width * s[id].height + SPR_HEAD;
	}
}
 
void static inline drawSprite(uint16_t x, uint16_t y, uint8_t id, uint8_t rotation) {
 
	unsigned int w,h;
	uint16_t x1,y1;
	uint8_t color;

	if (x >= HOR_RES-1 || y >= VER_RES-1 || x <= 0|| y <= 0) return;

	for (h=0; h < s[id].height; h++) {
		for (w=0; w < s[id].width; w++) {
			color = s[id].data[w + s[id].width*h];
			// don't draw if it matches transparency color
			if (color == s[id].trans) continue;
			rcc_color(color);
			//rcc_color(rand()); tv screen effect

			switch(rotation) {
				//  00 deg      0,0 1,0 2,0 ... 0,1
				//  90 deg CCW  7,0 7,1 7,2 ... 6,0
				// 180 deg      7,7 6,7 5,7 ... 7,6
				//  90 deg CW   0,7 0,6 0,5 ... 1,6
				case 0: // 0 degree
					x1 = x+w;
					y1 = y + (h<<2) + (h<<1);//y+(PIX_H*h);
					if (x1 >= HOR_RES-2) continue; //br
					if (y1 >= VER_RES-PIX_H) return; //ret
					fast_pixel(x1, y1);
					break;
				case 1: // 90 degree CW
					x1 = x+(s[id].width-h-1);
					y1 = y+(PIX_H*(w));
					if (x1 >= HOR_RES-1 || x1 <= 0) continue;
					if (y1 >= VER_RES-PIX_H || y1 <= 0) continue;
					fast_pixel(x1, y1);
					break;
				case 2: // 180 degree CW
					x1 = x+(s[id].width-w-1);
					y1 = y+(PIX_H*(s[id].height-h-1));
					if (x1 >= HOR_RES-1) continue;
					if (y1 >= VER_RES-PIX_H) continue;
					fast_pixel(x1, y1);
					break;
				case 3: // 90 degree CCW
					break;
				default:
					break;
			}
		}
	}
	//Nop();
}

int static inline nrange(double a, double b) {
	return (int)((a >= b) ? a-b : b-a);
}

static unsigned int shade = 0;

void drawSpriteRotation(uint16_t x, uint16_t y, uint8_t id, float rotation) {
	int x1,y1,x2,y2;
	unsigned int w,h, real_x, real_y;
	uint8_t color;
	float r_s,r_c;

	r_s = sin(rotation);
	r_c = cos(rotation);
 
	for (h=0; h < s[id].height; h++) {
		for (w=0; w < s[id].width; w++) {
			color = s[id].data[w + s[id].width*h];
			if (color == s[id].trans) continue;
			rcc_color(color+shade);

			x1 = -(s[id].width/2-w); // negative for origin centering 
			y1 = (s[id].height/2-h);
			x2 = x1*r_c - y1*r_s;
			y2 = x1*r_s + y1*r_c;

			real_x = x+nrange(x1,x2);
			real_y = y + PIX_H*nrange(y1,y2);

			if (real_x >= HOR_RES-1 || real_x <= 0) continue;
			if (real_y >= VER_RES-PIX_H || real_y <= 0) continue; // PIX_H for screen bordered setup
			//rcc_draw(real_x, real_y, 1, PIX_H);
			fast_pixel(real_x, real_y);
		}
	}
}

void config_timer() {
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
	PORTB = sinetable[idx] << 8;
	idx += 1;
	_T1IF = 0;
}

//_T2Interrupt() is the T2 interrupt service routine (ISR).
void __attribute__((__interrupt__)) _T2Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
	static unsigned short idx = 0;
	PR1 = song[idx];

	idx++;
	if(idx == sizeof(song) / sizeof(song[0])) /* loop it! */
		idx = 0;
	_T2IF = 0;
}

void verBlind(void) {
	// VERTICAL BLINDS
	// blind = rate of drop
	static uint16_t blind=0;
	rcc_color(0);
	rcc_draw(0, 0, HOR_RES-1, blind);
	rcc_draw(0, VER_RES-blind, HOR_RES-1, blind);
	if ( blind < 100) {
		blind+=2;
	}
}
/*
// FIRE SINE WAVE
balls = 0;
for (fire_x = -3.14159; fire_x <= 3.14159; fire_x+=0.1) {
	fire_y = amp*sin(fire_x*fire_b);

	if ( realtoint(fire_x, -3.14159, 3.14159, 0, HOR_RES-1) > HOR_RES-1) break;
	drawSprite(realtoint(fire_x, -3.14159, 3.14159, 0, HOR_RES-1), realtoint(fire_y, -1,1,0,480-(s[2].height*PIX_H)), 2, rand()%3);
	balls++;
}

if (max_fire > 100 || max_fire == -100) {
	fire_s *= -1;
}

if (amp >= 3.0 || amp < 0.5) {
	amp_s *= -1;
}

amp += amp_s;
max_fire += fire_s;
*/

#define MAX_PARTICLES 30

int numPart=0;
struct Particle
{
    uint16_t size;
    uint16_t posx;
    uint16_t posy;
    uint16_t speedx;
    uint16_t speedy;
    uint16_t color;
};

__eds__ struct Particle p[MAX_PARTICLES];

void addParticle()
{
    p[numPart].size = 1;//+(rand() % 5);
    p[numPart].posx = rand() % (HOR_RES-2);//rand()%5;//1+(rand() % (HOR_RES-10));
    p[numPart].posy = 1+(rand() % (VER_RES-7));
    p[numPart].speedx = 1+(rand() % 2);
    p[numPart].speedy = 0;//1+(rand() % 5);
    p[numPart].color = rand();
    numPart++;
}


char message[20];
uint8_t lol=0;

int main(void) {
 
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
 
	//loadSprite(0);
	loadAllSprites();

	int d;
	for(d = 0; d < MAX_PARTICLES; d++)
		addParticle();


	double fire_x, fire_y;
	uint16_t balls=0;
	signed int max_fire=0;
	uint16_t fire_s = 10;
	float fire_b = 1.0;
	// y = offset + A*sin(x*2*pi*B)

	float amp = 0.5;
	float amp_s = 0.05;
	uint16_t angle=0;

	uint8_t aa = 1;
	int next_fb = 1;
	int box_color = 0;
	while (1) {
		rcc_setdest(GFXDisplayBuffer[next_fb]);
 
		blank_background();
		//omar();
	
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
		if ( frames%4 == 0) {
			aa = !aa;
		}

		rcc_color(0);

		//if (frames < 200) {
		//	omar();
		//TODO: text fix
		//rcc_color(0x92);
		//rcc_draw(0, 210, 24, 42);
		//} else if (frames >= 200 && frames < 400) {
		//	verBlind();
		//} else if (frames >= 400) {
		//	drawSprite( 10, 60, 4, 0); // TROGDOOOOOR
		//}

		rcc_color(0x3);
		//render(angle, 360-angle, 0);
		//angle+=5;

/*
		// CIRCLE DRAWING
		int x, y;
		int ox,oy, radius;
		radius = 3;
		ox = HOR_RES/2;
		oy = VER_RES/2;
		for(y=-radius; y<=radius; y++) {
			for(x=-radius; x<=radius; x++) {
				rcc_color(1+(rand()%3));
				if(x*x+y*y <= radius*radius + radius*0.8f) fast_pixel(ox+x, oy+y*PIX_H);
				}
			}
*/

		drawBorder(0x92);
		cleanup();

		while(!_CMDMPT) continue; // Wait for GPU to finish drawing
		gpu_setfb(GFXDisplayBuffer[next_fb]);
		_VMRGNIF = 0;
		while(!_VMRGNIF) continue; // wait for vsync
		next_fb = !next_fb;
		frames++;
	}

	return 0;
}
