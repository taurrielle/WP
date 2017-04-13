#if defined(UNICODE) && !defined(_UNICODE)
#endif

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>

#include "header.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
static Objects *objects[200], *objects2[200];
static int nrObj2 = 0;


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
               "WP Lab4",       /* Title Text */
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
    static POINT leftCorner = {50, 50};
    static POINT dimensions = {80, 80};
    static HBRUSH hBrush;
    static Objects circle(leftCorner, dimensions, 3, RGB(0, 0, 0)),
           circle2(leftCorner, dimensions, 10, RGB(255, 0, 0)),
           circle3 (leftCorner, dimensions, 15, RGB(0, 0, 255));
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

    case WM_LBUTTONDOWN:
    {
        POINT coord, dimensionsPt;
        coord.x = LOWORD(lParam);
        coord.y = HIWORD(lParam);
        dimensionsPt.x = coord.x + 30;
        dimensionsPt.y = coord.y + 30;

        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;

        objects[nrObj] = new Objects(coord, dimensionsPt, 5, RGB(r, g, b));
        nrObj++;
        break;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd,&ps);
        GetClientRect(hwnd,&rect);

        FillRect(hdcMem, &rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

        circle.Move(hdcMem, rect, hBrush);
        circle2.Move(hdcMem, rect, hBrush);
        circle3.Move(hdcMem, rect, hBrush);

        for(int i = 0; i < nrObj; i++)
        {
            objects[i] -> Move(hdcMem, rect, hBrush);
        }

        onInteraction(circle, circle2, nrObj2);
        onInteraction(circle, circle3, nrObj2);
        onInteraction(circle2, circle3, nrObj2);

        for(int i = 0; i < nrObj - 1; i++)
        {
            for(int j = i + 1; j < nrObj; j++)
            {
                onInteraction2(*objects[i],*objects[j]);
            }
        }

        for(int i = 0; i < nrObj2; i++)
        {
            objects2[i] -> Move(hdcMem, rect, hBrush);
        }

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        EndPaint(hwnd,&ps);

        break;
    }

    case WM_MOUSEWHEEL:
    {
        if((short)HIWORD(wParam) < 0)
        {
            circle.decelerate(1);
            circle2.decelerate(1);
            circle3.decelerate(1);
            for(int i = 0; i < nrObj; i++)
            {
                objects[i] -> decelerate(1);
            }

            for(int i = 0; i < nrObj2; i++)
            {
                objects2[i] -> decelerate(1);
            }
        }
        else
        {
            circle.accelerate(1);
            circle2.accelerate(1);
            circle3.accelerate(1);
            for(int i = 0; i < nrObj; i++)
            {
                objects[i] -> accelerate(1);
            }

            for(int i = 0; i < nrObj2; i++)
            {
                objects2[i] -> accelerate(1);
            }
        }
        break;
    }

    case WM_KEYDOWN:
    {
        switch(wParam)
        {
        case VK_DOWN:
        {
            circle.decelerate(1);
            circle2.decelerate(1);
            circle3.decelerate(1);
            for(int i = 0; i < nrObj; i++)
            {
                objects[i] -> decelerate(1);
            }

            for(int i = 0; i < nrObj2; i++)
            {
                objects2[i] -> decelerate(1);
            }
            break;
        }

        case VK_UP:
        {
            circle.accelerate(1);
            circle2.accelerate(1);
            circle3.accelerate(1);
            for(int i = 0; i < nrObj; i++)
            {
                objects[i] -> accelerate(1);
            }

            for(int i = 0; i < nrObj2; i++)
            {
                objects2[i] -> accelerate(1);
            }
            break;
        }

        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
            break;
        }
        KillTimer(hwnd, ID_TIMER);
        SetTimer(hwnd, ID_TIMER, timerSpeed, NULL);
        break;
    }

    case WM_TIMER:
    {
        InvalidateRect(hwnd, NULL, FALSE);
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

Objects::Objects(POINT leftCorner, POINT dimensions, const int &speed, COLORREF color)
{

    this -> leftCorner = leftCorner;
    this -> dimensions = dimensions;
    this -> xSpeed = speed;
    this -> ySpeed = speed;
    this -> color = color;
}

void Objects::Move(const HDC &hdc, const RECT& rect, HBRUSH &hBrush)
{
    hBrush = CreateSolidBrush(color);
    SelectObject(hdc, hBrush);

    this -> leftCorner.x += xSpeed;
    this -> leftCorner.y += ySpeed;
    this -> dimensions.x += xSpeed;
    this -> dimensions.y += ySpeed;

    if(dimensions.x > rect.right-1)
    {
        xSpeed = - abs(xSpeed);
        squareFlag = 1;
    }
    else if(leftCorner.x < 0)
    {
        xSpeed = abs(xSpeed);
        squareFlag = 1;
    }

    if(dimensions.y > rect.bottom-1)
    {
        ySpeed = - abs(ySpeed);
        accelerate(1);
    }
    else if(leftCorner.y < rect.top + 1)
    {
        ySpeed = abs(ySpeed);
        accelerate(1);
    }

    if(squareFlag == 1)
    {
        Rectangle(hdc, leftCorner.x, leftCorner.y, dimensions.x, dimensions.y);
        SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        DeleteObject(hBrush);
    }
    else
    {
        Ellipse(hdc, leftCorner.x, leftCorner.y, dimensions.x, dimensions.y);
        SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        DeleteObject(hBrush);
    }
    return;

}

void Objects::accelerate(const int &delta)
{
    if (xSpeed < 0 && ySpeed < 0)
    {
        this -> xSpeed -= delta;
        this -> ySpeed -= delta;
    }
    else if (xSpeed > 0 && ySpeed > 0)
    {
        this -> xSpeed += delta;
        this -> ySpeed += delta;
    }
    else if (xSpeed > 0 && ySpeed < 0)
    {
        this -> xSpeed += delta;
        this -> ySpeed -= delta;
    }
    else if (xSpeed < 0 && ySpeed > 0)
    {
        this -> xSpeed -= delta;
        this -> ySpeed += delta;
    }
    else if (xSpeed == 0 && ySpeed == 0)
    {
        this -> xSpeed = 1;
        this -> ySpeed = 1;
    }
    return;
}

void Objects::decelerate(const int &delta)
{
    if (xSpeed < 0 && ySpeed < 0)
    {
        this -> xSpeed += delta;
        this -> ySpeed += delta;
    }
    else if (xSpeed > 0 && ySpeed > 0)
    {
        this -> xSpeed -= delta;
        this -> ySpeed -= delta;
    }

    else if (xSpeed > 0 && ySpeed < 0)
    {
        this -> xSpeed -= delta;
        this -> ySpeed += delta;
    }
    else if (xSpeed < 0 && ySpeed > 0)
    {
        this -> xSpeed += delta;
        this -> ySpeed -= delta;
    }

    else if(xSpeed == 0 && ySpeed == 0)
    {
        this -> xSpeed = 0;
        this -> ySpeed = 0;
        return;
    }
    return;
}

void onInteraction(Objects &obj1, Objects &obj2, int &nrObj2)
{
    POINT center1, center2;
    int radius1 = (obj1.dimensions.x - obj1.leftCorner.x) / 2;
    int radius2= (obj2.dimensions.x - obj2.leftCorner.x) / 2;

    center1.x = (obj1.dimensions.x - radius1);
    center1.y = (obj1.dimensions.y - radius1);

    center2.x = (obj2.dimensions.x - radius2);
    center2.y = (obj2.dimensions.y - radius2);

    float distance = sqrt(pow(center1.x - center2.x, 2) + pow(center1.y - center2.y,  2));

    if (distance <= radius1 + radius2)
    {
        obj1.changeColor();
        obj2.changeColor();
        int speed = (rand() % 10) + 1;
        objects2[nrObj2] = new Objects(obj1.leftCorner, obj1.dimensions, - speed, RGB(211, 211, 211));
        nrObj2 ++;
    }
    /*if (distance <= radius1 + radius2 && distance >= radius1 + radius2 - 5)
    {
        int speed = (rand() % 10) + 1;
        objects2[nrObj2] = new Objects(obj1.leftCorner, obj1.dimensions, - speed, RGB(211, 211, 211));
        nrObj2 ++;
    }*/
    return;
}

void onInteraction2(Objects &obj1, Objects &obj2)
{
    POINT center1, center2;
    int radius1 = (obj1.dimensions.x - obj1.leftCorner.x) / 2;
    int radius2= (obj2.dimensions.x - obj2.leftCorner.x) / 2;

    center1.x = (obj1.dimensions.x - radius1);
    center1.y = (obj1.dimensions.y - radius1);

    center2.x = (obj2.dimensions.x - radius2);
    center2.y = (obj2.dimensions.y - radius2);

    float distance = sqrt(pow(center1.x - center2.x, 2) + pow(center1.y - center2.y,  2));

    if (distance <= radius1 + radius2)
    {
        obj1.changeColor();
        obj2.changeColor();
    }
    return;

}

void Objects::changeColor()
{
    //srand(GetTickCount());
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    this -> color = RGB(r, g, b);
}
