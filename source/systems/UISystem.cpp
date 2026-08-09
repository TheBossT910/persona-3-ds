#include "UISystem.hpp"

void UISystem::Shutdown()
{
    cleanup();
}

void UISystem::on_receive(const Event::ConfigureUI& config)
{
    // reset previous config
    cleanup();

    // set background
    oamSub = config.oamSub;
    oamMain = config.oamMain;

    lruBgSub = config.bgSub;
    hwBgSub = config.bgSub;

    lruBgMain = config.bgMain;
    hwBgMain = config.bgMain;

    hideAllScreens();

    // register screens
    for (UIScreen* screen : config.screens)
    {
        if (screen == nullptr)
        {
            continue;
        }

        registerScreen(screen);
    }
}

void UISystem::on_receive(const Event::ShowScreen& msg)
{
    // check if it is already loaded. If not, load it in
    if (!msg.screen->isLoaded)
    {
        // add screen if space
        if (msg.screen->isMain ? (screenMainCount < 3) : (screenSubCount < 4))
        {
            registerScreen(msg.screen);
        }
        // swap out screens if no space
        else
        {
            UIScreen* oldScreen = nullptr;

            // remove the least recently displayed screen
            if (msg.screen->isMain)
            {
                int targetBgId = lruBgMain[0];
                for (int i = 0; i < 3; i++)
                {
                    if ((loadedMain[i] != nullptr) && (loadedMain[i]->bgId == targetBgId))
                    {
                        oldScreen = loadedMain[i];
                        loadedMain[i] = msg.screen;
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
                        loadedSub[i] = msg.screen;
                        break;
                    }
                }
            }

            oldScreen->unload();
            oldScreen->isLoaded = false;

            msg.screen->bgId = oldScreen->bgId;
            msg.screen->oam = oldScreen->oam;
            oldScreen->bgId = -1;
            oldScreen->oam = nullptr;

            msg.screen->load();
            msg.screen->isLoaded = true;
        }
    }

    // load screen
    hideAllScreens();
    msg.screen->renderSprites();
    bgShow(msg.screen->bgId);

    // update the "least recently updated" index
    lruUpdate(msg.screen->bgId, msg.screen->isMain);
}

void UISystem::on_receive(const Event::HideAllScreens& /*msg*/)
{
    hideAllScreens();
}

void UISystem::lruUpdate(int id, bool isMain)
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

void UISystem::registerScreen(UIScreen* screen)
{
    sassert(screen != nullptr, "UIScreen cannot be nullptr.");

    // load screen
    if (screen->isMain && screenMainCount < 3)
    {
        loadedMain[screenMainCount] = screen;
        screen->bgId = hwBgMain[screenMainCount];
        screenMainCount++;
        screen->oam = oamMain;
        screen->load();
        screen->isLoaded = true;
        return;
    }
    else if (!screen->isMain && screenSubCount < 4)
    {
        loadedSub[screenSubCount] = screen;
        screen->bgId = hwBgSub[screenSubCount];

        screenSubCount++;
        screen->oam = oamSub;
        screen->load();
        screen->isLoaded = true;
        return;
    }

    // throw error (too many screens registered)
    if (screen->isMain)
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

void UISystem::hideAllScreens()
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

void UISystem::cleanup()
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
