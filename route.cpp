
#include <mutex>
#include <thread>
#include <chrono>
using namespace std::literals;

#include <cassert>
#include <random>

#include <vector>
#include <queue>
#include <stack>
#include <map>

#include "print.h"
#include "route.h"
#include "bitmap.h"


constexpr auto      infinity=std::numeric_limits<int>::max();
constexpr Location  noRoute{infinity,infinity};

struct Element
{
    bool        blocked{};
    
    int         distance{infinity};
    Location    previous{noRoute};
};


struct Fringe
{
    int         distance;
    Location    location;
    
    auto operator<=>(Fringe const &rhs) const noexcept = default;

};


std::mutex                  gridLock;
Element                     grid[dim][dim];
std::priority_queue<Fringe> fringe;



void routeThread()
{

}


void fillBitmapGrid()
{
    static int maxDistance = static_cast<int>(std::sqrt(dim*dim));

// grid
    for(int row=0;row<dim;row++)
    {
        for(int column=0;column<dim;column++)
        {
            if(grid[row][column].blocked)
            {
                bitmapData[row][column]=Colour::blocked;
            }
            else
            {
                if(grid[row][column].distance == infinity)
                {
                    bitmapData[row][column]=Colour::background;
                }
                else
                {
                    bitmapData[row][column]=  grid[row][column].distance * 250 / maxDistance;
                }
            }
        }
    }                                                                       
}

void fillBitmapRoute(Location  walk)
{
    bitmapData[walk.row][walk.column]=Colour::route;

    auto previous = grid[walk.row][walk.column].previous;

    if(previous!=noRoute)
    {
        fillBitmapRoute(previous);
    }
}


void fillBitmap()
{
    std::unique_lock    _{gridLock};

    fillBitmapGrid();


// route

    if(grid[finish.row][finish.column].distance != infinity)
    {
        // final route
        fillBitmapRoute(finish);
    }
    else if(!fringe.empty())
    {
        // best route so far
        fillBitmapRoute(fringe.top().location);
    }
    else
    {
        // huh?
    }

    bitmapData[start.row][start.column]  =Colour::startStop;
    bitmapData[finish.row][finish.column]=Colour::startStop;
}



void startRouting()
{
    for(int row=dim/2-10;row<dim/2+10;row++)
    {
        for(int column=dim/2-10;column<dim/2+10;column++)
        {
            grid[row][column].blocked=true;

        }
    }

    std::thread{routeThread}.detach();
}


/*
void pathToNextUnknown()
{
    using PriorityQueue = std::priority_queue<Node,std::vector<Node>,NodeCompare>;

    auto                        queue{PriorityQueue{}};

    for(auto &tile : room)
    {
        tile.second.distance = infinity;
        tile.second.visited  = false;
        tile.second.previous = Location{0,0};
    }

    room[currentLocation].distance=0;

    queue.push( Node{currentLocation,0});

    while(!queue.empty())       
    {
        auto const current=queue.top();
        queue.pop();

        if(   room[current.location].visited
            || room[current.location].contents == Contents::wall)
        {
            continue;
        }

        room[current.location].visited=true;

        if(room[current.location].contents==Contents::unexplored)
        {
            // found somewhere to walk to.
            // save the moves to get there in the moves list and bail
            Location  walk={current.location};

            while(walk != currentLocation)
            {
                auto nextWalk=room[walk].previous;

                moves.push( directionFromDestination(walk, nextWalk));

                walk=nextWalk;
            }
            break;
        }
        else
        {
            auto neighbours = getNextNeighbourForPathing(current.location);

            for(auto neighbour : neighbours)
            {
                if(! room[neighbour].visited)
                {
                    auto newDistance = current.distance + 1;

                    if(newDistance < room[neighbour].distance)
                    {
                        room[neighbour].distance = newDistance;
                        room[neighbour].previous = current.location;

                        queue.push( Node{ room[neighbour].location,  room[neighbour].distance});
                    }
                }
            }
        }
    }
}



/*


#include <cassert>
#include <cstdint>


#include <iostream>
#include <sstream>
#include <format>

//#include <algorithm>
//#include <numeric>
//#include <ranges>

#include <thread>
#include <chrono>

using namespace std::literals;
#include "include/thrower.h"
#include "include/intcode.h"
#include "include/console.h"




class Drone
{
    constexpr static auto infinity = std::numeric_limits<int>::max();

    enum class Contents
    {
        floor,
        wall, 
        unexplored,
        target,
        willBeOxygen,
        oxygen
    };

    struct Location
    {
        int row;
        int column;

        auto operator<=>(Location const &rhs) const noexcept = default;
    };


    enum class Direction
    {
        up                  =1,
        down,
        left,
        right
    };

    struct Tile
    {
        // persistent            
        Contents    contents;
        Location    location{};

        // per-walk
        int         distance{infinity};
        Location    previous{};         // only needed if we need the route
        bool        visited{false};
    };

    struct Node
    {
        Location    location{};
        int         distance{infinity};
    };


    struct NodeCompare
    {
        bool operator()(Node const &lhs, Node const &rhs)
        {
            return lhs.distance > rhs.distance;
        }
    };





    std::map<Location,Tile>         room{ 
                                            {  Location{0,0}, Tile{Contents::floor, Location{0,0}} }
                                        };    


    Location                        currentLocation{0,0};
    Location                        nextLocation   {0,0};
    Location                        oxygenLocation {0,0};
    std::stack<Direction>           moves;

    IntCode::Interpreter<Drone>     ici;
    Console                         console;

public:


    Drone(IntCode::Program program) : ici{std::move(program), *this }
    {
        markNeighbours();
        console.clear();
        draw();
    }

    void run()
    {
        ici.run();
    }



    bool spreadOxygen()
    {
        bool  foundFloor{};

        for(auto &tile : room)
        {
            if(tile.second.contents == Contents::oxygen)
            {
                std::array              candidates
                {
                    locationFromMove(tile.second.location,Direction::up),
                    locationFromMove(tile.second.location,Direction::down),
                    locationFromMove(tile.second.location,Direction::left),
                    locationFromMove(tile.second.location,Direction::right)
                };

                for(auto candidate : candidates)
                {
                    if(room[candidate].contents==Contents::floor)
                    {
                        room[candidate].contents = Contents::willBeOxygen;
                        foundFloor=true;
                    }
                }
            }
        }

        if(!foundFloor)
        {
            return false;
        }

        for(auto &tile : room)
        {
            if(tile.second.contents == Contents::willBeOxygen)
            {
                tile.second.contents = Contents::oxygen;

                auto pos = adjust(tile.first);
                console.goTo(pos.row,pos.column);
                std::cout << "O";
                }
        }

        return true;        
    }


    void oxygen()
    {
        currentLocation={};
        clearMoves();
        draw();
        pathToNextUnknown();        // calculates distances to all room locations

        console.goTo(1,1);
        std::cout << "Part 1 : " << room[oxygenLocation].distance << "\n";


        room[oxygenLocation].contents=Contents::oxygen;
        int minutes{};

        while(spreadOxygen())   // draws updates itself
        {
            minutes++;
            std::this_thread::sleep_for(10ms);
        }

        console.goTo(2,1);
        std::cout << "Part 2 : " << minutes << "\n";

        draw();

    }


    IntCode::Word input()
    {
        if(moves.empty())
        {
            pathToNextUnknown();
        }

        if(!moves.empty())
        {
            auto move = moves.top();
            moves.pop();

            nextLocation=locationFromMove(currentLocation,move);

            return static_cast<IntCode::Word>(move);
        }
        else
        {
            //nowhere to walk through - abort drone
            draw();
            return -1;
        }
    }

    void output(IntCode::Word output)
    {
        processDroneResponse(static_cast<Response>(output));
    }

private:

    static Location locationFromMove(Location location, Direction direction)
    {
        switch(direction)
        {
        case Direction::up:    return { location.row-1, location.column};
        case Direction::down:  return { location.row+1, location.column};
        case Direction::left:  return { location.row  , location.column-1};
        case Direction::right: return { location.row  , location.column+1};
        default:                throw_runtime_error("Bad direction");
        }
    }

    static Direction directionFromDestination(Location to, Location from)
    {
        if(to.row    == from.row   -1) return Direction::up;
        if(to.row    == from.row   +1) return Direction::down;
        if(to.column == from.column-1) return Direction::left;
        if(to.column == from.column+1) return Direction::right;

        throw_runtime_error("no move");
    }


////////////////
// Dijkstra

    auto getNextNeighbourForPathing(Location currentStep)
    {
        std::vector<Location>   neighbours;
        std::array              candidates
        {
            locationFromMove(currentStep,Direction::up),
            locationFromMove(currentStep,Direction::down),
            locationFromMove(currentStep,Direction::left),
            locationFromMove(currentStep,Direction::right)
        };

        for(auto candidate : candidates)
        {
            if(   room.contains(candidate)
               && room[candidate].contents != Contents::wall)
            {
                neighbours.emplace_back(candidate);
            }
        }

        return neighbours;
    }




// Dijkstra
////////////////



private:

    void markNeighbours()
    {
        auto up    = locationFromMove(currentLocation,Direction::up);
        auto down  = locationFromMove(currentLocation,Direction::down);
        auto left  = locationFromMove(currentLocation,Direction::left);
        auto right = locationFromMove(currentLocation,Direction::right);


        room.insert({ up   , {Contents::unexplored,up   }});
        room.insert({ down , {Contents::unexplored,down }});
        room.insert({ left , {Contents::unexplored,left }});
        room.insert({ right, {Contents::unexplored,right}});
    }

    void processDroneResponse(Response response)
    {
        static int updateCount{};

        switch(response)
        {
        case Response::hitWall:
            room[ nextLocation ].contents = Contents::wall;
            clearMoves();
            updateCount++;
            break;

        case Response::moved:

            currentLocation = nextLocation;

            if(room[nextLocation].contents!=Contents::floor)
            {
                room[ nextLocation ].contents = Contents::floor;
                markNeighbours();
                updateCount++;
            }
            break;

        case Response::movedAndFoundTarget:
            room[ nextLocation ].contents = Contents::target;
            currentLocation = nextLocation;
            oxygenLocation  = currentLocation;
            markNeighbours();
            updateCount++;
            break;
        }

        if(updateCount==50)
        {
            updateCount=0;
            draw();
        }
    }

    void clearMoves()
    {
        while(!moves.empty())
        {
            moves.pop();
        }
    }

    Location adjust(Location location)
    {
        return Location{ location.row+30, location.column+30};
    };

    void draw()
    {

 
        for(auto const &tile : room)
        {
            auto pos = adjust(tile.first);
            console.goTo(pos.row,pos.column);

            switch(tile.second.contents)
            {
            case Contents::floor:
                std::cout << char(176);
                break;

            case Contents::wall:
                std::cout << char(219);
                break;

            case Contents::unexplored:
                std::cout << '?';
                break;

            case Contents::target:
                std::cout << 'o';
                break;

            case Contents::oxygen:
                std::cout << 'O';
                break;
            }


            pos = adjust(currentLocation);
            console.goTo(pos.row,pos.column);
            std::cout << '@';
        }
    }
};


int main()
try
{
    extern std::istringstream data;

    auto program = IntCode::programFromStream(data);

    Drone   drone{program };
    drone.run();

    // drone has explored all of room
    drone.oxygen();




    return 0;
}
catch(std::exception const &e)
{
    std::cout << e.what() << '\n';
}

*/