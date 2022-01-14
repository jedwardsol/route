#pragma once

#include <compare>
#include <atomic>

constexpr   int     dim   = 1000;

struct Location
{
    int row;
    int column;

    auto operator<=>(Location const &rhs) const noexcept = default;
};

constexpr Location start  {     30,    30 };
constexpr Location finish { dim-30,dim-30 };


extern std::atomic_bool  done;

void startRouting();
void fillBitmap();
