#include <nds.h>
#include <stdio.h>
#include "globals.h"
#include "IwatodaiDormView.h"

void IwatodaiDormView::Init() {
    // set video mode for 4 text layers
	videoSetMode(MODE_0_2D);
	// set sub video mode for 4 text layers
	videoSetModeSub(MODE_0_2D);

	// debug init
	// NOTE: for some reason, we cant use vram bank C. It might be because of consoleDemoInit...
	consoleDemoInit();
}

ViewState IwatodaiDormView::Update() {
    iprintf("\x1b[10;0HIwatodaiDormView");
    
    for (int delay = 0; delay < 15; delay++) {
        swiWaitForVBlank();
    }

    for(int i = 0; i > -16; i--) {
        setBrightness(3, i);
        for (int duration = 0; duration <= 2; duration++) {
            swiWaitForVBlank();
        }
    }

    // TODO: change to KEEP_CURRENT after implementation
    return ViewState::MAIN_MENU;
}

void IwatodaiDormView::Cleanup() {
    // reset brightness
    setBrightness(3, 0);

    // clear text
    iprintf("\x1b[2J"); 
}