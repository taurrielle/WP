#if defined(UNICODE) && !defined(_UNICODE)
#endif

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>

#define ID_BUTTON1 101
#define ID_TEXT1 102
#define ID_TEXT2 103
#define ID_BUTTON2 104
#define ID_BUTTON3 105
#define ID_TEXT3 106
#define ID_BUTTON4 107
#define ID_COMBOBOX 108

void checkIfCompOpened()
{
    LPCSTR browser = "Chrome_WidgetWin_1";
    HWND computer = FindWindow( browser, NULL);

    if(computer == NULL)
    {
        MessageBox(NULL, "Wow! You're not wasting time on the Internet. \nWell, that's a first! ", "Amazing", MB_OK |MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL, "Stop browsing the Internet and get to work!!!", "Get to work", MB_OK | MB_ICONERROR);
    }
}
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";
HINSTANCE hInst;
int flag1=0, flag2=0;

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
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);     /* Use default icon and mouse-pointer */
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);  /* Use Windows's default colour as the background of the window */

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
    {
        MessageBox(NULL, TEXT("Error"), szClassName, MB_ICONERROR);
        return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               "WP Lab1",       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
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
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    HFONT hfFont;
    static HWND button1, button2, button3, button4, comboBox, text1, text2, text3;
    static short cxClient, cyClient;
    static bool textFlag = false, redFlag = false, purpleFlag = false, pinkFlag = false, defaultFlag = false;
    int comboIndex;
    static int R, G, B;
    char* fontList[] = {"Cambria", "Arial", "Times New Roman", "Calibri", "Helvetica", "Consolas",
                        "Tahoma", "Courier New", "Georgia", "Baskerville"};

    switch (message)                  /* handle the messages */
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        DrawText(hdc, TEXT ("HELLO"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
        break;

    case WM_CREATE:
    {
        button1 = CreateWindowEx(0, "BUTTON", "Reset",
                                 WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                 0, 0, 0, 0,
                                 hwnd,(HMENU)ID_BUTTON1, GetModuleHandle(NULL), NULL);

        button2 = CreateWindowEx(0, "BUTTON", "OK",
                                 WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                 0, 0, 0, 0,
                                 hwnd,(HMENU)ID_BUTTON2, GetModuleHandle(NULL), NULL);

        button3 = CreateWindowEx(0, "BUTTON", "Click!!!",
                                 WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|BS_OWNERDRAW,
                                 0, 0, 0, 0,
                                 hwnd,(HMENU)ID_BUTTON3, GetModuleHandle(NULL), NULL);

        comboBox = CreateWindowEx(WS_EX_STATICEDGE, "COMBOBOX", "",
                                  CBS_DROPDOWNLIST | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                                  0, 0, 0, 0,
                                  hwnd,(HMENU)ID_COMBOBOX, GetModuleHandle(NULL), NULL);

        button4 = CreateWindowEx(0, "BUTTON", "Change font",
                                 WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                 0, 0, 0, 0,
                                 hwnd,(HMENU)ID_BUTTON4, GetModuleHandle(NULL), NULL);

        text1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Enter your name..."),
                               WS_VISIBLE | WS_CHILD,
                               0, 0, 0, 0,
                               hwnd, (HMENU)ID_TEXT1, GetModuleHandle(NULL), NULL);

        text2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Enter your surname..."),
                               WS_VISIBLE | WS_CHILD,
                               0, 0, 0, 0,
                               hwnd, (HMENU)ID_TEXT2, GetModuleHandle(NULL), NULL);

        text3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
                               WS_VISIBLE | WS_CHILD | ES_MULTILINE |ES_READONLY,
                               0, 0, 0, 0,
                               hwnd, (HMENU)ID_TEXT3, GetModuleHandle(NULL), NULL);

        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)"Select color");
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)"Dark red");
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)"Pretty purple");
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)"Pink");
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)"Pick random");
        SendMessage(comboBox, CB_SETCURSEL, 0, 0);

        hfFont = CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,"Courier New");
        SendMessage(button3, WM_SETFONT, (WPARAM)hfFont, MAKELPARAM(FALSE,0));
        SendMessage(text3, WM_SETFONT, (WPARAM)hfFont, MAKELPARAM(FALSE,0));

        break;
    }

    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_TEXT1)
        {
            SendMessage(text1, WM_SETTEXT, NULL, NULL);
            flag1 = 1;
        }

        if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_TEXT2)
        {
            SendMessage(text2, WM_SETTEXT, NULL, NULL);
            flag2 = 1;
        }

        switch(LOWORD(wParam))
        {
        case ID_COMBOBOX:
            if (HIWORD(wParam) == CBN_SELENDOK)
            {
                comboIndex = SendMessage(comboBox, (UINT) CB_GETCURSEL, 0, 0);
                if(comboIndex == 0)
                {
                    defaultFlag = true;
                    redFlag = textFlag = purpleFlag = pinkFlag = false;
                    InvalidateRect(text3, NULL, TRUE);
                    break;
                }
                else if(comboIndex == 1)
                {
                    redFlag = true;
                    textFlag = defaultFlag = purpleFlag = pinkFlag = false;
                    InvalidateRect(text3, NULL, TRUE);
                    break;
                }
                else if(comboIndex == 2)
                {
                    purpleFlag = true;
                    redFlag = defaultFlag = textFlag = pinkFlag = false;
                    InvalidateRect(text3, NULL, TRUE);
                    break;
                }
                else if(comboIndex == 3)
                {
                    pinkFlag = true;
                    redFlag = defaultFlag = textFlag = purpleFlag = false;
                    InvalidateRect(text3, NULL, TRUE);
                    break;
                }
                else if(comboIndex == 4)
                {
                    srand(GetTickCount());
                    R = rand() % 256;
                    G = rand() % 256;
                    B = rand() % 256;
                    textFlag = true;
                    redFlag = defaultFlag = purpleFlag = pinkFlag = false;

                    InvalidateRect(text3, NULL, TRUE);
                    break;
                }
            }
            break;

        case ID_BUTTON1:      /*reset button*/
        {
            SendMessage(text1, WM_SETTEXT, NULL, (LPARAM)"Enter your name...");
            SendMessage(text2, WM_SETTEXT, NULL, (LPARAM)"Enter your surname...");
            SendMessage(text3, WM_SETTEXT, NULL, NULL);
            flag1 = 0;
            flag2 = 0;
            break;
        }

        case ID_BUTTON2:    /*ok button*/
        {
            char buff[500], buff2[500], result[1000];
            GetWindowText(GetDlgItem(hwnd, ID_TEXT1), buff, 1000);
            GetWindowText(GetDlgItem(hwnd, ID_TEXT2), buff2, 1000);
            strcpy(result, buff);
            strcat(result, " ");
            strcat(result, buff2);
            SendMessage(text3, WM_SETTEXT, NULL, NULL);
            SetWindowText(GetDlgItem(hwnd, ID_TEXT3), result);
            break;
        }

        case ID_BUTTON3:    /*click button*/
        {
            char buff[500], result[1000] = "Hello ";
            GetWindowText(GetDlgItem(hwnd, ID_TEXT3), buff, 500);
            strcat(result, buff);
            strcat(result, "! \nCongrats! You clicked a button! \nNow get the hell out of here!!!");
            MessageBox(NULL, TEXT(result),
                       TEXT("Hello"), MB_OK | MB_ICONWARNING);
            break;
        }

        case ID_BUTTON4:    /*font button*/
        {
            int index;
            index = rand() % 9;
            hfFont = CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,fontList[index]);
            SendMessage(text3, WM_SETFONT, (WPARAM)hfFont, MAKELPARAM(FALSE,0));
            InvalidateRect(text3, NULL, TRUE);
            break;
        }
        }
        break;
    }
    case WM_SIZE:
    {
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        MoveWindow(button1, cxClient/2 + 70, cyClient/2 - 60, 100, 30, TRUE);
        MoveWindow(button2, cxClient/2 - 175, cyClient/2 - 60, 100, 30, TRUE);
        MoveWindow(button3, cxClient/2 - 50, cyClient/2 + 150, 100, 30, TRUE);
        MoveWindow(comboBox, cxClient/2 + 60, cyClient/2 + 40, 110, 200, TRUE);
        MoveWindow(button4, cxClient/2 + 60, cyClient/2 + 85, 110, 30, TRUE);

        MoveWindow(text1, cxClient/2 - 175, cyClient/2 - 170, 350, 25, TRUE);
        MoveWindow(text2, cxClient/2 - 175, cyClient/2 - 120, 350, 25, TRUE);
        MoveWindow(text3, cxClient/2 - 175, cyClient/2 + 40, 210, 80, TRUE);
        break;
    }


    case WM_CTLCOLORSTATIC:
    {
        if(defaultFlag == true && (HWND)lParam == text3)
        {
            HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
            SetTextColor((HDC) wParam, RGB(0,0,0));
            return (BOOL) hbr;
        }

        else if(textFlag == true && (HWND)lParam == text3)
        {
            HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
            SetTextColor((HDC) wParam, RGB(R,G,B));
            return (BOOL) hbr;
        }

        else if(redFlag == true && (HWND)lParam == text3)
        {
            HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
            SetTextColor((HDC) wParam, RGB(153, 0, 0));
            return (BOOL) hbr;
        }

        else if(purpleFlag == true && (HWND)lParam == text3)
        {
            HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
            SetTextColor((HDC) wParam, RGB(153, 115, 255));
            return (BOOL) hbr;
        }

        else if(pinkFlag == true && (HWND)lParam == text3)
        {
            HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
            SetTextColor((HDC) wParam, RGB(255, 179, 217));
            return (BOOL) hbr;
        }
        break;
    }

    case WM_CTLCOLOREDIT:
        if(GetDlgCtrlID((HWND)lParam) == ID_TEXT1)
        {
            hdc = (HDC)wParam;                                              //Get handles
            if(flag1)
            {
                SetTextColor(hdc, RGB(0,0,0));                              // Text color
            }
            else
            {
                SetTextColor(hdc, RGB(165, 164, 164));                        // Text color
            }
            hdc = (HDC) wParam;
            SetBkColor(hdc, RGB(255, 237, 237));
            return (INT_PTR)CreateSolidBrush(RGB(255, 237, 237));
        }

        else if(GetDlgCtrlID((HWND)lParam) == ID_TEXT2)
        {
            hdc = (HDC)wParam;                                              //Get handles
            if(flag2)
            {
                SetTextColor(hdc, RGB(0,0,0));                              // Text color
            }
            else
            {
                SetTextColor(hdc, RGB(165, 164, 164));                        // Text color
            }
            hdc = (HDC) wParam;
            SetBkColor(hdc, RGB(255, 237, 237));
            return (INT_PTR)CreateSolidBrush(RGB(255, 237, 237));
        }
        break;

    case WM_DRAWITEM:
        if ((UINT)wParam == ID_BUTTON3)
        {
            LPDRAWITEMSTRUCT lpdis = (DRAWITEMSTRUCT*)lParam;
            SIZE size;
            GetTextExtentPoint32(lpdis->hDC, "Click!!!", strlen("Click!!!"), &size);
            SetTextColor(lpdis->hDC, RGB(0, 0, 0));
            SetBkColor(lpdis->hDC, RGB(211, 141, 141));

            ExtTextOut(
                lpdis->hDC,
                ((lpdis->rcItem.right - lpdis->rcItem.left) - size.cx) / 2,
                ((lpdis->rcItem.bottom - lpdis->rcItem.top) - size.cy) / 2,
                ETO_OPAQUE | ETO_CLIPPED,
                &lpdis->rcItem,
                "Click!!!",
                strlen("Click!!!"),
                NULL);

            DrawEdge(
                lpdis->hDC,
                &lpdis->rcItem,
                (lpdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED ),
                BF_RECT);
            return TRUE;
        }

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO winSize = (LPMINMAXINFO)lParam;
        winSize->ptMinTrackSize.x = 500;
        winSize->ptMinTrackSize.y = 500;
        //winSize->ptMaxTrackSize.x = 630;
        //winSize->ptMaxTrackSize.y = 425;
        break;
    }

    case WM_SYSCOMMAND:
    {
        switch(wParam)
        {
        case SC_MINIMIZE:
        {
            checkIfCompOpened();
            break;
        }

        case SC_MAXIMIZE:
        {
            srand(GetTickCount());
            int xPos = rand() % 800;
            int yPos = rand() % 700;
            SetWindowPos( hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
            break;
        }

        case SC_CLOSE:
        {
            if(MessageBox(hwnd, "Are you sure you want to quit?", "Close?", MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                exit(1);
            }
            break;
        }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
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

