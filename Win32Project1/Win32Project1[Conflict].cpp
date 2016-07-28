// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project1.h"
#include "Windowsx.h"


#define MAX_LOADSTRING 100
#define ID_TIMER1	123123
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT p1, p2, p2_ancien;
BOOL enTrainDessin;
HBITMAP memBmp;									// dugnf để lưu bảng vẽ
HDC bitmapDC1; // 

int mode; //mode de dessin==> 0 pour ligne, 1 pour libre
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID				DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawLine	(HDC hdc);
VOID FreeDraw	(HDC hdc);
VOID PenBrushDemo(HDC hdc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU1);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZEBOX| WS_MINIMIZEBOX| WS_SYSMENU,
      0, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

     ShowWindow(hWnd, nCmdShow);
	 HDC hdc = GetDC(hWnd);
	 bitmapDC1 = CreateCompatibleDC(hdc);

	 RECT rect;
	 GetWindowRect(hWnd, &rect);
	 HBITMAP bitmap1 = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	 SelectObject(bitmapDC1, bitmap1);
	 SetROP2(bitmapDC1, R2_COPYPEN);
	 HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	 FillRect(bitmapDC1, &rect, hbrBkGnd);

	 DeleteObject(hbrBkGnd);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (mode)
	{
	case 0:
		FreeDrawProc(hWnd, message, wParam, lParam);
		break;
	case 1:
		DrawLineProc(hWnd, message, wParam, lParam);
	}
	
	switch (message)
    {
	case WM_CREATE:
		p1.x = p1.y = 0;
		p2 = p1;
		mode = 1;
		enTrainDessin = FALSE;
	

		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_DRAW_PEN:
				mode = 2;
				RECT rect;
				GetWindowRect(hWnd, &rect);
				InvalidateRect(hWnd, &rect, true);
				UpdateWindow(hWnd);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				if (MessageBox(hWnd, _T("Are you sure to close?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
				{
					DestroyWindow(hWnd);
				}
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_RBUTTONDOWN:
		SetTimer(hWnd, ID_TIMER1, 1000, NULL);
		
		break;
	case WM_RBUTTONDBLCLK:
		KillTimer(hWnd, ID_TIMER1);
		//effacer l'ecran
		SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)GetDC(hWnd), (LPARAM) nullptr);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case ID_TIMER1:
			KillTimer(hWnd, ID_TIMER1);
			mode = (mode + 1) % 2;
			MessageBox(hWnd, _T("Mode de dessin change"), _T("Notifcation"), MB_OK);
			break;
		}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            
            // TODO: Add any drawing code that uses hdc here...
			HDC hdc = BeginPaint(hWnd, &ps);
		
			RECT rect;
			GetWindowRect(hWnd, &rect);
			
			BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, bitmapDC1, 0, 0, SRCCOPY);
			
				switch (mode)
				{
				case 0:
					FreeDraw(hdc);
					DrawLine(bitmapDC1);
					break;
				case 1:
					DrawLine(hdc);
					
					break;
				case 2:
					PenBrushDemo(bitmapDC1);
					break;
				}
			
			
			
		
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

VOID DrawLine(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN myPen = CreatePen(PS_SOLID | PS_ENDCAP_ROUND, 10, RGB(0, 255, 0));
		SelectObject(hdc, myPen);
		HPEN oldPen = (HPEN)SelectObject(bitmapDC1, myPen);
	

		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);

		SelectObject(hdc, oldPen);
		DeleteObject(myPen);
	}
}

VOID FreeDraw(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN myPen = CreatePen(PS_SOLID | PS_ENDCAP_ROUND, 10, RGB(0, 255, 0));
		SelectObject(hdc, myPen);
		HPEN oldPen = (HPEN)SelectObject(bitmapDC1, myPen);
		
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
		p1 = p2;

		SelectObject(hdc, oldPen);
		DeleteObject(myPen);
	}
}
VOID DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
			DrawLine(bitmapDC1);
			enTrainDessin = FALSE;
			
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, TRUE);
			UpdateWindow(hWnd);
			break;
	}
}

VOID FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:
		enTrainDessin = FALSE;
		break;


	}
}
VOID PenBrushDemo(HDC hdc)
{
	HPEN hPen = CreatePen(PS_DASHDOT, 3, RGB(255, 0, 255));
	SelectObject(bitmapDC1, hPen);
	MoveToEx(bitmapDC1, 100, 100, NULL);
	LineTo(bitmapDC1, 200, 200);
	Rectangle(bitmapDC1, 100, 200, 300, 300);
	RECT rect;
	rect.left = 100;
	rect.top = 200;
	rect.right = 300;
	rect.bottom = 300;
	HBRUSH hbrush = CreateSolidBrush(RGB(255, 255, 40));
	FillRect(bitmapDC1, &rect, hbrush);
	Ellipse(bitmapDC1, 100, 200, 300, 300);
	HRGN hRegion = CreateEllipticRgn(100, 200, 300, 300);
	hbrush = CreateSolidBrush(RGB(23, 100, 40));
	FillRgn(bitmapDC1, hRegion, hbrush);
	//Ellipse(hdc, 100, 200, 300, 300);
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

