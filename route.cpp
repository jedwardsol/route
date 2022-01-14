#include <Windows.h>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std::literals;
#include <exception>
#include <system_error>
#include <cassert>
#include <random>

#include "print.h"
#include "route.h"
#include "bitmap.h"
#include "window.h"



void routeThread()
{
    for(int x=0;x<dim;x++)
    {
        for(int y=0;y<dim;y++)
        {
            bitmapData[x][y]=Colour::background;
        }
    }                                                                       

    bitmapData[start.row][start.column]  =Colour::startStop;
    bitmapData[finish.row][finish.column]=Colour::startStop;
}





int main()
try
{
    createWindow();

    std::thread{routeThread}.detach();

    windowMessageLoop();
}
catch(const std::exception &e)
{
    print("{}\n",e.what());
    MessageBox(nullptr,e.what(),"bitmapWindow",MB_OK|MB_ICONEXCLAMATION);
}