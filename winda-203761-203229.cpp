// winda-203761-203229.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "winda-203761-203229.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <queue>
#include <algorithm>

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
bool isPaused = false;
UINT_PTR pauseTimerId = 2;

HWND floorButtons[5][4];
//buttons
const int buttonWidth = 40;
const int buttonHeight = 25;
const int startX = 20;      
const int startY = 100;    
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

enum PersonState {
    Waiting,      // waiting on the floor    
    InsideElevator// inside the elevator, moving with it       
};

struct Person {
    int floor;
    int targetfloor;
    int x;
    int y;
    bool moving;
    bool leaving;
    bool inElevator;
    PersonState state;
    ULONGLONG exitTime = 0;
};

Person people[50];
int peoplecount = 0;
ULONGLONG now;

std::queue<int> requestQueue;

bool anyoneStillMoving = false;

int insideCount = 0;

ULONGLONG lastRequestTime = 0;

static void DrawLevitatingPerson(Graphics& g, int x, int y) {
    Pen pen(Color(255, 0, 0, 0), 2); // black pen, 2px wide

    // Head
    g.DrawEllipse(&pen, x, y, 20, 20); // head circle

    int cx = x + 10; // center of the head
    int top = y + 20;

    // Body
    g.DrawLine(&pen, Point(cx, top), Point(cx, top + 25)); // torso

    // Arms 
    g.DrawLine(&pen, Point(cx - 10, top + 10), Point(cx + 10, top + 10));

    // Legs cross-legged: draw an "X" under the body
    g.DrawLine(&pen, Point(cx - 10, top + 25), Point(cx + 10, top + 35));
    g.DrawLine(&pen, Point(cx + 10, top + 25), Point(cx - 10, top + 35));

    // Levitation shadow
    SolidBrush brush(Color(100, 0, 0, 0)); // semi-transparent
    g.FillEllipse(&brush, x, y + 50, 20, 5); // shadow
}

void DrawPersonWithTarget(Graphics& g, int x, int y, int targetFloor) {
    DrawLevitatingPerson(g, x, y);

    // Drawing target floor number above the head
    WCHAR buf[10];
    wsprintf(buf, L"%d", targetFloor);

    Font font(L"Arial", 14);
    SolidBrush brush(Color(255, 0, 0, 0)); // Black text
    g.DrawString(buf, -1, &font, PointF(x + 2, y - 20), &brush);
}

static void wholeshaft(Graphics& g, RECT client, double szer_pros, double wys_pros){

    Pen pen(Color(135, 0, 0, 0), 2);

    const float bottomPadding = 0.15f * wys_pros;  // space below 1st floor
    const float topPadding = 0.1f * wys_pros;
    
    RECT shaft;
    shaft.left = (client.right / 2 - szer_pros / 2);
    shaft.right = (client.right / 2 + szer_pros / 2);
    shaft.top = (client.bottom / 2 - wys_pros / 2);
    shaft.bottom = (client.bottom / 2 + wys_pros / 2);
    
    float shaftLeft = static_cast<float>(shaft.left);
    float shaftTop = static_cast<float>(shaft.top);
    float shaftBottom = (client.bottom / 2.0f + wys_pros / 2.0f);
    float shaftHeight = static_cast<float>(shaftBottom - shaftTop);

    float space = static_cast<float>(shaftHeight - topPadding - bottomPadding); // space between uppermost and lowest floor

    RectF shaftRect(shaftLeft, shaftTop, szer_pros, shaftHeight);
    g.DrawRectangle(&pen, shaftRect);

    // Set pen color for floor lines
    pen.SetColor(Color(175, 110, 175, 250)); 

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

    int liftx = client.right / 2 - 100; 
    int liftbasey = (5 * wys_wind - bottomPadding); // 5*wyswind - bottom padding to linia dolna lewa                        
    int lifty = liftbasey - (current-1) * 0.2f * (5 * wys_wind - topPadding - bottomPadding); // 100 - wysokosc innershaft


    //prostokat (winda)
    g.DrawRectangle(&pen, liftx + 3, lifty, szer_wind - 7, wys_wind);
}

static void movement(int fromFloor, int toFloor) {
    if (fromFloor < 1 || fromFloor > 5 || toFloor < 1 || toFloor > 5) return;

    pickupFloor = fromFloor;
    dropoffFloor = toFloor;
    destination = pickupFloor;  // First go to pickup

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

std::queue<int> betterQueue(std::queue<int> queue, int id) {
    std::queue<int> newQueue;
    if (queue.empty()) {
        queue.push(id);
        return queue;
    }
    else
    {
        int id1, id2, id3;
        if (id / 10 < id % 10) { //jazda w góre
            while (!queue.empty()) {
                if ((queue.front() / 10 < queue.front() % 10) && queue.front() / 10 != id / 10 && queue.front() % 10 != id % 10) {
                    int floors[4];
                    floors[0] = queue.front() / 10;
                    floors[1] = queue.front() % 10;
                    floors[2] = (int)(id / 10);
                    floors[3] = id % 10;
                    std::sort(&floors[0], &floors[3]);
                    id1 = floors[0] * 10 + floors[1];
                    id2 = floors[1] * 10 + floors[2];
                    id3 = floors[2] * 10 + floors[3];
                    newQueue.push(id1);
                    newQueue.push(id2);
                    newQueue.push(id3);
                    queue.pop();
                    while (!queue.empty()) {
                        newQueue.push(queue.front());
                        queue.pop();
                    }
                    return newQueue;
                }
                newQueue.push(queue.front());
                queue.pop();
            }
            return newQueue;
        }
        else { // jazda w dó³
            while (!queue.empty()) {
                if ((queue.front() / 10 > queue.front() % 10) && queue.front() / 10 != id / 10 && queue.front() % 10 != id % 10) {
                    int floors[4];
                    floors[0] = queue.front() / 10;
                    floors[1] = queue.front() % 10;
                    floors[2] = id / 10;
                    floors[3] = id % 10;
                    std::sort(&floors[0], &floors[3]);
                    id1 = floors[3] * 10 + floors[2];
                    id2 = floors[2] * 10 + floors[1];
                    id3 = floors[1] * 10 + floors[0];
                    newQueue.push(id1);
                    newQueue.push(id2);
                    newQueue.push(id3);
                    queue.pop();
                    while (!queue.empty()) {
                        newQueue.push(queue.front());
                        queue.pop();
                    }
                    return newQueue;
                }
                newQueue.push(queue.front());
                queue.pop();
            }
            return newQueue;
        }
    }
}

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
                    peopleWaiting[fromFloor - 1] = true;

                    insideCount = 0;
                    for (int j = 0; j < peoplecount; ++j) {
                        if (people[j].state == InsideElevator) {
                            insideCount++;
                        }
                    }

                    // Limit max 8 people inside elevator shaft
                    if (insideCount >= 8) {
                        break;  // Cancel adding this person
                    }
                    else {
                        if (peoplecount < 100) {
                            people[peoplecount].floor = fromFloor - 1;
                                                        
                            people[peoplecount].x = (globalright / 2 + 75) - (insideCount * 10);
                            people[peoplecount].y = globalbottom / 2 - 250 + 50 + 10 + (4 - (fromFloor - 1)) * 0.2 * globalspacing;

                            people[peoplecount].targetfloor = destFloor;
                            people[peoplecount].moving = true;
                            people[peoplecount].state = Waiting;

                            peoplecount++;
                        }

                        requestQueue = betterQueue(requestQueue, fromFloor * 10 + destFloor);
                        wchar_t tyf[100];
                        wsprintf(tyf, L"kolejka %d \n", requestQueue.front());
                        OutputDebugString(tyf);

                        if (pickupFloor == -1 && dropoffFloor == -1) {
                            int code = requestQueue.front();
                            requestQueue.pop();
                            int from = code / 10;
                            int to = code % 10;
                            movement(from, to);
                        }

                        break;
                    }
                }
            }
        }
        else {
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT: {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            }
            case IDM_EXIT: {
                DestroyWindow(hWnd);
                break;
            }
            default: {
                return DefWindowProc(hWnd, message, wParam, lParam);
                break;
                }
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

        if (GetClientRect(hWnd, &winrect)) { //aby dostac wspolrzedne okna dialogowego do pointera lpRect

            xleft = winrect.left; 
            xright = winrect.right;
            ytop = winrect.top; 
            ybottom = winrect.bottom;
        }
        else {
            DWORD err = GetLastError();
        }

        int space = 500 - 50 - 75; // wys_pros - 0.1wys_pros - 0.15wys_pros CHCE TYLKO OMINAC FLOATING POINT BO NIE CHCE JUZ ZMIENIAC DrawLevitatingPerson() PLS
        globalspacing = space;
        globalright = (int)xright;
        globalbottom = (int)ybottom;

        wholeshaft(graphics, winrect, 200, 500);
        innershaft(graphics, winrect, 200, 100);
        for (int floor = 0; floor < 5; floor++) {
            if (peopleWaiting[floor]) {


                for (int i = 0; i < peoplecount; ++i) {
                    if (people[i].state == InsideElevator) {
                        int drawX = people[i].x;
                        int drawY = people[i].y;
                        DrawPersonWithTarget(graphics, drawX, drawY - (i - 1) * 20, people[i].targetfloor);
                    }
                }

                wchar_t buf[100];
                swprintf_s(buf, 100, L"Person added to floor: %d\n", floor + 1);
                OutputDebugString(buf);
            }
        }

        insideCount = 0;
        for (int i = 0; i < peoplecount; ++i) {
            if (people[i].state == InsideElevator)
                insideCount++;
        }

        wchar_t buffer[100];
        swprintf(buffer, 100, L"Liczba osob w windzie: %d / 8", insideCount);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0)); // Black text
        TextOut(hdc, globalright / 2 - 100, 10, buffer, wcslen(buffer)); 

        wchar_t buffer2[100];
        swprintf(buffer2, 100, L"Masa osob w windzie: %d", insideCount*70);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0)); // Black text
        TextOut(hdc, globalright / 2 - 100, 25, buffer2, wcslen(buffer2));


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

                int y = startY + (4 - floor) * floorSpacing;


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

                buttons[structindex++] = { btnID, b + 1, floor + 1 };

                btnindex++;
            }
        }
        break;
    case WM_TIMER:
    {       
        if (wParam == 1) { // Timer ID
            now = GetTickCount64();

            wchar_t dbg[256];
            swprintf(dbg, 256, L"[TIMER] now=%llu, current=%d, direction=%d, destination=%d, pickup=%d, dropoff=%d, queueSize=%zu\n",
                now, current, direction ? 1 : 0, destination, pickupFloor, dropoffFloor, requestQueue.size());
            OutputDebugString(dbg);

            if (!isPaused && destination != -1) {
                if (direction && current < destination) current++;
                else if (!direction && current > destination) current--;

                InvalidateRect(hWnd, NULL, TRUE);

                if (current == destination) {
                    if (current == pickupFloor) {
                        isPaused = true;

                        for (int i = 0; i < peoplecount; ++i) {
                            if (people[i].state == Waiting && people[i].floor == pickupFloor - 1) {
                                people[i].state = InsideElevator;
                                people[i].inElevator = true;
                                people[i].floor = current - 1;
                                people[i].x = (globalright / 2) + 75 - (insideCount * 10); // position based on count
                                people[i].y = globalbottom / 2 - 250 + 50 + 10 + (4 - people[i].floor) * (int)(0.2 * globalspacing);
                                people[i].exitTime = 0;
                                break;  // only one person per pickup
                            }
                        }

                        KillTimer(hWnd, 1);  // Stopping movement timer
                        SetTimer(hWnd, pauseTimerId, 1500, NULL);  // Pausing for 1.5 seconds
                    }
                    if (current == dropoffFloor) {
                        // Arrived at dropoff floor, stop movement                        
                        KillTimer(hWnd, 1); 
                        
                        OutputDebugString(L"Before removal:\n");
                        for (int i = 0; i < peoplecount; ++i) {
                            wchar_t msg[100];
                            swprintf(msg, 100, L"Person %d state: %d, floor: %d, target: %d\n", i, people[i].state, people[i].floor, people[i].targetfloor);
                            OutputDebugString(msg);
                        }

                        wchar_t msg[100];
                        swprintf(msg, 100, L"current=%d\n", current);
                        OutputDebugString(msg);

                        for (int i = 0; i < peoplecount; ++i) {
                            if (people[i].state == InsideElevator && people[i].targetfloor == current  ) {
                                // Remove person by shifting array left
                                for (int j = i; j < peoplecount - 1; ++j) {
                                    people[j] = people[j + 1];
                                }
                                peoplecount--;
                                i--; 

                                OutputDebugString(L"Person exited elevator at dropoff floor.\n");
                            }
                        }

                        OutputDebugString(L"After removal:\n");
                        for (int i = 0; i < peoplecount; ++i) {
                            wchar_t msg[100];
                            swprintf(msg, 100, L"Person %d state: %d, floor: %d, target: %d\n", i, people[i].state, people[i].floor, people[i].targetfloor);
                            OutputDebugString(msg);
                        }

                        isPaused = true;                     

                        pickupFloor = -1;
                        dropoffFloor = -1;

                        if (!requestQueue.empty()) {
                             int code = requestQueue.front();
                             requestQueue.pop();
                             int from = code / 10;
                             int to = code % 10;
                             movement(from, to);
                        }
                        else {
                            OutputDebugString(L"No more requests.\n");
                        }
                        
                        SetTimer(hWnd, pauseTimerId, 1500, NULL);
                        InvalidateRect(hWnd, NULL, TRUE);                        
                    }
                }                
            }

            for (int i = 0; i < peoplecount; ++i) {
                if (people[i].state == InsideElevator) {
                    people[i].floor = current - 1;
                    people[i].y = globalbottom / 2 - 250 + 50 + 10 + (4 - people[i].floor) * (int)(0.2 * globalspacing);

                }
            }

            if (requestQueue.empty() && !isPaused && destination == -1) {
                DWORD now = GetTickCount();
                if (now - lastRequestTime > 10000) {  // few seconds passed
                    if (current != 1) {  // Not already at floor 1
                        OutputDebugString(L"Inactivity timeout. Returning to floor 1.\n");
                        movement(current, 1);  // Return to floor 1
                    }
                }
            }
        }

        else if (wParam == pauseTimerId) {  
            isPaused = false;
            KillTimer(hWnd, pauseTimerId);

            // Set destination to dropoff and restart movement
            destination = dropoffFloor;

            if (current < destination)
                direction = true;
            else if (current > destination)
                direction = false;

            SetTimer(hWnd, 1, 1000, NULL);
            InvalidateRect(hWnd, NULL, TRUE);
        }         
    }
        break;
    case WM_DESTROY: {
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        }
        break;
    
    default:
        {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
    }
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
//AAAAAAAAAAAAAAAAAAAA