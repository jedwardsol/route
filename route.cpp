
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


std::mutex                  gridLock;
Element                     grid[dim][dim];
std::priority_queue<Fringe> fringe;

bool visitable(Location where)
{
    if(    where.row    < 0
       ||  where.column < 0
       ||  where.row    >= dim
       ||  where.column >= dim)
    {
        return false;    
    }

    if(   element(where).blocked
       || element(where).visited)
    {
        return false;
    }

    return true;
}

auto getNeighbours(Location here)
{
    std::vector<Location> neighbours;

    Location  up   {here.row-1,here.column};
    Location  down {here.row+1,here.column};
    Location  left {here.row,  here.column-1};
    Location  right{here.row,  here.column+1};

    if(visitable(up   )) neighbours.push_back(up   );
    if(visitable(down )) neighbours.push_back(down );
    if(visitable(left )) neighbours.push_back(left );
    if(visitable(right)) neighbours.push_back(right);

    return neighbours;
}


auto getNeighboursDiagonal(Location here)
{
    std::vector<Location> neighbours;

    for(auto drow : {-1,0,+1})
    {
        for(auto dcol : {-1,0,+1})
        {
            Location  neighbour{here.row+drow,here.column+dcol};
            if(visitable(neighbour)) neighbours.push_back(neighbour);
        }
    }

    return neighbours;
}


double dijkstraCost(Location location)
{
    return  element(location).distance;
}

double aStarCost(Location location)
{
    auto drow = finish.row    - location.row;
    auto dcol = finish.column - location.column;

    auto crow = sqrt(drow*drow+dcol*dcol);

    return  element(location).distance + crow;
}



void takeSomeSteps()
{

    std::unique_lock    _{gridLock};

    for(int i=0;i<10000;i++)
    {
        if(!fringe.empty())
        {
            auto const current=fringe.top();
            fringe.pop();

            if(element(current.location).visited)
            {
                continue;
            }

            element(current.location).visited = true;

            if(current.location == finish)
            {
                while(!fringe.empty())
                {
                    fringe.pop();
                }
                return;                                     
            }


//            auto neighbours = getNeighbours(current.location);
            auto neighbours = getNeighboursDiagonal(current.location);

            for(auto neighbour : neighbours)
            {
                auto drow = neighbour.row-current.location.row;
                auto dcol = neighbour.column- current.location.column;

                double newDistance;
                if(drow && dcol)
                {
                    newDistance = current.distance + sqrt(2.0);
                }
                else
                {
                    newDistance = current.distance + 1 ;
                }
    

                if(newDistance < element(neighbour).distance)
                {
                    element(neighbour).distance = newDistance;
                    element(neighbour).previous = current.location;

                    if(false)
                    {
                        fringe.push( Fringe{ dijkstraCost(neighbour), 
                                             newDistance,
                                             neighbour});
                    }
                    else
                    {
                        fringe.push( Fringe{ aStarCost(neighbour), 
                                             newDistance,
                                             neighbour});
                    }
                }
            }
        }
    }
}


void routeThread()
{
    while(!fringe.empty())
    {
        takeSomeSteps();
        std::this_thread::sleep_for(100ms);
    }
}




void startRouting()
{
    std::unique_lock    _{gridLock};

    initGrid();


    element(start).distance=0;
    fringe.push( Fringe{0, 0,start});

    std::thread{routeThread}.detach();
}

void endRouting()
{
    std::unique_lock    _{gridLock};
}
