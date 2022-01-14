#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
#include "route.h"

// palette entries
enum Colour
{
    background  = 255,
    startStop   = 254,
    route       = 253,
    blocked     = 252,

    maxDistance = 250,

};


extern BITMAPINFO     *bitmapHeader;
extern uint8_t         bitmapData[dim][dim];
