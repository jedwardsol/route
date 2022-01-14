#include <Windows.h>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std::literals;
#include <exception>
#include <system_error>
#include <cassert>
#include <random>

constexpr   int     dim   = 256;
constexpr   int     scale = 4;

auto windowStyle    { WS_OVERLAPPED     | WS_CAPTION        | WS_SYSMENU      };


HWND                theWindow{};
constexpr int       WM_REFRESH{WM_APP};

// palette entries
constexpr int       background{255};
constexpr int       startStop {254};
constexpr int       route     {253};



unsigned char       data[dim][dim]{};


void drawThread()
{
    for(;;)
    {
        PostMessage(theWindow,WM_REFRESH,0,0);
        std::this_thread::sleep_for(10ms);
    }
}


void routeThread()
{
    for(int x=0;x<dim;x++)
    {
        for(int y=0;y<dim;y++)
        {
            data[x][y]=background;
        }
    }

    for(int i=0;i<dim;i++)
    {
        data[i][i]= i%256;
    }
}


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

    for(int i=0;i<250;i++)
    {
        header->bmiColors[i]= RGBQUAD(i,i,i);
    }
                                         //  B    G  R
    header->bmiColors[background] = RGBQUAD( 40, 20, 20);
    header->bmiColors[startStop]  = RGBQUAD(  0,200,  0);
    header->bmiColors[route]      = RGBQUAD(200,  0,200);




    return header;
}


void paint(HWND h,WPARAM w, LPARAM l)
{
    static auto header = makeHeader();


    PAINTSTRUCT paint;
    BeginPaint(h,&paint);
    
    StretchDIBits(paint.hdc,
                  0,0,
                  dim*scale,dim*scale,
                  0,0,
                  dim,dim,
                  data,
                  header,
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

        ShowWindow(h,SW_SHOW);
        return 0;
    }

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        paint(h,w,l);
        return 0;

    case WM_REFRESH:
        InvalidateRect(h,nullptr,FALSE);
        break;
    }

    return DefWindowProc(h,m,w,l);
}

int main()
try
{


    WNDCLASS    Class
    {
        CS_OWNDC,
        proc,
        0,
        0,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        "bitmapWindowClass"
    };

    if(!RegisterClass(&Class))
    {
        throw std::system_error{ static_cast<int>(GetLastError()), std::system_category(), "RegisterClass"};
    }

    theWindow = CreateWindow(Class.lpszClassName,
                             "bitmap",
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
    std::thread{routeThread}.detach();


    MSG     msg;
    while(GetMessage(&msg,0,0,0) > 0)
    {
        DispatchMessage(&msg);
    }

}
catch(const std::exception &e)
{
    MessageBox(nullptr,e.what(),"bitmapWindow",MB_OK|MB_ICONEXCLAMATION);
}