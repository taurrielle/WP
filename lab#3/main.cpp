#if defined(UNICODE) && !defined(_UNICODE)
#endif

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
bool IsInArea(POINT point, RECT rect, int excess);
bool IsInEraserArea(POINT point, RECT rect, int eraserWeight);

int getValue(HWND input);
POINT AdjustDrawLimits(POINT ptMouse, RECT limit, int stroke);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";
static char* szColorLabel[] = {"R", "G", "B"};
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
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW); /* Use Windows's default colour as the background of the window */

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
               WS_OVERLAPPEDWINDOW | WS_OVERLAPPED | WS_CAPTION,
               /* default window */
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
    HDC hdc;
    PAINTSTRUCT ps;
    static RECT rect;
    static HWND buttonRect, buttonLine, buttonBezier, buttonEraser, buttonZoomIn, fillCheck,
           strokeWeight, eraserWidth, buttonPencil, buttonFill, buttonClear, buttonEllipse;
    static HINSTANCE hInstance;
    static int cxCoord, cyCoord, shapeWeight, eraserWeight;
    static RECT drawingArea = {100, 15, 600, 450},
                eraserArea = {100, 15, 600, 450};
    static POINT apt[4], bezierPoints[4];
    static POINT Pt[4] = { {20, 80}, {30, 110}, { 70, 100 }, { 80,  80 } };//for bezier
    static POINT polyPt[4] = {{35, 310}, {25, 340}, {70, 340}, {80, 310}};
    static POINT ptMouse;
    static RECT newRect, newEllipse;
    static POINT newLine, ptPen;
    static COLORREF color;
    static BOOL rectangleFlag = false, lineFlag = false, bezierFlag = false, eraserFlag = false, zoomInFlag = false,
                lButtonFlag = false, ellipseFlag = false, pencilFlag = false, fillFlag = false;
    HDC hdcMem;
    static BITMAP bitmap;
    HBITMAP hbmpImage = NULL;


    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        CreateWindowEx(0, "Button", "Tools",
                       WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                       620, 8, 150, 200,
                       hwnd, (HMENU)0, hInstance, NULL);

        buttonPencil = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Pencil"),
                                    WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_ICON,
                                    710, 30, 36,36,
                                    hwnd, (HMENU) ID_PENCIL, GetModuleHandle(NULL), NULL);

        buttonFill = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Fill in"),
                                    WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                    710, 70, 36, 36,
                                    hwnd, (HMENU) ID_BUTTONFILL, GetModuleHandle(NULL), NULL);

        buttonEraser = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Eraser"),
                                      WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                      710, 110, 36, 36,
                                      hwnd, (HMENU) ID_ERASER, GetModuleHandle(NULL), NULL);

        buttonClear = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Clear"),
                                      WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                      710, 150, 36, 36,
                                      hwnd, (HMENU) ID_CLEAR, GetModuleHandle(NULL), NULL);

        buttonLine = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Line"),
                                    WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                    640, 30, 36, 36,
                                    hwnd, (HMENU) ID_LINE, GetModuleHandle(NULL), NULL);

        buttonRect = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Rectangle"),
                                    WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                    640, 70, 36, 36,
                                    hwnd, (HMENU) ID_RECTANGLE, GetModuleHandle(NULL), NULL);

        buttonEllipse = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Ellipse"),
                                    WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                    640, 110, 36, 36,
                                    hwnd, (HMENU) ID_ELLIPSE, GetModuleHandle(NULL), NULL);

        buttonBezier = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Bezier"),
                                      WS_VISIBLE | WS_CHILD |  BS_PUSHLIKE | BS_ICON,
                                      640, 150, 36, 36,
                                      hwnd, (HMENU) ID_BEZIER, GetModuleHandle(NULL), NULL);



        /*buttonZoomIn = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Zoom in"),
                                      WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                      630, 170, 80, 25,
                                      hwnd, (HMENU) ID_ZOOMIN, GetModuleHandle(NULL), NULL);*/

        CreateWindowEx(0, "Static", "Shape weight",
                       WS_VISIBLE | WS_CHILD,
                       630, 220, 120, 20,
                       hwnd, (HMENU)0, hInstance, NULL);

        strokeWeight = CreateWindowEx(0, "Edit", "1",
                                      WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
                                      725, 220, 30, 20,
                                      hwnd, (HMENU)ID_STROKEWEIGHT, hInstance, NULL);

        CreateWindowEx(0, "Static", "Eraser weight",
                       WS_VISIBLE | WS_CHILD,
                       630, 250, 120, 20,
                       hwnd, (HMENU)0, hInstance, NULL);

        eraserWidth = CreateWindowEx(0, "Edit", "1",
                                     WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
                                     725, 250, 30, 20,
                                     hwnd, (HMENU)ID_ERASERWEIGHT, hInstance, NULL);

        fillCheck = CreateWindowEx(0, "Button", "Fill shape",
                                   WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                   630, 280, 100, 30,
                                   hwnd, (HMENU)ID_FILL, hInstance, NULL);

        CreateWindowEx(0, "Button", "Colors",
                       WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                       620, 310, 140, 140,
                       hwnd, (HMENU)0, hInstance, NULL);

        CreateWindowEx(0, "BUTTON", "Red",
                       WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
                       650, 335, 60, 20,
                       hwnd, (HMENU)ID_RED, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "BUTTON", "Yellow",
                       WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                       650, 355, 65, 20,
                       hwnd, (HMENU)ID_YELLOW, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "BUTTON", "Blue",
                       WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                       650, 375, 60, 20,
                       hwnd, (HMENU)ID_BLUE, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "BUTTON", "Purple",
                       WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                       650, 395, 60, 20,
                       hwnd, (HMENU)ID_PURPLE, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "BUTTON", "Black",
                       WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BST_CHECKED,
                       650, 415, 60, 20,
                       hwnd, (HMENU)ID_BLACK, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "Static", "Samples:",
                       WS_VISIBLE | WS_CHILD,
                       20, 15, 60, 20,
                       hwnd, (HMENU)0, hInstance, NULL);

        Button_SetCheck(GetDlgItem(hwnd, ID_BLACK), BST_CHECKED);
        RegisterHotKey(hwnd, HK_RECT, MOD_CONTROL, 0x52); //ctrl+R
        RegisterHotKey(hwnd, HK_LINE, MOD_CONTROL, 0x4C); //ctrl+R
        hInst = ((LPCREATESTRUCT)lParam)->hInstance;

        static HICON  hIcon = LoadIcon(hInst, MAKEINTRESOURCE(PENCIL_ICON));
        SendMessage(buttonPencil, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(FILL_ICON));
        SendMessage(buttonFill, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(ERASER_ICON));
        SendMessage(buttonEraser, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(CLEAR_ICON));
        SendMessage(buttonClear, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);


        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(LINE_ICON));
        SendMessage(buttonLine, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(RECTANGLE_ICON));
        SendMessage(buttonRect, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(ELLIPSE_ICON));
        SendMessage(buttonEllipse, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(BEZIER_ICON));
        SendMessage(buttonBezier, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        ptMouse.x = LOWORD(lParam);
        ptMouse.y = HIWORD(lParam);

        eraserWeight = getValue(eraserWidth);
        shapeWeight = getValue(strokeWeight);

        if(Button_GetCheck(GetDlgItem(hwnd, ID_RED)) == BST_CHECKED)
        {
            color = RGB(255, 77, 77);
        }
        else if(Button_GetCheck(GetDlgItem(hwnd, ID_YELLOW)) == BST_CHECKED)
        {
            color = RGB(255,245,45);
        }
        else if(Button_GetCheck(GetDlgItem(hwnd, ID_BLUE)) == BST_CHECKED)
        {
            color = RGB(100,149,237);
        }
        else if(Button_GetCheck(GetDlgItem(hwnd, ID_PURPLE)) == BST_CHECKED)
        {
            color = RGB(147,112,219);
        }
        else if(Button_GetCheck(GetDlgItem(hwnd, ID_BLACK)) == BST_CHECKED)
        {
            color = RGB(0, 0, 0);
        }

        if (IsInArea(ptMouse, drawingArea, 0))
        {
            lButtonFlag = true;
            /*if(zoomInFlag == true)
            {
                RECT rct;
                GetClientRect(hwnd, &rct);

                rct.top = rct.top * 1.5;
                rct.bottom = rct.bottom * 1.5;
                rct.right = rct.right * 1.5;
                rct.left = rct.left * 1.5;

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            */
            if(rectangleFlag == true)
            {
                newRect.left = ptMouse.x;
                newRect.top = ptMouse.y;
            }
            if(lineFlag == true)
            {
                newLine = ptMouse;
            }
            if(bezierFlag == true)
            {
                bezierPoints[0] = ptMouse;
            }
            if(ellipseFlag == true)
            {
                newEllipse.left = ptMouse.x;
                newEllipse.top = ptMouse.y;
            }
            if(fillFlag == true)
            {
                hdc = GetDC(hwnd);
                HBRUSH brush = CreateSolidBrush(color);
                FillRect(hdc, &drawingArea, brush);
                DeleteObject(brush);
            }

        }
        else
        {
            lButtonFlag = false;
        }

        if (IsInEraserArea(ptMouse, eraserArea, eraserWeight/2))
        {
            if(eraserFlag == true)
            {
                ptPen = ptMouse;
            }
            if(pencilFlag == true)
            {
                ptPen = ptMouse;
            }
        }
        break;
    }

    case WM_LBUTTONUP:
    {
        ptMouse.x = LOWORD(lParam);
        ptMouse.y = HIWORD(lParam);




        if (lButtonFlag == true)
        {
            ptMouse = AdjustDrawLimits(ptMouse, drawingArea, shapeWeight);
            if(rectangleFlag == true )
            {
                newRect.right    = ptMouse.x;
                newRect.bottom   = ptMouse.y;
                hdc = GetDC(hwnd);
                HPEN strokePen = CreatePen(PS_SOLID, shapeWeight, color);
                HBRUSH fillBrush = (Button_GetCheck(fillCheck) == BST_CHECKED)? CreateSolidBrush(color) : (HBRUSH)GetStockObject(NULL_BRUSH);
                SelectObject(hdc, strokePen);
                SelectObject(hdc, fillBrush);
                Rectangle(hdc, newRect.left, newRect.top, newRect.right, newRect.bottom);
                DeleteObject(strokePen);
            }

            if(lineFlag == true)
            {
                hdc = GetDC(hwnd);
                HPEN strokePen   = CreatePen(PS_SOLID, shapeWeight, color);
                SelectObject(hdc, strokePen);
                MoveToEx(hdc, ptMouse.x, ptMouse.y, NULL);
                LineTo(hdc, newLine.x, newLine.y);
                DeleteObject(strokePen);
            }
            if(bezierFlag == true)
            {
                bezierPoints[1] = ptMouse;
            }

            if(ellipseFlag == true)
            {
                newEllipse.right    = ptMouse.x;
                newEllipse.bottom   = ptMouse.y;
                hdc = GetDC(hwnd);
                HPEN strokePen = CreatePen(PS_SOLID, shapeWeight, color);
                HBRUSH fillBrush = (Button_GetCheck(fillCheck) == BST_CHECKED)? CreateSolidBrush(color) : (HBRUSH)GetStockObject(NULL_BRUSH);
                SelectObject(hdc, strokePen);
                SelectObject(hdc, fillBrush);
                Ellipse(hdc, newEllipse.left, newEllipse.top, newEllipse.right, newEllipse.bottom);
                DeleteObject(strokePen);
            }
        }
        break;
    }

    case WM_RBUTTONDOWN:
    {
        ptMouse.x = LOWORD(lParam);
        ptMouse.y = HIWORD(lParam);
        ptMouse = AdjustDrawLimits(ptMouse, drawingArea, shapeWeight);
        if(bezierFlag == true )//&& IsInArea(ptMouse, drawingArea, 0))
        {
            bezierPoints[2] = ptMouse;
        }
        break;
    }

    case WM_RBUTTONUP:
    {
        ptMouse.x = LOWORD(lParam);
        ptMouse.y = HIWORD(lParam);
        shapeWeight   = getValue(strokeWeight);
        ptMouse = AdjustDrawLimits(ptMouse, drawingArea, shapeWeight);
        if(bezierFlag == true)// && IsInArea(ptMouse, drawingArea, 0))
        {
            bezierPoints[3] = ptMouse;
            hdc = GetDC(hwnd);
            HPEN strokePen   = CreatePen(PS_SOLID, shapeWeight, color);
            SelectObject(hdc, strokePen);
            PolyBezier(hdc, bezierPoints, 4);
            DeleteObject(strokePen);
        }
        break;
    }

    case WM_MOUSEMOVE:
    {
        ptMouse.x = LOWORD(lParam);
        ptMouse.y = HIWORD(lParam);

        if (IsInEraserArea(ptMouse, drawingArea, shapeWeight/2))
        {
            if((pencilFlag == true) && (wParam == MK_LBUTTON))
            {
                hdc = GetDC(hwnd);
                HPEN strokePen = CreatePen(PS_SOLID, shapeWeight, color);
                SelectObject(hdc, strokePen);
                MoveToEx(hdc, ptMouse.x, ptMouse.y, NULL);
                LineTo(hdc, ptPen.x, ptPen.y);
                DeleteObject(strokePen);
                ptPen = ptMouse;
            }
        }

        if (IsInEraserArea(ptMouse, eraserArea, eraserWeight/2))
        {
            if((eraserFlag == true) && (wParam == MK_LBUTTON))
            {
                hdc = GetDC(hwnd);
                HPEN strokePen = CreatePen(PS_SOLID, eraserWeight, RGB(255,255,255));
                SelectObject(hdc, strokePen);
                MoveToEx(hdc, ptMouse.x, ptMouse.y, NULL);
                LineTo(hdc, ptPen.x, ptPen.y);
                DeleteObject(strokePen);
                ptPen = ptMouse;
            }
        }
        return 0;

    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);

        SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0,0,0)));
        SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        Rectangle(hdc, drawingArea.left, drawingArea.top, drawingArea.right, drawingArea.bottom);

        COLORREF lineColor = RGB(255, 77, 77);

        HPEN hpen;
        HPEN linePen = CreatePen(PS_SOLID, 2, lineColor);
        hpen = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, 25, 40, NULL);
        LineTo(hdc, 75, 40);
        Rectangle(hdc, 25, 180, 80, 210);

        lineColor = RGB(255,245,45);
        linePen = CreatePen(PS_SOLID, 3, lineColor);
        hpen = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, 25, 45, NULL);
        LineTo(hdc, 75, 45);
        Pie(hdc, 30, 220, 80, 260, 80, 220, 80, 260);

        lineColor = RGB(60,179,113);
        linePen = CreatePen(PS_SOLID, 4, lineColor);
        hpen = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, 25, 52, NULL);
        LineTo(hdc, 75, 52);
        Ellipse(hdc, 25, 270, 80, 300);

        lineColor = RGB(100,149,237);
        linePen = CreatePen(PS_SOLID, 5, lineColor);
        hpen = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, 25, 59, NULL);
        LineTo(hdc, 75, 59);
        Polygon(hdc, polyPt, 4);

        lineColor = RGB(147,112,219);
        linePen = CreatePen(PS_SOLID, 6, lineColor);
        hpen = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, 25, 67, NULL);
        LineTo(hdc, 75, 67);
        SelectObject(hdc, hpen);
        DeleteObject(linePen);

        linePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HPEN hpen2 = (HPEN)SelectObject(hdc, linePen);
        PolyBezier(hdc, Pt, 4);

        hbmpImage = (HBITMAP)LoadImage(hInstance, "moon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hbmpImage);
        GetObject(hbmpImage, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0, 430, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);
        DeleteObject(hbmpImage);

        RECT rectTemp;
        HBRUSH hBrush;
        rectTemp.left    = 25;
        rectTemp.right   = 80;
        int r1=0,g1=0,b1=0; //Any start color
        int r2=255,g2=0,b2=0; //Any stop color
        for(int i = 0; i < 30; i++)
        {
            int red, green, blue;
            red = r1 + (i * (r2-r1) / 30);
            green = g1 + (i * (g2-g1) / 30);
            blue = b1 + (i * (b2-b1) / 30);
            rectTemp.top    = 110  + i;
            rectTemp.bottom = 110 + i + 1;
            hBrush = CreateSolidBrush(RGB(red, green, blue));
            FillRect(hdc, &rectTemp, hBrush);
            DeleteObject(hBrush);
        }

        rectTemp.left    = 25;
        rectTemp.right   = 80;

        r1=255,g1=0,b1=0; //Any start color
        r2=255,g2=255,b2=255; //Any stop color
        for(int i = 0; i < 30; i++)
        {
            int red, green, blue;
            red = r1 + (i * (r2-r1) / 30);
            green = g1 + (i * (g2-g1) / 30);
            blue = b1 + (i * (b2-b1) / 30);
            rectTemp.top    = 145  + i;
            rectTemp.bottom = 145 + i + 1;
            hBrush = CreateSolidBrush(RGB(red, green, blue));
            FillRect(hdc, &rectTemp, hBrush);
            DeleteObject(hBrush);
        }
        EndPaint(hwnd, &ps);
        break;

    }


    case WM_HOTKEY:
    {
        switch(wParam)
        {
        case HK_RECT:
        {
            rectangleFlag = true;
            lineFlag = false;
            bezierFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            pencilFlag = false;
            ellipseFlag = false;
            fillFlag = false;
            break;
        }
        case HK_LINE:
        {
            rectangleFlag = false;
            lineFlag = true;
            bezierFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            pencilFlag = false;
            ellipseFlag = false;
            fillFlag = false;
            break;
        }
        }
        break;
    }

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_RECTANGLE:
        {
            rectangleFlag = true;
            lineFlag = false;
            bezierFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            pencilFlag = false;
            fillFlag = false;
            break;
        }
        case ID_LINE:
        {
            lineFlag = true;
            rectangleFlag = false;
            bezierFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            pencilFlag = false;
            fillFlag = false;
            break;
        }
        case ID_BEZIER:
        {
            bezierFlag = true;
            lineFlag = false;
            rectangleFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            pencilFlag = false;
            fillFlag = false;
            break;
        }
        case ID_ELLIPSE:
        {
            ellipseFlag = true;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            eraserFlag = false;
            zoomInFlag = false;
            pencilFlag = false;
            fillFlag = false;
            break;
        }
        case ID_ERASER:
        {
            eraserFlag = true;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            pencilFlag = false;
            fillFlag = false;
            break;
        }

        case ID_PENCIL:
        {
            pencilFlag = true;
            eraserFlag = false;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            fillFlag = false;
            break;
        }

        case ID_BUTTONFILL:
        {
            fillFlag = true;
            pencilFlag = false;
            eraserFlag = false;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            break;
        }

        case ID_CLEAR:
        {
            eraserFlag = false;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            zoomInFlag = false;
            ellipseFlag = false;
            pencilFlag = false;
            fillFlag = false;
            InvalidateRect(hwnd, &drawingArea, FALSE);
            InvalidateRect(hwnd, &drawingArea, TRUE);
        }

        case ID_ZOOMIN:
        {
            zoomInFlag = true;
            eraserFlag = false;
            bezierFlag = false;
            lineFlag = false;
            rectangleFlag = false;
            ellipseFlag = false;
            fillFlag = false;
            pencilFlag = false;
            break;
        }

        }
        break;


    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO winSize = (LPMINMAXINFO)lParam;
        winSize->ptMinTrackSize.x = 800;
        winSize->ptMinTrackSize.y = 500;
        winSize->ptMaxTrackSize.x = 800;
        winSize->ptMaxTrackSize.y = 500;
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

bool IsInArea(POINT point, RECT rect, int excess)
{
    if ((point.x - excess > rect.left) && (point.x - excess < rect.right) &&
            (point.y - excess > rect.top) && (point.y - excess < rect.bottom))
    {
        return true;
    }
    else
        return false;
}

bool IsInEraserArea(POINT point, RECT rect, int eraserWeight)
{
    if ((point.x - eraserWeight > rect.left) && (point.x + eraserWeight < rect.right) &&
            (point.y - eraserWeight > rect.top) && (point.y + eraserWeight < rect.bottom))
    {
        return true;
    }
    else
        return false;
}

int getValue(HWND input)
{
    int result;
    char buff[500];
    SendMessage(input, WM_GETTEXT, 500, (LPARAM)buff);
    result = atoi(buff);
    return result;
}

POINT AdjustDrawLimits(POINT ptMouse, RECT limit, int stroke)
{
    static POINT result;
    stroke = stroke / 2 + 1;

    if(ptMouse.x - stroke < limit.left)
    {
        result.x = limit.left + stroke;
    }
    else if(ptMouse.x + stroke > limit.right)
    {
        result.x = limit.right - stroke;
    }
    else result.x = ptMouse.x;

    if(ptMouse.y - stroke < limit.top)
    {
        result.y = limit.top + stroke;
    }
    else if(ptMouse.y + stroke > limit.bottom)
    {
        result.y = limit.bottom - stroke;
    }
    else result.y = ptMouse.y;

    return result;
}
