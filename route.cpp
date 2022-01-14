#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "print.h"


LRESULT CALLBACK proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    switch(m)
    {
    case WM_CREATE:
    {
        RECT r;
        GetClientRect(h,&r);

        print("{},{} - {},{}\n",r.left,r.top, r.right,r.bottom);

        ShowWindow(h,SW_SHOW);
        return 0;
    }

    }


    return DefWindowProcA(h,m,w,l);
}

int main()
{
    WNDCLASSA   wndClass{};

    wndClass.lpfnWndProc   = proc;
    wndClass.lpszClassName = "route";

    if(!RegisterClassA(&wndClass))
    {
        return 0;
    }

    RECT    client{0,0,1000,1000};
    AdjustWindowRect(&client,WS_OVERLAPPEDWINDOW,false);

    CreateWindowA(wndClass.lpszClassName,
                  "route",
                  WS_OVERLAPPEDWINDOW,
                  CW_USEDEFAULT,CW_USEDEFAULT,
                  client.right-client.left,client.bottom-client.top,
                  nullptr,
                  nullptr,nullptr,nullptr);

    MSG msg;                  

    while(GetMessage(&msg,nullptr,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;
}
