#include <nds.h>
#include <stdio.h>
#include "MainMenuView.h"

void MainMenuView::Init() {
    // setup
}

ViewState MainMenuView::Update() {
    // lifecycle
    iprintf("\x1b[10;0HHello, world from MainMenuView");
    return ViewState::KEEP_CURRENT;
}

void MainMenuView::Cleanup() {
    // cleanup
}