#include <nds.h>
#include <stdio.h>
#include "globals.h"
#include "IwatodaiDormView.h"

// assets
#include "teapot_bin.h"
#include "environment_bin.h"
#include "character_bin.h"

#include "character.h"
#include "environment.h"

float angle = 0.0;
float translateX = 0.0;
float translateZ = 0.0;

// texture ID
static int environmentTextureId;
static int characterTextureId;

void DrawEnvironmentModel() {
    // bind texture before drawing
    glBindTexture(GL_TEXTURE_2D, environmentTextureId);
    glCallList((u32*)environment_bin);
}

void DrawPlayerModel() {
    // glCallList((u32*)teapot_bin);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);
    glCallList((u32*)character_bin);
}

void IwatodaiDormView::Init() {
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D);    // enable texturing

    glClearColor(0,0,0,31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    // set size of screen
    glViewport(0,0,255,191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);

    // light id, light color, x, y, z
    glLight(0, RGB15(31,31,31), 0,                   floattov10(-1.0),  0);
    glLight(1, RGB15(31,0,31),  0,                   floattov10(1) - 1, 0);
    glLight(2, RGB15(0,31,0),   floattov10(-1.0),    0,                 0);
    glLight(3, RGB15(0,0,31),   floattov10(1.0) - 1, 0,                 0);

    // load texture
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);

    // environment
    glGenTextures(1, &environmentTextureId);
    glBindTexture(GL_TEXTURE_2D, environmentTextureId);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA,
        TEXTURE_SIZE_256, TEXTURE_SIZE_256,
        0,
        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        environmentBitmap  // from environment.h
    );

    // character
    glGenTextures(1, &characterTextureId);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA,
        TEXTURE_SIZE_16, TEXTURE_SIZE_16,
        0,
        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        characterBitmap  // from character.h
    );

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
    glColor3b(255, 255, 255);   // keep white so texture colors aren't tinted

    consoleDemoInit();
}

ViewState IwatodaiDormView::Update() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    scanKeys();
    u32 keys = keysHeld();

    if(keys & KEY_START) return ViewState::MAIN_MENU;
    if(keys & KEY_L) angle += 3;
    if(keys & KEY_R) angle -= 3;

    if(!(keys & KEY_UP)) translateX += 0.01;
    if(!(keys & KEY_DOWN)) translateX -= 0.01;
    if(!(keys & KEY_LEFT)) translateZ += 0.01;
    if(!(keys & KEY_RIGHT)) translateZ -= 0.01;

    // move cameras
    glRotateX(angle);
    gluLookAt(  translateX, 0.6, -1 + translateZ,   // location of camera (x, y, z)
                translateX, -0.8, translateZ,       // where camera is looking (x, y, z)
                0, 1.0, 0.0);                       // unit vectors describing direction

    // draw environment
    glPushMatrix();
        DrawEnvironmentModel();
    glPopMatrix(1);

    // draw characters
    glPushMatrix();
        // move character
        glTranslatef(translateX, 0, translateZ);
        // TODO: export character and environment model at 0,0,0 in Blockbench. This is translating the model to show properly
        glTranslatef(0, 0.0, -0.5);
        DrawPlayerModel();
    glPopMatrix(1);

    glFlush(0);

    iprintf("\x1b[10;0HIwatodaiDormView");

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::Cleanup() {
    setBrightness(3, 0);
    iprintf("\x1b[2J");
}