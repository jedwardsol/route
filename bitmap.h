#pragma once
#include <cstdint>
#include "route.h"

// palette entries
enum Colour
{
    background=255,
    startStop =254,
    route     =253,
};


extern BITMAPINFO     *bitmapHeader;
extern uint8_t         bitmapData[dim][dim];
