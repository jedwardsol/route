#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <exception>

#include "print.h"
#include "route.h"
#include "window.h"

std::atomic_bool  done{false};



int main()
try
{
    createWindow();

    startRouting();

    windowMessageLoop();
}
catch(const std::exception &e)
{
    print("{}\n",e.what());
    MessageBox(nullptr,e.what(),"bitmapWindow",MB_OK|MB_ICONEXCLAMATION);
}
