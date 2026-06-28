#include "IntroView.h"
#include "core/globals.h"
#include <maxmod9.h>
#include <nds.h>
#include <stdio.h>
#include <string>

// sfx
#include "soundbank.h"

void IntroView::init()
{
    // set video mode for 3 text layers and 1 extended rotation layer
    videoSetMode(MODE_3_2D);
    // set sub video mode for 4 text layers
    videoSetModeSub(MODE_0_2D);

    // map vram bank A and D to main engine background (slot 0)
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    // map vram bank B to sub engine background
    vramSetBankC(VRAM_C_SUB_BG);
    // map vram bank B to main engine sprites (slot 0)
    vramSetBankB(VRAM_B_MAIN_SPRITE);

    // enable extended palettes
    bgExtPaletteEnable();
    bgExtPaletteEnableSub();

    // set brightness on both screen to completely dark (no visible image)
    setBrightness(3, -16);

    // initialize backgrounds
    // check https://mtheall.com/vram.html to ensure bg fit in vram
    bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 11, 2);       // silhouette
    bg[1] = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 9, 0);        // room
    bg[2] = bgInit(2, BgType_Text8bpp, BgSize_T_256x256, 10, 3);       // sky
    bg[3] = bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 19, 8);    // overlay
    bgSubLogo = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1); // logo
    bgSubSky = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 1, 2);  // sky (sub screen)

    // setup console
    consoleInit(&console, 2, BgType_Text4bpp, BgSize_T_256x256, 4, 5, false, true);
    consoleSelect(&console);

    // need to set priority to properly display
    // 0 is highest, 3 is lowest
    bgSetPriority(bg[0], 1); // silhouette
    bgSetPriority(bg[1], 1); // room
    bgSetPriority(bg[2], 3); // sky
    bgSetPriority(bg[3], 2); // overlay
    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSubLogo, 1);
    bgSetPriority(bgSubSky, 2);

    // reset background vram
    // 512x512 backgrounds use 8192 bytes of map memory
    // calculated using (512 / 8) * (512 / 8) * 2
    // the DS divides pixels into 8x8 tiles (hence we divide by 8) and we use 16-bit colour (which is 2 bytes)
    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192); // silhouette
    dmaFillHalfWords(0, bgGetMapPtr(bg[3]), 8192); // overlay
    // 256x256 backgrounds use 2048 bytes of map memory
    dmaFillHalfWords(0, bgGetMapPtr(bg[1]), 2048); // room
    dmaFillHalfWords(0, bgGetMapPtr(bg[2]), 2048); // sky
    dmaFillHalfWords(0, bgGetMapPtr(bgSubLogo), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSubSky), 2048);

    bool femc = saveData.femcMode;
    std::string bgPath = fatBasePath + "graphics/IntroView/backgrounds/";
    std::string spritePath = fatBasePath + "graphics/IntroView/sprites/";
    std::string suffix = femc ? "FEMC" : "";

    GraphicAsset silhouette =
        graphicsCtrl.loadGrit(bgPath + "silhouetteBackground" + suffix + "/silhouetteBackground" + suffix);
    GraphicAsset room = graphicsCtrl.loadGrit(bgPath + "roomBackground" + suffix + "/roomBackground" + suffix);
    GraphicAsset sky = graphicsCtrl.loadGrit(bgPath + "skyBackground" + suffix + "/skyBackground" + suffix);
    GraphicAsset overlay = graphicsCtrl.loadGrit(bgPath + "overlayBackground" + suffix + "/overlayBackground" + suffix);
    GraphicAsset skySub = graphicsCtrl.loadGrit(bgPath + "skyBackgroundSub" + suffix + "/skyBackgroundSub" + suffix);

    GraphicAsset attribution = graphicsCtrl.loadGrit(bgPath + "attributionBackground/attributionBackground");
    GraphicAsset logoLeft = graphicsCtrl.loadGrit(spritePath + "logoSpriteLeft/logoSpriteLeft");
    GraphicAsset logoRight = graphicsCtrl.loadGrit(spritePath + "logoSpriteRight/logoSpriteRight");

    // copy graphics to vram
    dmaCopy(silhouette.tiles, bgGetGfxPtr(bg[0]), silhouette.tilesLen);
    dmaCopy(room.tiles, bgGetGfxPtr(bg[1]), room.tilesLen);
    dmaCopy(sky.tiles, bgGetGfxPtr(bg[2]), sky.tilesLen);
    dmaCopy(overlay.tiles, bgGetGfxPtr(bg[3]), overlay.tilesLen);
    dmaCopy(attribution.tiles, bgGetGfxPtr(bgSubLogo), attribution.tilesLen);
    dmaCopy(skySub.tiles, bgGetGfxPtr(bgSubSky), skySub.tilesLen);

    // copy maps to vram
    dmaCopy(silhouette.map, bgGetMapPtr(bg[0]), silhouette.mapLen);
    dmaCopy(room.map, bgGetMapPtr(bg[1]), room.mapLen);
    dmaCopy(sky.map, bgGetMapPtr(bg[2]), sky.mapLen);
    dmaCopy(overlay.map, bgGetMapPtr(bg[3]), overlay.mapLen);
    dmaCopy(attribution.map, bgGetMapPtr(bgSubLogo), attribution.mapLen);
    dmaCopy(skySub.map, bgGetMapPtr(bgSubSky), skySub.mapLen);

    // can only write to extended palettes in LCD mode
    vramSetBankE(VRAM_E_LCD); // for main engine
    vramSetBankH(VRAM_H_LCD); // for subv engine

    // copy palettes to extended palette area
    dmaCopy(silhouette.pal, &VRAM_E_EXT_PALETTE[0][0], silhouette.palLen);
    dmaCopy(room.pal, &VRAM_E_EXT_PALETTE[1][0], room.palLen);
    dmaCopy(sky.pal, &VRAM_E_EXT_PALETTE[2][0], sky.palLen);
    dmaCopy(overlay.pal, &VRAM_E_EXT_PALETTE[3][0], overlay.palLen);
    dmaCopy(attribution.pal, &VRAM_H_EXT_PALETTE[0][0], attribution.palLen);
    dmaCopy(skySub.pal, &VRAM_H_EXT_PALETTE[1][0], skySub.palLen);

    // map vram to extended palette
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    bgHide(bg[3]);                // hide overlay
    bgSetCenter(bg[3], 128, 96);  // pivot point on the screen (at the screen's center)
    bgSetScroll(bg[3], 256, 256); // pivot point on the image (at the image's center)

    // showing logo as sprite
    logoSprite[0] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 15, -25, 100};
    logoSprite[1] = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 15, 39, 100};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, no external palette support
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // allocating space for sprite graphics
    logoSprite[0].gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);
    logoSprite[1].gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);

    if (logoLeft.tiles)
        dmaCopy(logoLeft.tiles, logoSprite[0].gfx, logoLeft.tilesLen);
    if (logoRight.tiles)
        dmaCopy(logoRight.tiles, logoSprite[1].gfx, logoRight.tilesLen);

    // NOTE: left and right will use the same palette. Just ensure that the order of colours when indexed
    // is THE SAME for both images!
    if (logoRight.pal)
        dmaCopy(logoRight.pal, SPRITE_PALETTE, logoRight.palLen);

    // for slide in animation
    // move camera to the empty right half of the 512px wide background
    bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
    bgUpdate();

    // unload all graphics now that it's copied to vram
    graphicsCtrl.unloadGrit(silhouette);
    graphicsCtrl.unloadGrit(room);
    graphicsCtrl.unloadGrit(sky);
    graphicsCtrl.unloadGrit(overlay);
    graphicsCtrl.unloadGrit(attribution);
    graphicsCtrl.unloadGrit(skySub);
    graphicsCtrl.unloadGrit(logoLeft);
    graphicsCtrl.unloadGrit(logoRight);

    // point to music
    musicCtrl.loadSFX(SFX_SELECT);
    musicCtrl.init((fatBasePath + "music/menus/title/tightrope.pcm").c_str(), 17.962f, 66.082f);

    // hide sub screen text and attribution text layer
    REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_SRC_BG0 | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BACKDROP;
    REG_BLDALPHA_SUB = 0 | (16 << 8);

    // hide main skyBackground
    // blend control. takes effect mode / source / destination
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BACKDROP;

    // fade top screen in
    for (int i = 0; i <= 16; i++)
    {
        setBrightness(1, -16 + i);

        // wait for duration amount of frames
        for (int frame = 0; frame <= 3; frame++)
        {
            musicCtrl.update();
            swiWaitForVBlank();
        }
    }

    // fade skyBackgrounds in
    for (int i = 0; i <= 16; i++)
    {
        // source opacity / dest opacity. They should add up to 16
        REG_BLDALPHA = i | ((16 - i) << 8);
        // fade sub screen
        setBrightness(2, -16 + i);

        // wait for duration amount of frames
        for (int frame = 0; frame <= 6; frame++)
        {
            musicCtrl.update();
            swiWaitForVBlank();
        }
    }
}

ViewState IntroView::update()
{
    musicCtrl.update();
    scanKeys();
    int pressed = keysDown();

    // transition to menu state on any input
    if ((pressed & KEY_A) || (pressed & KEY_START) || (pressed & KEY_TOUCH))
    {
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        musicCtrl.pause();
        // transition both screens to black
        for (int i = 0; i <= 16; i++)
        {
            setBrightness(3, -i);

            // wait a few frames
            for (int duration = 0; duration <= 2; duration++)
            {
                musicCtrl.update();
                swiWaitForVBlank();
            }
        }
        return ViewState::MAIN_MENU;
    }
    else if (pressed & KEY_B)
    {
        musicCtrl.playSFX(SFX_CANCEL, 255, 128);
        musicCtrl.pause();
        // transition both screens to black
        for (int i = 0; i <= 16; i++)
        {
            setBrightness(3, -i);

            // wait a few frames
            for (int duration = 0; duration <= 2; duration++)
            {
                musicCtrl.update();
                swiWaitForVBlank();
            }
        }
        return ViewState::INTRO_VIDEO;
    }

    // scroll silhouette background
    // animate X (moving right towards 0)
    if (silhouetteX < 0 && frame % 5 == 0)
    {
        silhouetteX += (-silhouetteX) / 6 + 1;
        if (silhouetteX > 0)
            silhouetteX = 0;
    }

    // animate Y (moving up towards 0)
    if (silhouetteY > 0 && frame % 5 == 0)
    {
        silhouetteY += (-silhouetteY) / 6 + 1;
        if (silhouetteY < 0)
            silhouetteY = 0;
    }

    bgSetScroll(bg[0], -silhouetteX, -silhouetteY);

    // perform code after silhouette slide-in
    if (silhouetteX < 0 || silhouetteY < 0)
    {
        return ViewState::KEEP_CURRENT;
    }

    if (animateText)
    {
        // NOTE: The text uses ansi escape sequences.
        // The bottom screen has 24 lines, 32 columns (from 0 -> 23, 0 -> 32)
        // Center the text by doing (32 / 2) - (len / 2)
        iprintf("\x1b[11;8HPress Any Button");

        durationCounter++;

        if (durationCounter >= duration)
        {
            durationCounter = 0;
            textAlpha += textAlphaDirection;

            if (textAlpha >= 16)
            {
                textAlpha = 16;
                textAlphaDirection = -1; // Start fading out
            }
            else if (textAlpha <= 0)
            {
                textAlpha = 0;
                textAlphaDirection = 1; // Start fading in
            }

            REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BACKDROP;
            REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);
        }
    }

    // setup logoSprite
    if (!displayLogo)
    {
        displayLogo = true;

        for (int i = 0; i < 2; i++)
        {
            oamSet(&oamMain, // main display (OamState)
                   i,        // oam entry to set (id)
                   logoSprite[i].x,
                   logoSprite[i].y,            // position
                   0,                          // priority
                   logoSprite[i].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
                   logoSprite[i].size,
                   logoSprite[i].format,
                   logoSprite[i].gfx,
                   logoSprite[i].rotationIndex,
                   true,  // double the size of rotated sprites
                   false, // don't hide the sprite
                   false,
                   false, // vflip, hflip
                   false  // apply mosaic
            );

            oamMain.oamMemory[i].attribute[0] |= ATTR0_TYPE_BLENDED;
        }

        // setup fade for main screen sprites
        REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BG2;
        REG_BLDALPHA = 0 | (16 << 8);

        // setup fade for sub screen attribution text layer
        REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BG1 | BLEND_DST_BACKDROP;
        REG_BLDALPHA_SUB = 0 | (16 << 8);
    }

    // fade in attribution text layer and logoSprite
    if (logoOpacity < 16 && frame % 4 == 0)
    {
        logoOpacity++;
        REG_BLDALPHA = logoOpacity | ((16 - logoOpacity) << 8);
        REG_BLDALPHA_SUB = logoOpacity | ((16 - logoOpacity) << 8);
    }

    // code after sprite fade in
    if (logoOpacity < 16)
    {
        oamMain.oamMemory[0].attribute[0] &= ~ATTR0_TYPE_BLENDED; // disable sprite blending
        oamMain.oamMemory[1].attribute[0] &= ~ATTR0_TYPE_BLENDED; // disable sprite blending
        return ViewState::KEEP_CURRENT;
    }

    // setup animated text
    if (logoOpacity >= 16 && !animateText)
    {
        animateText = true;
        REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BACKDROP;
        REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);
    }

    // setup blending for overlay
    if (!displayOverlay)
    {
        displayOverlay = true;
        REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG2;
        REG_BLDALPHA = 0 | (16 << 8);
        bgShow(bg[3]);
    }

    // fade in overlay
    if (overlayOpacity < 6 && frame % 4 == 0)
    {
        overlayOpacity++;
        REG_BLDALPHA = overlayOpacity | ((16 - overlayOpacity) << 8);
    }

    // rotate overlay
    if (frame % 4 == 0)
    {
        waveAngle += 50;
        // NOTE: since DS does not have floating point numbers, sinLerp returns value from -4096 -> 4096, which is why we divide by 4096 (shift >> 12)
        int rotationSpeed = baseSpeed + ((sinLerp(waveAngle) * fluctuation) >> 12);
        currentRotation += rotationSpeed;
        bgSetRotateScale(bg[3], currentRotation, 256, 256);
    }

    // default state
    return ViewState::KEEP_CURRENT;
}

void IntroView::cleanup()
{
    BaseView::cleanup();

    // clear all sprites from oam
    oamClear(&oamMain, 0, 0);

    // free allocated sprite vram
    for (int i = 0; i < 2; i++)
    {
        if (logoSprite[i].gfx != NULL)
        {
            oamFreeGfx(&oamMain, logoSprite[i].gfx);
        }
    }
}
