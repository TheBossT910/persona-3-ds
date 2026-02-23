#include <nds.h>
#include <stdio.h>
#include "MainMenuView.h"

void MainMenuView::Init() {
    // transition both screens from white
    for(int i = 16; i > 0; i--) {
        setBrightness(3, i);
    
        // wait a few frames
        for (int duration = 0; duration <= 2; duration++) {
            swiWaitForVBlank();
        }
    }
}

ViewState MainMenuView::Update() {
    // lifecycle
    iprintf("\x1b[10;0HHello, world from MainMenuView");

    // wait a few frames
    for (int duration = 0; duration <= 30; duration++) {
        swiWaitForVBlank();
    }

    // // transition both screens to black
    for(int i = 0; i > -16; i--) {
        setBrightness(3, i);
    
        // wait a few frames
        for (int duration = 0; duration <= 2; duration++) {
            swiWaitForVBlank();
        }
    }

    // cycle back to IntroView
    return ViewState::INTRO;
}

void MainMenuView::Cleanup() {
    // reset brightness
    setBrightness(3, 0);
    
    // clear text
    iprintf("\x1b[2J");
}