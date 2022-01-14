#pragma once

#include <compare>

constexpr   int     dim   = 256;

struct Location
{
    int row;
    int column;

    auto operator<=>(Location const &rhs) const noexcept = default;
};

constexpr Location start  {     30,    30 };
constexpr Location finish { dim-30,dim-30 };



void startRouting();

void fillBitmap();
