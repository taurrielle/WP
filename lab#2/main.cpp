#if defined(UNICODE) && !defined(_UNICODE)
#endif

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ColorScrDlg (HWND, UINT, WPARAM, LPARAM) ;

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
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON)); /* Use default icon and mouse-pointer */
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON));
    wincl.hCursor = LoadCursor (hThisInstance, MAKEINTRESOURCE(ID_CURSOR));
    wincl.lpszMenuName =  MAKEINTRESOURCE(IDM_MENU);
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
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    static HWND scrollBar[3], scrollLabel[3], scrollValue[3], listBox, submitBox,
                buttonAdd;
    static HINSTANCE hInstance;
    static int cxCoord, cyCoord;
    static int xPos, xMin, xMax, i;
    static int color[3] ={0,0,0} ;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {

        for (i = 0; i < 3; i++)
        {
            listBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
                             WS_VISIBLE | WS_CHILD | LBS_NOTIFY,
                             0, 0, 0, 0,
                             hwnd, (HMENU) ID_LISTBOX, GetModuleHandle(NULL), NULL );

            submitBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
                             WS_VISIBLE | WS_CHILD,
                             0, 0, 0, 0,
                             hwnd, (HMENU)ID_SUBMIT, GetModuleHandle(NULL), NULL);

            scrollBar[i] = CreateWindowEx(NULL, TEXT("SCROLLBAR"), TEXT("KNKJNK"),
                                    WS_VISIBLE | WS_CHILD | SBS_HORZ,
                                    0, 0, 0, 0,
                                    hwnd, (HMENU)(i + 109), GetModuleHandle(NULL), NULL);

            scrollLabel[i] = CreateWindow ("static", szColorLabel[i],
                              WS_CHILD | WS_VISIBLE | SS_CENTER ,
                              0, 0, 0, 0,
                              hwnd, (HMENU) (i + 112), hInstance, NULL) ;

             scrollValue[i] = CreateWindow ("static", "0",
                              WS_CHILD | WS_VISIBLE | SS_CENTER,
                              0, 0, 0, 0,
                              hwnd, (HMENU) (i + 115), hInstance, NULL) ;
            xPos = 0;
            xMin = 0;
            xMax = 255;
            SetScrollRange(scrollBar[i], SB_CTL, 0, 255, FALSE);
            SetScrollPos(scrollBar[i], SB_CTL, 0, TRUE);
        }

        RegisterHotKey(hwnd, HK_EXIT, MOD_CONTROL, 0x57); //ctrl+W
        RegisterHotKey(hwnd, HK_HELP, MOD_CONTROL, 0x48); //ctrl+H

        hInst = ((LPCREATESTRUCT)lParam)->hInstance;
        break;
    }

    case WM_SIZE:
    {
        cxCoord = LOWORD(lParam);
        cyCoord = HIWORD(lParam);

        for(i = 0; i < 3; i++)
        {
            MoveWindow(listBox, cxCoord - 625, cyCoord - 350, 300, 300, TRUE);
            MoveWindow(submitBox, cxCoord - 625, cyCoord - 390, 300, 25, TRUE);
            MoveWindow(scrollBar[i], cxCoord - 260, 40 * i + 100, 225, 20, TRUE);
            MoveWindow(scrollLabel[i], cxCoord - 290, 40 * i + 100, 20, 20, TRUE);
            MoveWindow(scrollValue[i], cxCoord - 30, 40 * i + 100, 20, 20, TRUE);
        }
        break;
    }

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_EXIT:
        {
            exit(1);
        }
        case ID_HELP:
        {
            MessageBox(hwnd, "You need help??? Why?", "Help?", MB_OK | MB_ICONQUESTION);
            break;
        }
        case ID_ABOUT:
        {
            DialogBox(hInstance, MAKEINTRESOURCE(ID_DLGBOX), hwnd, DlgProc);
            break;
        }
        break;
        }
        break;

    case WM_HSCROLL:
    {
        i = GetWindowLong ((HWND) lParam, GWL_ID);
        i = i - 109;

            switch(LOWORD(wParam))
            {
            case SB_LINELEFT:
            {
                color[i] -= 1;
                break;
            }

            case SB_LINERIGHT:
            {
                color[i] += 1;
                break;
            }

            case SB_PAGELEFT:
            {
                color[i] -= 10;
                break;
            }

            case SB_PAGERIGHT:
            {
                color[i] += 10;
                break;
            }

            case SB_TOP:
            {
                color[i] = xMin;
                break;
            }

            case SB_BOTTOM:
            {
                color[i] = xMax;
                break;
            }

            case SB_THUMBPOSITION:


            case SB_THUMBTRACK:
            {
                color[i] = HIWORD(wParam);
                break;
            }

            default:
                break;
            }

            SetScrollPos(scrollBar[i], SB_CTL, color[i], TRUE);
            if(color[i] == xMax)
            {

                EnableScrollBar(scrollBar[i], SB_CTL, ESB_DISABLE_RIGHT);
            }
            else if(color[i] == xMin)
            {
                EnableScrollBar(scrollBar[i], SB_CTL, ESB_DISABLE_LEFT);
            }

            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(color[0], color[1], color[2])));
            InvalidateRect(hwnd, NULL, TRUE);

        break;
    }

        case WM_CTLCOLORSTATIC:
        {
            SetBkColor ((HDC) wParam, RGB(color[0], color[1], color[2]));
            break;

        }

    case WM_HOTKEY:
    {
        switch(wParam)
        {
        case HK_EXIT:
        {
            PostQuitMessage(0);
            break;
        }
        case HK_HELP:
        {
            MessageBox(hwnd, "You need help??? Why?", "Help?", MB_OK | MB_ICONQUESTION);
            break;
        }
        default:
            break;
        }
        break;
    }

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO winSize = (LPMINMAXINFO)lParam;
        winSize->ptMinTrackSize.x = 700;
        winSize->ptMinTrackSize.y = 500;
        //winSize->ptMaxTrackSize.x = 630;
        //winSize->ptMaxTrackSize.y = 425;
        break;
    }

    case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(ID_CURSOR)));
                return TRUE;
            }
            else
            {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
            break;
        }

    case WM_SYSCOMMAND:
    {
        switch(wParam)
        {
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
    }
    break;


    case WM_DESTROY:

        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
    }
    return FALSE;
}

