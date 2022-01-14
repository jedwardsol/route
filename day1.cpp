#include <cassert>
#include <cstdint>

#include <array>
#include <vector>
#include <string>

#include <iostream>
#include <sstream>
#include <format>

#include <algorithm>
#include <numeric>
#include <ranges>

using namespace std::literals;
#include "include/thrower.h"

extern std::istringstream data;

struct Location
{
    int row{};
    int column{};
};


enum Direction
{
    North,East,South,West

};


int main()
try
{
    std::string  token;

    Location        location {0,0};
    Direction       direction{North};

    while(data>>token)
    {
        assert(token[0]=='L' || token[0]=='R');
        assert(std::isdigit(token[1]));

        if(token[0]=='L')
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 4 - 1) % 4);
        }
        else
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
        }

        token.erase(token.begin());

        auto distance = stoi(token);

        switch(direction)
        {
        case North:     location.row-=distance; break;
        case South:     location.row+=distance; break;
        case East:      location.column+=distance; break;
        case West:      location.column-=distance; break;
        }
    }

    std::cout << "Part 1 : " << std::abs(location.row)+std::abs(location.column) << "\n";


    return 0;
}
catch(std::exception const &e)
{
    std::cout << e.what() << '\n';
}