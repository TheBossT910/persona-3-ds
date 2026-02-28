#include <nds.h>
#include <stdio.h>
#include "globals.h"
#include "IwatodaiDormView.h"

// assets
#include "teapot_bin.h"
#include "output_bin.h"
#include "texture.h"

float rotateX = 0.0;
float rotateY = 0.0;
float translateX = 0.0;
float translateY = 0.0;

// texture ID
static int textureID;

void IwatodaiDormView::Init() {
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    glInit();
    
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D);  // enable texturing

    glClearColor(0,0,0,31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    glViewport(0,0,255,191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(  0.0, 0.0, 2,
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.0);

    glLight(0, RGB15(31,31,31), 0,                   floattov10(-1.0),  0);
    glLight(1, RGB15(31,0,31),  0,                   floattov10(1) - 1, 0);
    glLight(2, RGB15(0,31,0),   floattov10(-1.0),    0,                 0);
    glLight(3, RGB15(0,0,31),   floattov10(1.0) - 1, 0,                 0);

    // use VRAM_A for textures (VRAM_C is taken by consoleDemoInit)
    vramSetBankA(VRAM_A_TEXTURE);

    // load texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA,
        TEXTURE_SIZE_64, TEXTURE_SIZE_64,
        0,
        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        textureBitmap  // from texture.h
    );

    // lights off for textured model — if you want lighting AND texture,
    // change GL_RGBA to GL_RGB and add POLY_FORMAT_LIGHTx flags back below
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);

    glColor3b(255, 255, 255);  // keep white so texture colors aren't tinted

    consoleDemoInit();
}

ViewState IwatodaiDormView::Update() {
    scanKeys();
    u32 keys = keysHeld();

    if(keys & KEY_START) return ViewState::MAIN_MENU;
    if(keys & KEY_A) rotateX += 3;
    if(keys & KEY_B) rotateX -= 3;
    if(keys & KEY_X) rotateY += 3;
    if(keys & KEY_Y) rotateY -= 3;
    if(!(keys & KEY_UP)) translateX += 3;
    if(!(keys & KEY_DOWN)) translateX -= 3;
    if(!(keys & KEY_LEFT)) translateY += 3;
    if(!(keys & KEY_RIGHT)) translateY -= 3;

    glPushMatrix();

    glRotateX(rotateY);
    glRotateY(rotateX);
    glTranslatef32(translateY * -10, translateX * 10, 0);

    // bind texture before drawing
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCallList((u32*)output_bin);

    glPopMatrix(1);

    glFlush(0);

    iprintf("\x1b[10;0HIwatodaiDormView");

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::Cleanup() {
    setBrightness(3, 0);
    iprintf("\x1b[2J");
}