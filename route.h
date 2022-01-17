#pragma once

#include <compare>
#include <atomic>

constexpr auto      infinity=std::numeric_limits<double>::max();

constexpr   int     dim   = 1000;

struct Location
{
    int row;
    int column;

    auto operator<=>(Location const &rhs) const noexcept = default;
};

constexpr Location start  {     30,    30 };
constexpr Location finish { dim-30,dim-30 };
constexpr Location noRoute{-1,-1};


extern std::atomic_bool  done;

void startRouting();
void fillBitmap();
void initGrid();
void endRouting();


struct Element
{
// contents
    bool        blocked{};

// routing
    bool        visited{false};    
    double      distance{infinity};
    Location    previous{noRoute};
};


extern Element                     grid[dim][dim];

