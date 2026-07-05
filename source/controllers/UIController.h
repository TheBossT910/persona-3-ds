#pragma once
#include "components/ui/UIScreen.h"

class UIController
{
  public:
    static void create();
    static void destroy();
    static UIController* getInstance();

    // TODO: add persistence support
    // TODO: add a way to pass in -1, indicating reduced # of bg slots
    /*
    * NOTE ON BACKGROUND IDs:
    * The arrays passed into setGraphics() must contain the actual libnds
    * hardware background layer IDs (e.g., 0, 1, 2, 3).
    * * The UIController manages the mapping of UIScreens to these hardware layers.
    * When a screen is registered, it is assigned an available hardware bgId.
    * The lruBgMain/lruBgSub arrays dynamically shuffle to track the Least Recently Used
    * (LRU) hardware layer at index [0], ensuring the oldest visible screen is
    * the one overwritten when capacity is reached.
    */
    void setGraphics(int iBgSub[4], int iBgMain[3], OamState* iOamSub, OamState* iOamMain);
    void registerScreen(UIScreen* screen, bool isMain);
    void show(UIScreen* screen, bool isMain);
    void hideAll();
    void cleanup();

  private:
    UIController() = default; // this needs to be defaul, else it won't initialize properly
    ~UIController() {};
    static UIController* instance;

    void lruUpdate(int id, bool isMain);

    OamState* oamSub;
    OamState* oamMain;

    // background ids. The order of the arrays matter. Front = least recently updated, back = last updated
    int lruBgSub[4];
    int lruBgMain[3];

    // original background ids (order doesn't change)
    int hwBgSub[4];
    int hwBgMain[3];

    // currently loaded screens (max 4 sub, 3 main)
    int screenMainCount = 0;
    int screenSubCount = 0;
    UIScreen* loadedSub[4];
    UIScreen* loadedMain[3];
};
