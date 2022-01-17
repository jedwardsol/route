
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
    for(int i=100;i<dim;i+=100)
    {
        addObstacle(  0, i, 
                    900, i+10);
    
        addObstacle( 100, i+50, 
                    1000, i+60);
    }
}


void addTrap()
{
    addObstacle( 10, 800, 
                900, 810);

    addObstacle(800 , 10, 
                810, 900);

}


void initGrid()
{
    addRandomObstacles();
//    addGates();
//    addTrap();
}
