/*---------------------------------------------------------------------------------

	$Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

	Simple console print demo
	-- dovoto


---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "background.h"

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
	vramSetBankA(VRAM_A_MAIN_BG);

	// debug init
	consoleDemoInit();

	// set brightness on bottom screens to completely dark (no visible image)
	setBrightness(2, -16);

	// setup bitmap background (for image) on background 3
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	// NOTE: imgs need to be converted to be used (http://www.coranac.com/projects/#grit)
	// NOTE: backgroundBitmap is an extern defined in background.h
	decompress(backgroundBitmap, BG_GFX, LZ77Vram);
	
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
