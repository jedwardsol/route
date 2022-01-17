#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstddef>
#include "bitmap.h"
#include <utility>
#include <mutex>

namespace
{

auto makeHeader()
{
    auto header = reinterpret_cast<BITMAPINFO*>( new  std::byte[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)]);

    header->bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
    header->bmiHeader.biWidth           =  dim;
    header->bmiHeader.biHeight          = -dim;
    header->bmiHeader.biPlanes          =    1;
    header->bmiHeader.biBitCount        =    8;
    header->bmiHeader.biCompression     = BI_RGB;
    header->bmiHeader.biSizeImage       = dim*dim,
    header->bmiHeader.biXPelsPerMeter   = 0;
    header->bmiHeader.biYPelsPerMeter   = 0;
    header->bmiHeader.biClrUsed         = 0;
    header->bmiHeader.biClrImportant    = 0;

    for(int i=0;i<=maxDistance;i++)
    {
        header->bmiColors[i]= RGBQUAD(i,i,i);
    }
                                                  //  B    G    R
    header->bmiColors[Colour::background] = RGBQUAD( 40,  20,  20);
    header->bmiColors[Colour::startStop]  = RGBQUAD(  0, 200,   0);
    header->bmiColors[Colour::route]      = RGBQUAD( 20,  20, 200);
    header->bmiColors[Colour::blocked]    = RGBQUAD( 50,  50, 100);

    return header;
}

}


BITMAPINFO     *bitmapHeader        {makeHeader()};
uint8_t         bitmapData[dim][dim]{};
//                               ^---- must be multiple of 4



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
    
    do
    {
        bitmapData[walk.row][walk.column]=Colour::route;

        walk = element(walk).previous;

    }while(walk !=noRoute);
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
        // no route
    }

    bitmapData[start.row][start.column]  =Colour::startStop;
    bitmapData[finish.row][finish.column]=Colour::startStop;
}


