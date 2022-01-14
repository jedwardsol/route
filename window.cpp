#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cassert>
#include <thread>
#include <chrono>
using namespace std::literals;
#include <system_error>

#include "print.h"
#include "bitmap.h"
#include "window.h"
#include "route.h"

namespace
{
HWND                theWindow   {};
constexpr int       WM_REFRESH  {WM_APP};
constexpr auto      windowStyle { WS_OVERLAPPED | WS_CAPTION  | WS_SYSMENU  | WS_VISIBLE    };
constexpr int       scale       {1};


void drawThread()
{
    while(!done)
    {
        fillBitmap();
        PostMessage(theWindow,WM_REFRESH,0,0);
        std::this_thread::sleep_for(10ms);
    }
}



void paint(HWND h,WPARAM w, LPARAM l)
{
    PAINTSTRUCT paint;
    BeginPaint(h,&paint);
    
    StretchDIBits(paint.hdc,
                  0,0,
                  dim*scale,dim*scale,
                  0,0,
                  dim,dim,
                  bitmapData,
                  bitmapHeader,
                  DIB_RGB_COLORS,
                  SRCCOPY);

    
    EndPaint(h,&paint);
}

LRESULT CALLBACK proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    switch(m)
    {
    case WM_CREATE:
    {        
        RECT    client{0,0, dim*scale, dim*scale};

        AdjustWindowRect(&client,windowStyle,FALSE);
        SetWindowPos(h, nullptr,0,0, client.right-client.left, client.bottom-client.top, SWP_NOMOVE|SWP_NOZORDER);
        GetClientRect(h,&client);
        assert(client.right  - client.left == dim*scale);
        assert(client.bottom - client.top  == dim*scale);

        return 0;
    }

    case WM_CLOSE:
        done=true;
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        paint(h,w,l);
        return 0;

    case WM_REFRESH:
        InvalidateRect(h,nullptr,FALSE);
        return 0;
    
    case WM_NCHITTEST:
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_SETCURSOR:
        break;

    default:
        //print("msg {:#x}\n",m);
        break;
    }

    return DefWindowProc(h,m,w,l);
}


}


void createWindow()
{
    WNDCLASS    Class
    {
        CS_OWNDC,
        proc,
        0,
        0,
        GetModuleHandle(nullptr),
        nullptr,
        LoadCursorA(nullptr,IDC_ARROW),
        nullptr,
        nullptr,
        "bitmapWindowClass"
    };

    if(!RegisterClass(&Class))
    {
        throw std::system_error{ static_cast<int>(GetLastError()), std::system_category(), "RegisterClass"};
    }

    theWindow = CreateWindow(Class.lpszClassName,
                             "route",
                             windowStyle,
                             CW_USEDEFAULT,CW_USEDEFAULT,
                             10,10,
                             nullptr,
                             nullptr,
                             GetModuleHandle(nullptr),
                             nullptr);

    if(theWindow==nullptr)
    {
        throw std::system_error{ static_cast<int>(GetLastError()), std::system_category(), "RegisterClass"};
    }

    std::thread{drawThread}.detach();
}



void windowMessageLoop()
{
    MSG     msg;
    while(GetMessage(&msg,0,0,0) > 0)
    {
        DispatchMessage(&msg);
    }
}