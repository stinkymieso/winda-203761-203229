// winda-203761-203229.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "winda-203761-203229.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDA203761203229, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDA203761203229));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDA203761203229));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDA203761203229);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

static void DrawLevitatingPerson(Graphics& g, int x, int y) {
    Pen pen(Color(255, 0, 0, 0), 2); // black pen, 2px wide

    // Head
    g.DrawEllipse(&pen, x, y, 20, 20); // head circle

    int cx = x + 10; // center of the head
    int top = y + 20;

    // Body
    g.DrawLine(&pen, Point(cx, top), Point(cx, top + 25)); // torso

    // Arms (folded, optional)
    g.DrawLine(&pen, Point(cx - 10, top + 10), Point(cx + 10, top + 10));

    // Legs cross-legged: draw an "X" under the body
    g.DrawLine(&pen, Point(cx - 10, top + 25), Point(cx + 10, top + 35));
    g.DrawLine(&pen, Point(cx + 10, top + 25), Point(cx - 10, top + 35));

    // Levitation shadow
    SolidBrush brush(Color(100, 0, 0, 0)); // semi-transparent
    g.FillEllipse(&brush, x, y + 50, 20, 5); // shadow
}


static void wholeshaft(Graphics& g, int lewygoraX, int lewygoraY, int szer_pros, int wys_pros){

    Pen pen(Color(0, 0, 0), 2);

    //prostokat(poruszanie sie windy)
    g.DrawRectangle(&pen, lewygoraX, 0.6 * lewygoraY, szer_pros, 1.25 * wys_pros);

    //rysowanie poziomow windy

    // Linie poziome wychodz¹ce z ka¿dego piêtra po obu stronach

    //LEWA
    g.DrawLine(&pen, Point(0.2 * lewygoraX, lewygoraY + 0.2 * wys_pros), Point(lewygoraX, lewygoraY + 0.2 * wys_pros));
    g.DrawLine(&pen, Point(0.2 * lewygoraX, lewygoraY + 0.6 * wys_pros), Point(lewygoraX, lewygoraY + 0.6 * wys_pros));
    g.DrawLine(&pen, Point(0.2 * lewygoraX, lewygoraY + wys_pros), Point(lewygoraX, lewygoraY + wys_pros));

    //PRAWA
    g.DrawLine(&pen, Point(lewygoraX + szer_pros, lewygoraY + 0.4 * wys_pros), Point(1.8 * lewygoraX + szer_pros, lewygoraY + 0.4 * wys_pros));
    g.DrawLine(&pen, Point(lewygoraX + szer_pros, lewygoraY + 0.8 * wys_pros), Point(1.8 * lewygoraX + szer_pros, lewygoraY + 0.8 * wys_pros));

}

static void innershaft(Graphics& g, int lewygoraX, int lewygoraY, int szer_wind, int wys_wind)
{
    Pen pen(Color(255, 0, 0), 1);

    //prostokat (winda)
    g.DrawRectangle(&pen, lewygoraX + 3, lewygoraY, szer_wind - 7, wys_wind);
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            Graphics graphics(hdc);

            wholeshaft(graphics, 50, 50, 80, 200);
            innershaft(graphics, 50, 50, 80, 40);
            DrawLevitatingPerson(graphics, 10, 10);






            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
