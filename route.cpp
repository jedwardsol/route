
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


constexpr auto      infinity=std::numeric_limits<double>::max();

constexpr Location  noRoute{-1,-1};

struct Element
{
// contents
    bool        blocked{};

// routing
    bool        visited{false};    
    double      distance{infinity};
    Location    previous{noRoute};
};


struct Fringe
{
    double      cost;
    double      distance;
    Location    location;
    
    auto operator<(Fringe const &rhs) const noexcept 
    {
        return rhs.cost < cost;
    }

};


std::mutex                  gridLock;
Element                     grid[dim][dim];
std::priority_queue<Fringe> fringe;


auto &element(Location location)
{
    return grid[location.row][location.column];
}


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

/*
                    fringe.push( Fringe{ dijkstraCost(neighbour), 
                                         newDistance,
                                         neighbour});
*/
                    fringe.push( Fringe{ aStarCost(neighbour), 
                                         newDistance,
                                         neighbour});

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


void fillBitmapGrid()
{
    double maxDistance = 1;

    for(int row=0;row<dim;row++)
    {
        for(int column=0;column<dim;column++)
        {
            if(grid[row][column].distance != infinity)
            {
                maxDistance = std::max(maxDistance,grid[row][column].distance);
            }
        }
    }                                                                       



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
                    bitmapData[row][column] = Colour::background;
                }
                else
                {
                    bitmapData[row][column] = grid[row][column].distance * 250 / maxDistance;
                }
            }
        }
    }                                                                       
}

void fillBitmapRoute(Location  walk)
{
    bitmapData[walk.row][walk.column]=Colour::route;

    auto previous = element(walk).previous;

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

    if(element(finish).distance != infinity)
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


void addObstacle(int top, int left, int bottom, int right)
{
    for(int row=top;row<bottom;row++)
    {
        for(int column=left;column<right;column++)
        {
            grid[row][column].blocked=true;
        }
    }
}



void addRandomObstacle()
{
    static std::mt19937                         rng{std::random_device{}()};
    static std::uniform_int_distribution        loc{0,dim-20};

    auto top =loc(rng);
    auto left=loc(rng);

    if(    top  < start.row
       &&  top  > start.row - 20
       &&  left < start.column
       &&  left > start.column-20)
    {
        return;
    }

    if(    top  < finish.row
       &&  top  > finish.row - 20
       &&  left < finish.column
       &&  left > finish.column-20)
    {
        return;
    }

    addObstacle(top,left,top+20,left+20);

}


void addRandomObstacles()
{
    for(int i=0;i<1500;i++)
    {
        addRandomObstacle();
    }
}



void addGates()
{
    addObstacle(  0, 50, 
                900, 60);

    addObstacle( 100, 500, 
                1000, 510);

}


void addTrap()
{
    addObstacle( 10, 800, 
                900, 810);

    addObstacle(800 , 10, 
                810, 900);

}



void startRouting()
{
    std::unique_lock    _{gridLock};

//    addRandomObstacles();
//    addGates();

    addTrap();

    element(start).distance=0;
    fringe.push( Fringe{0, 0,start});

    std::thread{routeThread}.detach();
}


