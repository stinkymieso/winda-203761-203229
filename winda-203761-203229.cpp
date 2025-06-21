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

int current = 1;
const int allfloors = 5;
const int floorheight = 100;
int destination = 5;
bool direction = true; //true - going up, false - going down


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


static void wholeshaft(Graphics& g, RECT client, double szer_pros, double wys_pros){

    Pen pen(Color(135, 0, 0, 0), 2);//proba zmiany alpha

    const float bottomPadding = 0.25f * wys_pros;  // space below 1st floor
    const float topPadding = 0.1f * wys_pros;
    
    RECT shaft;
    shaft.left = (client.right / 2 - szer_pros / 2);
    shaft.right = (client.right / 2 + szer_pros / 2);
    shaft.top = (client.bottom / 2 - wys_pros / 2);
    shaft.bottom = (client.bottom / 2 + wys_pros / 2);
    
    // Convert RECT coordinates and sizes to float
    float shaftLeft = static_cast<float>(shaft.left);
    float shaftTop = static_cast<float>(shaft.top) - topPadding;
    float shaftBottom = (client.bottom / 2.0f + wys_pros / 2.0f) + bottomPadding;
    float shaftHeight = static_cast<float>(shaftBottom - shaftTop);

    // Draw shaft rectangle using RectF
    RectF shaftRect(shaftLeft, shaftTop, szer_pros, shaftHeight);
    g.DrawRectangle(&pen, shaftRect);

    // Set pen color for floor lines
    pen.SetColor(Color(175, 110, 175, 250)); // baby blue

    // Draw left horizontal lines (floors)
    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + 0.2f * shaftHeight),
        PointF(shaftLeft, shaftTop + 0.2f * shaftHeight));

    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + 0.6f * shaftHeight),
        PointF(shaftLeft, shaftTop + 0.6f * shaftHeight));

    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + shaftHeight),
        PointF(shaftLeft, shaftTop + shaftHeight));

    // Draw right horizontal lines (floors)
    g.DrawLine(&pen,
        PointF(shaftLeft + szer_pros, shaftTop + 0.4f * shaftHeight),
        PointF(1.8f * shaftLeft + szer_pros, shaftTop + 0.4f * shaftHeight));

    g.DrawLine(&pen,
        PointF(shaftLeft + szer_pros, shaftTop + 0.8f * shaftHeight),
        PointF(1.8f * shaftLeft + szer_pros, shaftTop + 0.8f * shaftHeight));


}

static void innershaft(Graphics& g, RECT client, int szer_wind, int wys_wind){
    
    Pen pen(Color(200,230, 155, 175), 1);

    int liftx = client.right / 2 - 100;
    int liftbasey = (client.bottom / 2) + 250 - 125; //trzeba uwzglednic to, ze 1 pietro jest na wysokosci wys_pros a caly shaft ma 1,25 wys_pros, wiec 0,25wys_pros jest puste pod winda
    //                                           ^podloga windy - 0,25wys_pros
    int lifty = liftbasey - (current-1) * 100; // 100 - wysokosc innershaft


    //prostokat (winda)
    g.DrawRectangle(&pen, liftx + 3, lifty + 1, szer_wind - 7, wys_wind);
}

static void movement(Graphics& g, RECT liftcoords) {
    if (destination < 1 || destination > 5) {
        DWORD err = GetLastError(); //poprawic zeby wyswietlalo blad, ze za duzy floor, to tylko testowe bo potem beda dzialay buttonsy tylko wiec nie bedzie problemu z inputem zlego pietra
    }
    else {
        int liftx = liftcoords.right / 2 - 100;
        int liftbasey = (liftcoords.bottom/2) + 250 - 125; //trzeba uwzglednic to, ze 1 pietro jest na wysokosci wys_pros a caly shaft ma 1,25 wys_pros, wiec 0,25wys_pros jest puste pod winda
        // ^podloga windy - 0,25wys_pros
        int lifty = liftbasey - (current-1) * 100; // 100 - wysokosc innershaft
    }
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//  WM_CREATE   - handling the timer - my addition
//  WM_TIMER    - for animation purposes - my addition
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

            LONG xleft = 0, xright = 0, ytop = 0, ybottom = 0;
            RECT winrect;

            if (GetClientRect(hWnd, &winrect)) { //aby dostac coordinates okna dialogowego do pointera lpRect

                xleft = winrect.left; //chyba zawsze 0, mozna pominac
                xright = winrect.right;
                ytop = winrect.top; //to tez zawsze 0, mozna pominac
                ybottom = winrect.bottom;
            }
            else {
                DWORD err = GetLastError();
            }


            wholeshaft(graphics,winrect, 200, 500);
            innershaft(graphics, winrect, 200, 100);
            DrawLevitatingPerson(graphics, 10, 10);






            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    
    case WM_CREATE:
        SetTimer(hWnd, 1, 1000, NULL);
        return 0;
    case WM_TIMER:
        if (wParam == 1) { // Timer ID
            if (direction && current < destination) {
                current++;
                if (current == destination) direction = false;
            }
            else if (!direction && current >= destination) {
                current--;
            }

            InvalidateRect(hWnd, NULL, TRUE); // Trigger repaint
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
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
