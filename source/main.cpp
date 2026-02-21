/*---------------------------------------------------------------------------------

	$Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

	Simple console print demo
	-- dovoto


---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "skyBackground.h"
#include "roomBackground.h"
#include "silhouetteBackground.h"
#include "logoSprite.h"

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
int bg[3];

// fn for the interrupt
void Vblank() {
	frame++;
}
	
int main(void) {
	touchPosition touchXY;

	irqSet(IRQ_VBLANK, Vblank);

	// set video mode for 2 text layers and 2 extended background layers
	videoSetMode(MODE_0_2D);
	// set sub video mode for 4 text layers
	videoSetModeSub(MODE_0_2D);

	// map vram bank A to main engine background (slot 0)
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	// map vram bank B to main engine sprites (slot 0)
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// enable extended palettes
	bgExtPaletteEnable();

	// debug init
	consoleDemoInit();

	// set brightness on bottom screen to completely dark (no visible image)
	setBrightness(2, -16);

	// initialize backgrounds
	// check https://mtheall.com/vram.html to ensure bg fit in vram
	bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 12, 6);	// silhouette
	bg[1] = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 10, 0);	// room
	bg[2] = bgInit(2, BgType_Text8bpp, BgSize_T_256x256, 11, 2);	// sky
	

	// copy graphics to vram
	dmaCopy(silhouetteBackgroundTiles,  bgGetGfxPtr(bg[0]), silhouetteBackgroundTilesLen);
	dmaCopy(roomBackgroundTiles,  bgGetGfxPtr(bg[1]), roomBackgroundTilesLen);
  	dmaCopy(skyBackgroundTiles, bgGetGfxPtr(bg[2]), skyBackgroundTilesLen);

	// copy maps to vram
	dmaCopy(silhouetteBackgroundMap,  bgGetMapPtr(bg[0]), silhouetteBackgroundMapLen);
	dmaCopy(roomBackgroundMap,  bgGetMapPtr(bg[1]), roomBackgroundMapLen);
  	dmaCopy(skyBackgroundMap, bgGetMapPtr(bg[2]), skyBackgroundMapLen);

	vramSetBankE(VRAM_E_LCD); // for main engine

	// copy palettes to extended palette area
	dmaCopy(silhouetteBackgroundPal, &VRAM_E_EXT_PALETTE[0][0], silhouetteBackgroundPalLen); // bg 0, slot 0
	dmaCopy(roomBackgroundPal,  &VRAM_E_EXT_PALETTE[1][0],  roomBackgroundPalLen);  // bg 1, slot 0
	dmaCopy(skyBackgroundPal, &VRAM_E_EXT_PALETTE[2][12], skyBackgroundPalLen); // bg 2, slot 12 (specified slot in .grit file)

	// map vram to extended palette
	vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	// showing moon as 3 sprites
	MySprite sprites[] = {
		{0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 15, 0, 120},
		// {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 0, 64, 0},
		// {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 128, 0}
	};

	// initialize sub sprite engine with 1D mapping, 128 byte boundry, no external palette support
	oamInit(&oamMain, SpriteMapping_1D_128, false);
	
	// allocating space for sprite graphics
	u16* logoSpriteGfxPtr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);
	dmaCopy(logoSpriteTiles, logoSpriteGfxPtr, logoSpriteTilesLen);
    dmaCopy(logoSpritePal, SPRITE_PALETTE, logoSpritePalLen);

	for(int i = 0; i < 1; i++)
      sprites[i].gfx = logoSpriteGfxPtr;
	
	// text uses ansi escape sequences
	iprintf("Taha Rashid\n");
	iprintf("\033[31;1;4mFeb 18, 2025\n\x1b[39m");
	iprintf("Line 3\n");
	iprintf("\x1b[32;1mLine 4\n\x1b[39m");
	iprintf("\x1b[31;1;4mLine 5\n\x1b[39m");
	iprintf("Line 6\n");

	// NOTE: bottom screen has 24 lines, 32 columns (from 0 -> 23, 0 -> 32)
	iprintf("\x1b[23;31HTest!");
	// center the text by doing (32 / 2) - (len / 2)
	iprintf("\x1b[11;8HPress Any Button");

	// for slide in animation
	// move camera to the empty right half of the 512px wide background
	int silhouetteX = -256;
	int silhouetteY = 192;
	bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
	bgUpdate();

	// fade top screen in
	// blend control. takes effect mode / source / destination
	REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BACKDROP;
	for(int i = 0; i <= 16; i++) {
		// source opacity / dest opacity. They should add up to 16
		REG_BLDALPHA = i | ((16 - i) << 8);
	
		// wait for duration amount of frames
		for (int frame = 0; frame <= 6; frame++) {
			swiWaitForVBlank();
		}
	}

	// for bottom screen text animation
	bool animateText = true;
	int duration = 4;
	int durationCounter = 0;
	int brightness = 16;
	int brightnessCounter = 0;

	bool displayLogo = false;
	int logoOpacity = 0;
 
	while(pmMainLoop()) {
		swiWaitForVBlank();
		bgUpdate();
		oamUpdate(&oamMain);
		scanKeys();
		int keys = keysDown();

		// cancel text animation on any key input
		if (keys) {
			setBrightness(2, 0);
			animateText = false;
		}
		
		touchRead(&touchXY);

		// print at using ansi escape sequence \x1b[line;columnH 
		iprintf("\x1b[10;0HFrame = %d",frame);
		iprintf("\x1b[16;0HTouch x = %04X, %04X\n", touchXY.rawx, touchXY.px);
		iprintf("Touch y = %04X, %04X\n", touchXY.rawy, touchXY.py);

		// scroll silhouette background
		// animate X (moving right towards 0)
		if (silhouetteX < 0 && frame % 5 == 0) {
			silhouetteX += (-silhouetteX) / 6 + 1;
			if (silhouetteX > 0) silhouetteX = 0;
		}

		// animate Y (moving up towards 0)
		if (silhouetteY > 0 && frame % 5 == 0) {
			silhouetteY += (-silhouetteY) / 6 + 1; 
			if (silhouetteY < 0) silhouetteY = 0;
		}

		bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
		
		// perform code after silhouette slide-in
		if (silhouetteX < 0 || silhouetteY < 0) {
			oamMain.oamMemory[0].attribute[0] &= ~ATTR0_DISABLED;
			continue;
		}

		// animate bottom screen text
		if (animateText) {
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

		// setup sprites
		if (!displayLogo) {
			displayLogo = true;
			for(int i = 0; i < 1; i++) {
				oamSet(
					&oamMain, 						// main display (OamState)
					i,       						// oam entry to set (id)
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
				
				oamMain.oamMemory[i].attribute[0] |= ATTR0_TYPE_BLENDED;
				// source is sprite, dest is all bgs
				REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_SPRITE | 
							BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BG2 | BLEND_DST_BACKDROP;
			}
		}

		// fade in sprites
        if (logoOpacity < 16 && frame % 4 == 0) {
            logoOpacity++;
			REG_BLDALPHA = logoOpacity | ((16 - logoOpacity) << 8);
        }
	}

	return 0;
}
