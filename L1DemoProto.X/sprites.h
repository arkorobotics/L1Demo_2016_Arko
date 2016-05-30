#ifndef SPRITES_H 
#define SPRITES_H 

#define MAX_PARTICLES 30

#define SPR_HEAD 5
#define SPR_OFF_WIDTH 0
#define SPR_OFF_HEIGHT 1
#define SPR_OFF_BITRES 2
#define SPR_OFF_TRANS 3
#define SPR_OFF_ROTATE 4
#define SPR_OFF_DATA 5

// SPRITE DEFINITIONS

#define MAX_SPRITES 11

#define SPR_FIRE_16_A 0
#define SPR_FIRE_16_B 1
#define SPR_FIRE_8_A 2
#define SPR_FIRE_8_B 3

#define SPR_INT_LAYER 4
#define SPR_INT_ONE 5
#define SPR_INT_BORD_A 6
#define SPR_INT_BORD_B 7

struct Sprite {
	 uint8_t width;  // Width (in pixels)
	 uint8_t height; // Height (in pixels)
	 uint8_t bitres; // Bits per Pixel
 	 uint8_t trans;  // Transparency
 	 uint8_t rotate; // Rotation, 0: none, 1: 90 cw, 2: 180, 3: 90 ccw
	__prog__ uint8_t *data;  // Pointer to sprite pixel data
};

struct Particle
{
    uint16_t size;
    uint16_t posx;
    uint16_t posy;
    uint16_t speedx;
    uint16_t speedy;
    uint16_t color;
};

extern struct Sprite s[MAX_SPRITES];

extern __eds__ struct Particle p[MAX_PARTICLES];
extern int numPart;

void loadAllSprites(void);
void inline drawSprite(uint16_t x, uint16_t y, uint8_t id, uint8_t rotation);
int inline nrange(double a, double b);
void drawSpriteRotation(uint16_t x, uint16_t y, uint8_t id, float rotation);
void addParticle(void);


#endif
