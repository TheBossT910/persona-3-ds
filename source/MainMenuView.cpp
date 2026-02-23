#include <nds.h>
#include <stdio.h>
#include "globals.h"
#include "MainMenuView.h"

// assets
#include "menuSilhouetteBackground.h"
#include "doorBackground.h"
#include "fogBackground.h"

void MainMenuView::Init() {
    // transition both screens from white
    for(int i = 16; i > 0; i--) {
        setBrightness(3, i);
    
        // wait a few frames
        for (int duration = 0; duration <= 2; duration++) {
            swiWaitForVBlank();
        }
    }

    // set video mode for 2 text layers and 2 extended rotation layer
	videoSetMode(MODE_5_2D);
	// set sub video mode for 4 text layers
	videoSetModeSub(MODE_0_2D);

	// map vram bank A to main engine background (slot 0)
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
	// map vram bank B to main engine sprites (slot 0)
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// enable extended palettes
	bgExtPaletteEnable();

	// debug init
	// NOTE: for some reason, we cant use vram bank C. It might be because of consoleDemoInit...
	consoleDemoInit();

	// set brightness on bottom screen to completely dark (no visible image)
	setBrightness(2, -16);

	// initialize backgrounds
	// check https://mtheall.com/vram.html to ensure bg fit in vram
	bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 4, 0);		// silhouette
    bg[1] = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 9, 2);		// door
    bg[2] = bgInit(2, BgType_ExRotation, BgSize_ER_256x256, 8, 5); 	// fog
	
	// need to set priority to properly display
	// 0 is highest, 3 is lowest
    bgSetPriority(bg[0], 0);	// silhouette
    bgSetPriority(bg[2], 1);	// fog
    bgSetPriority(bg[1], 2);	// door

    // reset background vram
    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192);
    dmaFillHalfWords(1, bgGetMapPtr(bg[0]), 2048);
    dmaFillHalfWords(2, bgGetMapPtr(bg[0]), 2048);
	
	// copy graphics to vram
	dmaCopy(menuSilhouetteBackgroundTiles,  bgGetGfxPtr(bg[0]), menuSilhouetteBackgroundTilesLen);
    dmaCopy(doorBackgroundTiles,  bgGetGfxPtr(bg[1]), doorBackgroundTilesLen);
    dmaCopy(fogBackgroundTiles, bgGetGfxPtr(bg[2]), fogBackgroundTilesLen);

	// copy maps to vram
	dmaCopy(menuSilhouetteBackgroundMap,  bgGetMapPtr(bg[0]), menuSilhouetteBackgroundMapLen);
    dmaCopy(doorBackgroundMap,  bgGetMapPtr(bg[1]), doorBackgroundMapLen);
    dmaCopy(fogBackgroundMap,   bgGetMapPtr(bg[2]), fogBackgroundMapLen);

	vramSetBankE(VRAM_E_LCD); // for main engine

	// copy palettes to extended palette area
	dmaCopy(menuSilhouetteBackgroundPal, &VRAM_E_EXT_PALETTE[0][0], menuSilhouetteBackgroundPalLen);
    dmaCopy(doorBackgroundPal,  &VRAM_E_EXT_PALETTE[1][0],  doorBackgroundPalLen);
    dmaCopy(fogBackgroundPal,   &VRAM_E_EXT_PALETTE[2][0], fogBackgroundPalLen);

	// map vram to extended palette
	vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    bgHide(bg[2]);
	bgSetCenter(bg[2], 128, 96);	// pivot point on the screen (at the screen's center)
	bgSetScroll(bg[2], 128, 96);	// pivot point on the image (at the image's center)

	// NOTE: bottom screen has 24 lines, 32 columns (from 0 -> 23, 0 -> 32)
	// center the text by doing (32 / 2) - (len / 2)
	iprintf("\x1b[11;0HLoad Game\n");
    iprintf("New Game\n");
    iprintf("Config\n");
    iprintf("Return to Title\n");

	// for slide in animation
	// move camera to the empty right half of the 512px wide background
	bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
	bgUpdate();

    // fade door background in
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BACKDROP;
	for(int i = 0; i <= 16; i++) {
		REG_BLDALPHA = i | ((16 - i) << 8);
	
		// wait for duration amount of frames
		for (int frame = 0; frame <= 6; frame++) {
			swiWaitForVBlank();
		}
	}
}

ViewState MainMenuView::Update() {
    scanKeys();
    int keys = keysDown();

    // transition to menu state on any input
    if (keys) {
        // transition both screens to white
        for(int i = 0; i > -16; i--) {
            setBrightness(3, i);
        
            // wait a few frames
            for (int duration = 0; duration <= 2; duration++) {
                swiWaitForVBlank();
            }
        }
        return ViewState::INTRO;
    }

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
        return ViewState::KEEP_CURRENT;
    }

    // fade in bottom screen text
    if (brightness < 16 && frame % 4 == 0) {
        brightness++;
        setBrightness(2, brightness - 16);
    }

    // setup blending for fog
    if (!displayFog) {
        displayFog = true;
        REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BACKDROP | BLEND_DST_BG1;
        bgShow(bg[2]);
    }

    // fade in fog
    if (fogOpacity < 6 && frame % 4 == 0) {
        fogOpacity++;
        REG_BLDALPHA = fogOpacity | ((16 - fogOpacity) << 8);
    }

    // rotate fog
    if (frame % 4 == 0) {
        waveAngle += 50;        
        int rotationSpeed = baseSpeed + ((sinLerp(waveAngle) * fluctuation) >> 12);
        currentRotation += rotationSpeed;
        bgSetRotateScale(bg[2], currentRotation, 256, 256);
    }

    // default state
    return ViewState::KEEP_CURRENT;
}

void MainMenuView::Cleanup() {
    // reset brightness
    setBrightness(3, 0);

    // disable blending
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;

    // hide all backgrounds
    bgHide(bg[0]);
    bgHide(bg[1]);
    bgHide(bg[2]);

    // clear all sprites from oam
    oamClear(&oamMain, 0, 0);

    // clear text
    iprintf("\x1b[2J"); 
}