#pragma once
#include <nds.h>

float randf()
{
    return (float)rand() / ((float)RAND_MAX + 1);
}
