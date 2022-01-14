#pragma once

constexpr   int     dim   = 256;

struct Location
{
    int row;
    int column;
};

constexpr Location start  {     30,    30 };
constexpr Location finish { dim-30,dim-30 };
