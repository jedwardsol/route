#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "print.h"


HBITMAP field;

LRESULT CALLBACK proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    switch(m)
    {
            
    

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        auto dc = BeginPaint(h, &ps);

        auto cdc       = CreateCompatibleDC(dc);
        auto oldBitmap = SelectObject(cdc, field);

        BITMAP bitmap;

        GetObject(field, sizeof(bitmap), &bitmap);

        BitBlt(dc,  0, 0, bitmap.bmWidth, bitmap.bmHeight, 
               cdc, 0, 0, SRCCOPY);


        SelectObject(cdc, oldBitmap);
        DeleteDC(cdc);

        EndPaint(h, &ps);
        return 0;
    }

    }


    return DefWindowProcA(h,m,w,l);
}

int main()
{
    field = CreateBitmap(1000,1000,


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
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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
