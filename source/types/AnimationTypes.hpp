#pragma once

#include <nds.h>
#include <vector>

struct Keyframe
{
    int time;
    s16 rotX, rotY, rotZ;
    s16 posX, posY, posZ;
};

struct SubList
{
    int texSlot;
    std::vector<u32> displayList;
    u32 displayListSize;
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

struct AnimNode
{
    int id;
    int parentId;
    std::vector<SubList> subLists;
    std::vector<int> children;
    v16 pivotX, pivotY, pivotZ;
};
