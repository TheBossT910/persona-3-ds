#include "UIController.h"

UIController* UIController::instance = nullptr;

void UIController::create()
{
    if (instance == nullptr)
    {
        instance = new UIController();
    }
}

void UIController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

UIController* UIController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

// set the background pointers to use
void UIController::setGraphics(int iBgSub[4], int iBgMain[3], OamState* iOamSub, OamState* iOamMain)
{
    oamSub = iOamSub;
    oamMain = iOamMain;
    for (int i = 0; i < 4; i++)
    {
        lruBgSub[i] = iBgSub[i];
        hwBgSub[i] = iBgSub[i];
    }
    for (int i = 0; i < 3; i++)
    {
        lruBgMain[i] = iBgMain[i];
        hwBgMain[i] = iBgMain[i];
    }

    UIController::hideAll();
}

// register screen. Persistent = is always loaded into memory. Paged (swappable) = can be loaded/unloaded into memory.
// Register screens to have them pre-loaded before calling show()
void UIController::registerScreen(UIScreen* screen, bool isMain)
{
    // load screen
    if (isMain && screenMainCount < 3)
    {
        loadedMain[screenMainCount] = screen;
        screen->bgId = hwBgMain[screenMainCount];
        screenMainCount++;
        screen->oam = oamMain;
        screen->load();
        screen->isLoaded = true;
        return;
    }
    else if (!isMain && screenSubCount < 4)
    {
        loadedSub[screenSubCount] = screen;
        screen->bgId = hwBgSub[screenSubCount];
        ;
        screenSubCount++;
        screen->oam = oamSub;
        screen->load();
        screen->isLoaded = true;
        return;
    }

    // throw error (too many screens registered)
    if (isMain)
    {
        sassert(screenMainCount < 3,
                "Too many screens registered. A maximum of 4 main screens and 3 sub screens can be registered.");
    }
    else
    {
        sassert(screenSubCount < 4,
                "Too many screens registered. A maximum of 4 main screens and 3 sub screens can be registered.");
    }
}

// switches to the specified screen. If already on screen, unhides it. If not loaded, becomes loaded. Cannot load the same screen on both sub and main
void UIController::show(UIScreen* screen, bool isMain)
{
    // check if it is already loaded. If not, load it in
    if (!screen->isLoaded)
    {
        // add screen if space
        if (isMain ? (screenMainCount < 3) : (screenSubCount < 4))
        {
            registerScreen(screen, isMain);
        }
        // swap out screens if no space
        else
        {
            UIScreen* oldScreen = nullptr;

            // remove the least recently displayed screen
            if (isMain)
            {
                int targetBgId = lruBgMain[0];
                for (int i = 0; i < 3; i++)
                {
                    if ((loadedMain[i] != nullptr) && (loadedMain[i]->bgId == targetBgId))
                    {
                        oldScreen = loadedMain[i];
                        loadedMain[i] = screen;
                        break;
                    }
                }
            }
            else
            {
                int targetBgId = lruBgSub[0];
                for (int i = 0; i < 4; i++)
                {
                    if ((loadedSub[i] != nullptr) && (loadedSub[i]->bgId == targetBgId))
                    {
                        oldScreen = loadedSub[i];
                        loadedSub[i] = screen;
                        break;
                    }
                }
            }

            oldScreen->unload();
            oldScreen->isLoaded = false;

            screen->bgId = oldScreen->bgId;
            screen->oam = oldScreen->oam;
            oldScreen->bgId = -1;
            oldScreen->oam = nullptr;

            screen->load();
            screen->isLoaded = true;
        }
    }

    // load screen
    hideAll();
    screen->renderSprites();
    bgShow(screen->bgId);

    // update the "least recently updated" index
    lruUpdate(screen->bgId, isMain);
}

// hides all screens
void UIController::hideAll()
{
    // hide sub screens
    for (UIScreen* val : loadedSub)
    {
        if (val == nullptr)
        {
            continue;
        }
        bgHide(val->bgId);
        val->removeSprites();
    }

    // NOTE: do not hide the 3D layer
    // hide main screens
    for (UIScreen* val : loadedMain)
    {
        if (val == nullptr)
        {
            continue;
        }
        bgHide(val->bgId);
        val->removeSprites();
    }
}

// clean up all the screens
void UIController::cleanup()
{
    // reset all bg ids, UIScreens
    // sub
    for (int i = 0; i < 4; i++)
    {
        lruBgSub[i] = hwBgSub[i];
        if (loadedSub[i] == nullptr)
        {
            continue;
        }
        loadedSub[i]->unload();
        loadedSub[i]->isLoaded = false;
        loadedSub[i]->oam = nullptr;
        loadedSub[i]->bgId = -1;
        loadedSub[i] = nullptr;
    }

    // main
    for (int j = 0; j < 3; j++)
    {
        lruBgMain[j] = hwBgMain[j];
        if (loadedMain[j] == nullptr)
        {
            continue;
        }
        loadedMain[j]->unload();
        loadedMain[j]->isLoaded = false;
        loadedMain[j]->oam = nullptr;
        loadedMain[j]->bgId = -1;
        loadedMain[j] = nullptr;
    }

    screenMainCount = 0;
    screenSubCount = 0;
}

// Updates the order of lruBgSub/lruBgMain for the "least recently updated" id (id is the background id)
void UIController::lruUpdate(int id, bool isMain)
{
    if (isMain)
    {
        int pos = 0;

        // find where the existing id currently is
        for (int i = 0; i < 3; i++)
        {
            if (lruBgMain[i] == id)
            {
                pos = i;
                break;
            }
        }

        // shift everything after that position to the left to close the gap
        for (int i = pos; i < 2; i++)
        {
            lruBgMain[i] = lruBgMain[i + 1];
        }

        // place the updated id at the very end (most recently used)
        lruBgMain[2] = id;
    }
    else
    {
        int pos = 0;

        // find where the existing id currently is
        for (int i = 0; i < 4; i++)
        {
            if (lruBgSub[i] == id)
            {
                pos = i;
                break;
            }
        }

        // shift everything after that position to the left to close the gap
        for (int i = pos; i < 3; i++)
        {
            lruBgSub[i] = lruBgSub[i + 1];
        }

        // place the updated id at the very end (most recently used)
        lruBgSub[3] = id;
    }
}
