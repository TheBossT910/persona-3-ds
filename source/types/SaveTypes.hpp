#pragma once

struct Save
{
    char introVideoPath[128];
    char lastName[32];
    char firstName[32];
    bool femcMode;
} __attribute__((packed));
