/**
 * @file structs.h
 * @brief Holds structs used game-wide
 */

#pragma once
#include "core/enums.h"
#include "core/geometry.h"
#include <nds.h>
#include <string>
#include <vector>
class BaseMenu;

struct SpriteRegister
{
    int id;
    void* tiles;
    u32 tilesLen;
    void* pal;
    u32 palLen;
};

struct SpriteDBEntry
{
    SpriteType type;
    int id;
    const char* filename;
};

//a simple sprite structure
struct Sprite
{
    u16* gfx;
    SpriteSize size;
    SpriteColorFormat format;
    int rotationIndex;
    int paletteAlpha;
    int x;
    int y;
};

struct MenuOption
{
    const char* name;
    int bgIndex;
    ViewState (BaseMenu::*onSelect)();
};

struct MenuState
{
    MenuOption* options;
    int optionCount;
    int selectedOption;
    int startIndex;
};

struct Keyframe
{
    int time;
    s16 rotX, rotY, rotZ;
    s16 posX, posY, posZ;
};

struct AnimTrack
{
    std::vector<Keyframe> frames;
};

struct Animation
{
    int duration;
    std::vector<AnimTrack> nodeTracks;
};

struct SubList
{
    int texSlot;
    std::vector<u32> displayList;
    u32 displayListSize;
};

struct EnvironmentTexture
{
    const char* name;
    int width;
    int height;

    const unsigned int* bitmap;
};

struct BillboardData
{
    const char* name;
    v16 x, y, z;
    v16 halfWidth;
    v16 halfHeight;

    int texSlot;

    short u0, v0;
    short u1, v1;
};

struct EnvironmentDbEntry
{
    // Name/debugging
    const char* name;

    // Binary display list file
    const char* binaryFile;

    // World bounds
    float worldOffsetX;
    float worldOffsetZ;
    float worldWidth;
    float worldDepth;

    // Texture information
    int textureCount;
    const EnvironmentTexture* textures;

    // Billboards
    int billboardCount;
    const BillboardData* billboards;
};

struct AnimNode
{
    int id;
    int parentId;
    std::vector<SubList> subLists;
    std::vector<int> children;
    v16 pivotX, pivotY, pivotZ;
};

struct CharacterPosition
{
    float x;
    float z;
    float y; // height
    float facingAngle;
};

// From DialogueController.h
struct Dialogue;
struct DialogueSelection
{
    std::string text;
    bool isSelected;
    Dialogue* next;
};
struct Dialogue
{
    std::string characterName;
    std::string text;
    int imageId;
    Dialogue* prev;
    Dialogue* next;
    std::vector<DialogueSelection> selections;
};

struct Save
{
    char introVideoPath[128];
    char lastName[32];
    char firstName[32];
    bool femcMode;
} __attribute__((packed));

struct GraphicAsset
{
    void* tiles;
    u32 tilesLen;
    void* pal;
    u32 palLen;
    void* map;
    u32 mapLen;
};
