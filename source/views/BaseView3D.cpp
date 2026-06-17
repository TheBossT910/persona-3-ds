#include "BaseView3D.h"
#include "core/globals.h"
#include <nds.h>
#include <stdio.h>

void BaseView3D::init()
{
    // video modes
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    // vram alloc
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);

    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    bgExtPaletteEnableSub();

    // 3D init
    glInit();
    glEnable(GL_ANTIALIAS);  // cleans up edges
    glEnable(GL_TEXTURE_2D); // for adding textures
    glEnable(GL_BLEND);      // useful for UI
    glEnable(GL_FOG);        // hide poping of polygons

    glClearColor(0, 0, 0, 31);
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    // viewport
    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 1.0, 10.0);

    // fog
    // setup color
    glFogColor(22, 25, 28, 31); // daytime blue
    // glFogColor(30, 25, 16, 31); // evening orange
    // glFogColor(16, 17, 19, 31); // rainy gray

    // how much depth difference there is between table entries
    glFogShift(shift);
    // depth at which the fog starts (and the table starts applying)
    glFogOffset(depth);

    // generate a linear density table
    int density = 0;
    for (int i = 0; i < 32; i++) // it has 32 steps
    {
        glFogDensity(i, density);
        // exponentially increase mass the furthur back the fog is
        density += (mass * i) >> 2;

        // entries are 7 bit, so cap the density to 127
        if (density > 127)
            density = 127;
    }

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG);
    glColor3b(255, 255, 255);
}
