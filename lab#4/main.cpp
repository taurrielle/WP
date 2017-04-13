#if defined(UNICODE) && !defined(_UNICODE)
#endif

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";
static HINSTANCE hInst;
HWND hDlgModeless = NULL;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION); /* Use default icon and mouse-pointer */
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName =  NULL;
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1); /* Use Windows's default colour as the background of the window */

    if (!RegisterClassEx (&wincl))     /* Register the window class, and if it fails quit the program */
    {
        MessageBox(NULL, TEXT("Error"), szClassName, MB_ICONERROR);
        return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               "WP Lab2",       /* Title Text */
               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               500,                 /* The programs width */
               500,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }

    }
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc, hdcMem;
    static HBITMAP hbmMem;
    static HANDLE hOld;

    PAINTSTRUCT ps;
    static RECT rect;
    static HWND scrollBar;
    static HINSTANCE hInstance;
    static int cxCoord, cyCoord;
    static int xPos, xMin, xMax, i, index,iVertPos;
    static int timerSpeed = 10, nrObj = 0;
    static RECT ellipse = {0,0,100,100};
    static POINT leftCorner = {50, 50};
    static POINT dimensions = {75, 75};
    static HBRUSH hBrush;
    static Objects circle(leftCorner, dimensions, 3);
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        hdc = GetDC(hwnd);
        GetClientRect(hwnd,&rect);

        hdcMem = CreateCompatibleDC(hdc);
        hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        hOld = SelectObject(hdcMem,hbmMem);

        SetTimer(hwnd, ID_TIMER, timerSpeed, NULL);

        break;
    }

    case WM_SIZE:
    {
        SelectObject(hdcMem, hOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        hdc = GetDC(hwnd);
        GetClientRect(hwnd, &rect);

        hdcMem = CreateCompatibleDC(hdc);
        hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        hOld = SelectObject(hdcMem, hbmMem);
        break;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd,&ps);
        GetClientRect(hwnd,&rect);

        FillRect(hdcMem, &rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

        circle.Move(hdcMem, rect, hBrush);
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        EndPaint(hwnd,&ps);

        break;
    }

    case WM_MOUSEWHEEL:
    {
        if((short)HIWORD(wParam) < 0)
        {
            timerSpeed += 10;
        }
        else
        {
            timerSpeed -= 10;
            if (timerSpeed < 0)
            {
                timerSpeed = 0;
            }
        }
        KillTimer(hwnd, ID_TIMER);
        SetTimer(hwnd, ID_TIMER, timerSpeed, NULL);
        break;
    }

    case WM_KEYDOWN:
    {
        switch(wParam)
        {
        case VK_DOWN:
        {
            timerSpeed += 30;
            break;
        }

        case VK_UP:
        {
            timerSpeed -= 30;

            if (timerSpeed < 0)
            {
                timerSpeed = 0;
            }
            break;
        }

        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
            break;
        }

        KillTimer(hwnd,ID_TIMER);
        SetTimer(hwnd,ID_TIMER,timerSpeed,NULL);
        break;
    }

    case WM_TIMER:
    {
        InvalidateRect(hwnd,NULL,FALSE);
        break;
    }

    case WM_COMMAND:

        break;


    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO winSize = (LPMINMAXINFO)lParam;
        winSize->ptMinTrackSize.x = 500;
        winSize->ptMinTrackSize.y = 500;
        //winSize->ptMaxTrackSize.x = 630;
        //winSize->ptMaxTrackSize.y = 425;
        break;
    }


    case WM_DESTROY:

        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

Objects::Objects(POINT leftCorner, POINT dimensions, const int &speed)
{

    this -> leftCorner = leftCorner;
    this -> dimensions = dimensions;
    this -> xSpeed = speed;
    this -> ySpeed = speed;
    color = RGB(0,255,0);

}
bool Objects::Move(const HDC &hdc, const RECT& rect, HBRUSH &hBrush)
{
    hBrush = CreateSolidBrush(RGB(0,0,0));
    SelectObject(hdc, hBrush);


    leftCorner.x += xSpeed;
    leftCorner.y += ySpeed;
    dimensions.x += xSpeed;
    dimensions.y += ySpeed;

    if(dimensions.x > rect.right-1)
    {
        xSpeed = - abs(xSpeed);
    }
    else if(leftCorner.x < 0)
    {
        xSpeed = abs(xSpeed);
    }

    if(dimensions.y > rect.bottom-1)
    {
        ySpeed = - abs(ySpeed);
    }
    else if(leftCorner.y < rect.top + 1)
    {
        ySpeed = abs(ySpeed);
    }

    Ellipse(hdc, leftCorner.x, leftCorner.y, dimensions.x, dimensions.y);
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    DeleteObject(hBrush);
    return TRUE;

}
