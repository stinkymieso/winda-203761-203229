// winda-203761-203229.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "winda-203761-203229.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <queue>

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

//global declarations

int globalspacing;
int globalright;
int globalbottom;

int current = 1;
const int allfloors = 5;
const int floorheight = 100;
int destination = 1;
bool direction = true; //true - going up, false - going down
HWND globalHwnd;
int pickupFloor = -1;
int dropoffFloor = -1;

HWND floorButtons[5][4];
//buttons
const int buttonWidth = 40;
const int buttonHeight = 25;
const int startX = 20;      // Starting x for buttons
const int startY = 100;     // Starting y, adjust to match your UI
const int floorSpacing = 100; // Vertical space between floors
const int buttonSpacing = 50; // Horizontal space between buttons
const WCHAR* buttonLabels[6] = { L"1", L"2", L"3", L"4", L"5" };

struct ButtonInfo {
    int id;
    int targetFloor;
    int fromFloor;
};

ButtonInfo buttons[20];
int structindex = 0;

//starting with the queues
std::queue<int> floorQueue;
bool peopleWaiting[5] = { false };

//drawing people
struct Person {
    int floor;
    int x;
    int y;
    bool moving;
};

Person people[50];
int peoplecount = 0;

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

    const float bottomPadding = 0.15f * wys_pros;  // space below 1st floor
    const float topPadding = 0.1f * wys_pros;
    
    RECT shaft;
    shaft.left = (client.right / 2 - szer_pros / 2);
    shaft.right = (client.right / 2 + szer_pros / 2);
    shaft.top = (client.bottom / 2 - wys_pros / 2);
    shaft.bottom = (client.bottom / 2 + wys_pros / 2);
    
    // Convert RECT coordinates and sizes to float
    float shaftLeft = static_cast<float>(shaft.left);
    float shaftTop = static_cast<float>(shaft.top);
    float shaftBottom = (client.bottom / 2.0f + wys_pros / 2.0f);
    float shaftHeight = static_cast<float>(shaftBottom - shaftTop);

    float space = static_cast<float>(shaftHeight - topPadding - bottomPadding); // space between uppermost and lowest floor

    // Draw shaft rectangle using RectF
    RectF shaftRect(shaftLeft, shaftTop, szer_pros, shaftHeight);
    g.DrawRectangle(&pen, shaftRect);

    // Set pen color for floor lines
    pen.SetColor(Color(175, 110, 175, 250)); // baby blue

    // Draw left horizontal lines (floors)
    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + 0.2f * space + topPadding),
        PointF(shaftLeft, shaftTop + 0.2f * space + topPadding));

    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + 0.6f * space + topPadding),
        PointF(shaftLeft, shaftTop + 0.6f * space + topPadding));

    g.DrawLine(&pen,
        PointF(0.2f * shaftLeft, shaftTop + space + topPadding),
        PointF(shaftLeft, shaftTop + space + topPadding));

    // Draw right horizontal lines (floors)
    g.DrawLine(&pen,
        PointF(shaftLeft + szer_pros, shaftTop + 0.4f * space + topPadding),
        PointF(1.8f * shaftLeft + szer_pros, shaftTop + 0.4f * space + topPadding));

    g.DrawLine(&pen,
        PointF(shaftLeft + szer_pros, shaftTop + 0.8f * space + topPadding),
        PointF(1.8f * shaftLeft + szer_pros, shaftTop + 0.8f * space + topPadding));


}

static void innershaft(Graphics& g, RECT client, int szer_wind, int wys_wind){
    
    Pen pen(Color(200,230, 155, 175), 1);

    const float bottomPadding = 0.15f * wys_wind * 5;  // space below 1st floor
    const float topPadding = 0.1f * wys_wind * 5;

    int liftx = client.right / 2 - 100; //zmienuic na szer_wind/2 (?)
    int liftbasey = (5 * wys_wind - bottomPadding); // 5*wyswind - bottom padding to linia dolna lewa                        
    int lifty = liftbasey - (current-1) * 0.2f * (5 * wys_wind - topPadding - bottomPadding); // 100 - wysokosc innershaft


    //prostokat (winda)
    g.DrawRectangle(&pen, liftx + 3, lifty, szer_wind - 7, wys_wind);
}

static void movement(int fromFloor, int toFloor) {
    if (fromFloor < 1 || fromFloor > 5 || toFloor < 1 || toFloor > 5) return;

    if (current != fromFloor) {
        pickupFloor = fromFloor;
        dropoffFloor = toFloor;
        destination = pickupFloor;
    }
    else {
        // Already at pickup, go to drop-off
        destination = toFloor;
        pickupFloor = -1; // Clear pickup
        dropoffFloor = toFloor;
    }

    // Set direction
    if (current < destination)
        direction = true;
    else if (current > destination)
        direction = false;

    // Start the movement
    SetTimer(globalHwnd, 1, 1000, NULL);

    // Debug
    wchar_t buf[100];
    swprintf_s(buf, 100, L"Movement queued/called: floor %d\n", toFloor);
    OutputDebugString(buf);
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

            wchar_t buf[100];
            wsprintf(buf, L"Button %d clicked\n", wmId);
            OutputDebugString(buf);


            //floor buttons
            if (wmId >= 100 && wmId <= 143)
            {
                for (int i = 0; i < structindex; i++)
                {
                    if (buttons[i].id == wmId)
                    {
                        int destFloor = buttons[i].targetFloor;
                        int fromFloor = buttons[i].fromFloor;
                        peopleWaiting[fromFloor-1] = true;

                        
                        //people[peoplecount].floor = fromFloor - 1;
                        //people[peoplecount].x = ((fromFloor - 1) % 2 == 0) ? (globalright / 2 - 250 - 100) : (globalright / 2 + 250 + 100);
                        //people[peoplecount].y = globalbottom / 2 - 250 + 50 + 10 + (4 - (fromFloor - 1)) * (int)(0.2f * globalspacing);
                        //people[peoplecount].moving = true;
                        //peoplecount++;

                        movement(fromFloor, destFloor); 
                        //peopleWaiting[fromFloor] = false;
                        break;
                    }
                }
            }
            else {


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
            int space = 500 - 50 - 75; // wys_pros - 0.1wys_pros - 0.15wys_pros CHCE TYLKO OMINAC FLOATING POINT BO NIE CHCE JUZ ZMIENIAC DrawLevitatingPerson() PLS
            globalspacing = space;
            globalright = (int)xright;
            globalbottom = (int)ybottom;

            wholeshaft(graphics,winrect, 200, 500);
            innershaft(graphics, winrect, 200, 100);
            for (int floor = 0; floor < 5; floor++) {
                if (peopleWaiting[floor]) {   

                    int y = ybottom/2 - 250 + 50 + 10 + (4-floor) * 0.2 * space; //50=0.1wyspros = topPadding, 250 = 0.5wyspros, ybottom/2 = srodek w pionie, 10 = offest, zeby lewitowali troche nizej nad podloga hehe 
                    int x;
                    
                    if (floor % 2 == 0) {
                       x = xright / 2 - 250 - 100;
                    }
                    else {
                        x = xright / 2 + 250 + 100;
                    }

                    DrawLevitatingPerson(graphics, x, y);

                    /*for (int i = 0; i < peoplecount; ++i) {
                        DrawLevitatingPerson(graphics, people[i].x, people[i].y);
                    }*/

                    wchar_t buf[100];
                    swprintf_s(buf, 100, L"Person added to floor: %d\n", floor+1);
                    OutputDebugString(buf);

                    //peopleWaiting[floor] = false;
                }
                    
            }
            






            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    
    case WM_CREATE:        
        globalHwnd = hWnd;
        SetTimer(hWnd, 1, 1000, NULL);

        for (int floor = 0; floor < 5; ++floor)
        {
            int btnindex = 0;

            for (int b = 0; b < 5; ++b)
            {
                if (b == floor) continue;
                int x;
                if (floor % 2 == 0) {
                    x = startX + btnindex * (buttonWidth + 10);
                }
                else {
                    x = startX + btnindex * (buttonWidth + 10) + 1200;
                }
                
                int y = startY + (4-floor) * floorSpacing;

                
                int btnID = 100 + floor * 10 + btnindex; // Unique ID for each button

                floorButtons[floor][b] = CreateWindow(
                    L"BUTTON",
                    buttonLabels[b],
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    x, y,
                    buttonWidth, buttonHeight,
                    hWnd,
                    (HMENU)(INT_PTR)btnID,
                    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                    NULL
                );

                buttons[structindex++] = {btnID, b+1, floor+1};                

                btnindex++;
            }
        }



        break;
    case WM_TIMER:
        if (wParam == 1) { // Timer ID
            if (direction && current < destination) {
                current++;
            }
            else if (!direction && current > destination) {
                current--;
            }

            /*for (int i = 0; i < peoplecount; ++i) {
                if (people[i].moving) {
                    int targetX = globalright / 2;  // Shaft center

                    // Move step-by-step
                    if (people[i].x > targetX) people[i].x -= 5;
                    else if (people[i].x < targetX) people[i].x += 5;
                    else people[i].moving = false;  // Reached shaft

                    wchar_t buf[200];
                    swprintf_s(buf, 200, L"Window size: right = %d, bottom = %d, spacing = %d\n", globalright, globalbottom, globalspacing);
                    OutputDebugString(buf);


                    wchar_t dbg[100];
                    swprintf_s(dbg, 100, L"Person %d pos: (%d, %d)\n", i, people[i].x, people[i].y);
                    OutputDebugString(dbg);
                }
                else {
                    // reached shaft
                    for (int j = i; j < peoplecount - 1; ++j)
                        people[j] = people[j + 1];
                    peoplecount--;
                    i--;  // So the next person isn't skipped
                }
            }*/


            if (current == destination) {
                if (pickupFloor != -1) {
                    // Arrived at pickup, go to drop-off
                    destination = dropoffFloor;
                    pickupFloor = -1;

                    direction = (destination > current);
                }
                else {
                    // Done
                    KillTimer(hWnd, 1);
                }
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
