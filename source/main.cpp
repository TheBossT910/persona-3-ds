/*---------------------------------------------------------------------------------

	$Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

	Simple console print demo
	-- dovoto


---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "background.h"

//a simple sprite structure
//it is generally preferred to separate your game object
//from OAM
typedef struct
{
   u16* gfx;
   SpriteSize size;
   SpriteColorFormat format;
   int rotationIndex;
   int paletteAlpha;
   int x;
   int y;
} MySprite;

volatile int frame = 0;

// fn for the interrupt
void Vblank() {
	frame++;
}
	
int main(void) {
	touchPosition touchXY;

	irqSet(IRQ_VBLANK, Vblank);

	// set video mode for 2 text layers and 2 extended background layers
	videoSetMode(MODE_5_2D);
	// set sub video mode for 4 text layers
	videoSetModeSub(MODE_0_2D);

	// map vram bank A to main engine background (slot 0)
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	// map vram bank B to main engine sprites (slot 0)
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// debug init
	consoleDemoInit();

	// set brightness on bottom screen to completely dark (no visible image)
	setBrightness(2, -16);

	// setup bitmap background (for background image) on background 3
	int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	dmaCopy(backgroundBitmap, bgGetGfxPtr(bg3), 256*256);
	dmaCopy(backgroundPal, BG_PALETTE, 256*2);
	
	// TODO: setup moon as sprite
	// creating 3 sprites with different color formats
	MySprite sprites[] = {
		{0, SpriteSize_32x32, SpriteColorFormat_Bmp, 0, 15, 20, 15},
		{0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 20, 80},
		{0, SpriteSize_32x32, SpriteColorFormat_16Color, 0, 1, 20, 136}
	};

	// initialize sub sprite engine with 1D mapping, 128 byte boundry, no external palette support
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	
	// allocating space for sprite graphics
	for(int i = 0; i < 3; i++)
      sprites[i].gfx = oamAllocateGfx(&oamMain, sprites[i].size, sprites[i].format);

	// fill bmp sprite with the color red
	dmaFillHalfWords(ARGB16(1,31,0,0), sprites[0].gfx, 32*32*2);
	// fill the 256 color sprite with index 1 (2 pixels at a time)
	dmaFillHalfWords((1<<8)|1, sprites[1].gfx, 32*32);
	// fill the 16 color sprite with index 1 (4 pixels at a time)
	dmaFillHalfWords((1<<12)|(1<<8)|(1<<4)|1, sprites[2].gfx, 32*32 / 2);

	// set index 1 to blue...this will be the 256 color sprite
	SPRITE_PALETTE[1] = RGB15(0,31,0);
	// set index 17 to green...this will be the 16 color sprite
	SPRITE_PALETTE[16 + 1] = RGB15(0,0,31);

	for(int i = 0; i < 3; i++) {
		oamSet(
		&oamMain, 						// main display
		i,       						// oam entry to set
		sprites[i].x, sprites[i].y, 	// position
		0, 								// priority
		sprites[i].paletteAlpha, 		// palette for 16 color sprite or alpha for bmp sprite
		sprites[i].size,
		sprites[i].format,
		sprites[i].gfx,
		sprites[i].rotationIndex,
		true, 							// double the size of rotated sprites
		false, 							// don't hide the sprite
		false, false, 					// vflip, hflip
		false 							// apply mosaic
		);
	}

	// update display to show sprites
	oamUpdate(&oamMain);
	
	// text uses ansi escape sequences
	iprintf("Taha Rashid\n");
	iprintf("\033[31;1;4mFeb 18, 2025\n\x1b[39m");
	iprintf("Line 3\n");
	iprintf("\x1b[32;1mLine 4\n\x1b[39m");
	iprintf("\x1b[31;1;4mLine 5\n\x1b[39m");
	iprintf("Line 6\n");

	// NOTE: bottom screen has 24 lines, 32 columns (from 0 -> 23, 0 -> 32)
	iprintf("\x1b[23;31HTest!");
	iprintf("\x1b[11;11HPress Start");

	// fade top screen in
	// blend control. takes effect mode / source / destination
	REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BACKDROP;
	for(int i = 0; i <= 16; i++) {
		// source opacity / dest opacity. They should add up to 16
		REG_BLDALPHA = i | ((16 - i) << 8);
	
		// wait for duration amount of frames
		for (int frame = 0; frame <= 4; frame++) {
			swiWaitForVBlank();
		}
	}

	// for bottom screen text animation
	bool animateText = true;
	int duration = 4;
	int durationCounter = 0;
	int brightness = 16;
	int brightnessCounter = 0;
 
	while(pmMainLoop()) {
		swiWaitForVBlank();
		scanKeys();
		int keys = keysDown();

		// cancel text animation on start btn or touchscreen input
		if (keys & (KEY_START | KEY_TOUCH)) {
			setBrightness(2, 0);
			animateText = false;
		}
		
		touchRead(&touchXY);

		// print at using ansi escape sequence \x1b[line;columnH 
		iprintf("\x1b[10;0HFrame = %d",frame);
		iprintf("\x1b[16;0HTouch x = %04X, %04X\n", touchXY.rawx, touchXY.px);
		iprintf("Touch y = %04X, %04X\n", touchXY.rawy, touchXY.py);

		
		// animate text (fade in/out)
		if (!animateText) {
			continue;
		}

		durationCounter++;
		if (durationCounter >= duration) {
			durationCounter = 0;
			brightnessCounter++;
			setBrightness(2, brightnessCounter - 16);
		}

		if (brightnessCounter >= brightness) {
			brightnessCounter = 0;
		}
	}

	return 0;
}
