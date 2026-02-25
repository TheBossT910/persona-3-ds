#include <nds.h>
#include <stdio.h>
#include "globals.h"
#include "IwatodaiDormView.h"

// assets
#include "teapot_bin.h"

float rotateX = 0.0;
float rotateY = 0.0;

void IwatodaiDormView::Init() {
    // set video mode for 3　2D backgrounds, 1 3D background
	videoSetMode(MODE_0_3D);
	// set sub video mode for 4 text layers
	videoSetModeSub(MODE_0_2D);

    // initialize gl
    glInit();
    
    // enable antialiasing
    glEnable(GL_ANTIALIAS);

    // setup the rear plane
	glClearColor(0,0,0,31); // BG must be opaque for AA to work
	glClearPolyID(63);      // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);

	// this should work the same as the normal gl call
	glViewport(0,0,255,191);

	// any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);

	gluLookAt(	0.0, 0.0, 3.5,		// camera possition
				0.0, 0.0, 0.0,		// look at
				0.0, 1.0, 0.0);		// up

	glLight(0, RGB15(31,31,31), 0,                      floattov10(-1.0),   0);
	glLight(1, RGB15(31,0,31),  0,                      floattov10(1) - 1,  0);
	glLight(2, RGB15(0,31,0),   floattov10(-1.0),       0,                  0);
	glLight(3, RGB15(0,0,31),   floattov10(1.0) - 1,    0,                  0);

	//not a real gl function and will likely change
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | 
        POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 | POLY_FORMAT_LIGHT2 | POLY_FORMAT_LIGHT3 ) ;

	// debug init
	// NOTE: for some reason, we cant use vram bank C. It might be because of consoleDemoInit...
	consoleDemoInit();
}

ViewState IwatodaiDormView::Update() {
    scanKeys();
    u32 keys = keysHeld();

    if(keys & KEY_A) return ViewState::MAIN_MENU;
    if(keys & KEY_START) return ViewState::KEEP_CURRENT;
    if(!(keys & KEY_UP)) rotateX += 3;
    if(!(keys & KEY_DOWN)) rotateX -= 3;
    if(!(keys & KEY_LEFT)) rotateY += 3;
    if(!(keys & KEY_RIGHT)) rotateY -= 3;

    glPushMatrix();

    glRotateX(rotateX);
    glRotateY(rotateY);

    glCallList((u32*)teapot_bin);

    glPopMatrix(1);

    glFlush(0);

    // bottom screen
    iprintf("\x1b[10;0HIwatodaiDormView");
    
    // for (int delay = 0; delay < 15; delay++) {
    //     swiWaitForVBlank();
    // }

    // for(int i = 0; i > -16; i--) {
    //     setBrightness(3, i);
    //     for (int duration = 0; duration <= 2; duration++) {
    //         swiWaitForVBlank();
    //     }
    // }

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::Cleanup() {
    // reset brightness
    setBrightness(3, 0);

    // clear text
    iprintf("\x1b[2J"); 
}