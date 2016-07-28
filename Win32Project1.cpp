// Win32Project1.cpp : Defines the entry point for the application.
/*
Trong chương trình này, ta sẽ cố gắng lưu lại bản vẽ sau khi bản vẽ.
Việc vẽ các đường thẳng tạm sẽ không được vẽ trực tiếp lên bản vẽ này.
Ý tưởng: sử dụng một bitmap để lưu bản vẽ hiện hành. Việc vẽ các đường thẳng tạm sẽ được vẽ trực tiếp lên màn hình.
khi kết thúc việc vẽ thì ta mới vẽ đường thẳng lên bitmap
Đối với kỹ thuật này thì khi vẽ đường thẳng ta không cần vẽ đè đường
*/

#include "stdafx.h"
#include "Win32Project1.h"
#include "Windowsx.h"
#include <iostream>
#include <Windows.h>
#include "Commdlg.h"
#include <Strsafe.h>
#include <CommCtrl.h>
#include "pbitmapinfo.h"

using namespace std;


#define MAX_LOADSTRING 100
#define ID_TIMER1	123123
#define BUFSIZE 65535 
#define SHIFTED 0x8000 
//*********
#define IDD_GRID      120 
#define IDD_RECT      121 
#define IDD_PAINTRECT 122 
#define IDD_OK        123 
#define IDD_CANCEL    124 
#define ID_EDIT 1


// Global Variables:
char AppCaption[40];
HINSTANCE hInst;
const int NUMBUTTONS = 8;
// current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT p1, p2, p2_ancien;
int width, R, G, B, style;
HBITMAP hBitmap;
BOOL enTrainDessin;
LOGFONT lf;
HFONT hfont, hfontPrev;
DWORD rgbCurrent, rgbCurrent1, rgbCurrent2;
HBRUSH      NewBrush;
HIMAGELIST g_hImageList = NULL;


int mode; //mode de dessin==> 0 pour ligne, 1 pour libre
		  // Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Size_Pen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Style_Pen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Brush_Style(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawLine(HDC hdc);
VOID FreeDraw(HDC hdc);
VOID DrawCurve(HDC hdc);
VOID DrawRectangle(HDC hdc);
VOID DrawSquare(HDC hdc);
VOID PenBrushDemo(HDC hdc);
VOID DrawEllipse(HDC hdc);
VOID DrawRound(HDC hdc);
VOID PEN(HDC hdc);
////PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
//VOID CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,	HBITMAP hBMP, HDC hDC);
void Brush_Color(HWND hWnd);


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
	if (!InitInstance(hInstance, nCmdShow))
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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU,
		0, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&InitCtrlEx);

	TBBUTTON tbrButtons[8];

	tbrButtons[0].iBitmap = 0;
	tbrButtons[0].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[0].fsState = TBSTATE_ENABLED;
	tbrButtons[0].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[0].dwData = 0L;
	tbrButtons[0].iBitmap = 0;
	tbrButtons[0].iString = 0;

	tbrButtons[1].iBitmap = 1;
	tbrButtons[1].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[1].fsState = TBSTATE_ENABLED;
	tbrButtons[1].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[1].dwData = 0L;
	tbrButtons[1].iString = 0;

	tbrButtons[2].iBitmap = 7;
	tbrButtons[2].idCommand = 0;
	tbrButtons[2].fsState = TBSTATE_ENABLED;
	tbrButtons[2].fsStyle = TBSTYLE_SEP;
	tbrButtons[2].dwData = 0L;
	tbrButtons[2].iString = 0;

	tbrButtons[3].iBitmap = 2;
	tbrButtons[3].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[3].fsState = TBSTATE_ENABLED;
	tbrButtons[3].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[3].dwData = 0L;
	tbrButtons[3].iString = 0;

	tbrButtons[4].iBitmap = 3;
	tbrButtons[4].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[4].fsState = TBSTATE_ENABLED;
	tbrButtons[4].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[4].dwData = 0L;
	tbrButtons[4].iString = 0;

	tbrButtons[5].iBitmap = 4;
	tbrButtons[5].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[5].fsState = TBSTATE_ENABLED;
	tbrButtons[5].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[5].dwData = 0L;
	tbrButtons[5].iString = 0;

	tbrButtons[6].iBitmap = 5;
	tbrButtons[6].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[6].fsState = TBSTATE_ENABLED;
	tbrButtons[6].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[6].dwData = 0L;
	tbrButtons[6].iString = 0;

	tbrButtons[7].iBitmap = 6;
	tbrButtons[7].idCommand = ID_LINE_FREESTYLE;
	tbrButtons[7].fsState = TBSTATE_ENABLED;
	tbrButtons[7].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[7].dwData = 0L;
	tbrButtons[7].iString = 0;

	HWND hWndToolbar;
	hWndToolbar = CreateToolbarEx(hWnd,
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		IDB_BITMAP2,
		NUMBUTTONS,
		hInst,
		IDB_BITMAP2,
		tbrButtons,
		NUMBUTTONS,
		16, 16, 16, 16,
		sizeof(TBBUTTON));


	ShowWindow(hWnd, nCmdShow);
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
	HDC  hdc = GetDC(hWnd);
	switch (mode)
	{
	case 0:
		FreeDrawProc(hWnd, message, wParam, lParam);
		break;
	case 1: case 6:
		DrawLineProc(hWnd, message, wParam, lParam);
		break;
	case 2:case 3: case 4: case 5:
		DrawRectangleProc(hWnd, message, wParam, lParam);
		break;
	}
	////
	TEXTMETRIC tm;             // structure for text metrics 
	static DWORD dwCharX;      // average width of characters 
	static DWORD dwCharY;      // height of characters 
	static DWORD dwClientX;    // width of client area 
	static DWORD dwClientY;    // height of client area 
	static DWORD dwLineLen;    // line length 
	static DWORD dwLines;      // text lines in client area 
	static int nCaretPosX = 0; // horizontal position of caret 
	static int nCaretPosY = 0; // vertical position of caret 
	static int nCharWidth = 0; // width of a character 
	static int cch = 0;        // characters in buffer 
	static int nCurChar = 0;   // index of current character 
	static PTCHAR pchInputBuf; // input buffer 
	int i, j;                  // loop counters 
	int cCR = 0;               // count of carriage returns 
	int nCRIndex = 0;          // index of last carriage return 
	int nVirtKey;              // virtual-key code 
	TCHAR szBuf[128];          // temporary buffer 
	TCHAR ch;                  // current character 
	PAINTSTRUCT ps;            // required by BeginPaint 
	RECT rc;                   // output rectangle for DrawText 
	SIZE sz;                   // string dimensions 
	COLORREF crPrevText;       // previous text color 
	COLORREF crPrevBk;         // previous background color
	size_t * pcch = new size_t;
	HRESULT hResult;
	static HWND hwndEdit;
	////

	switch (message)
	{
	case WM_CREATE:
	{
	/*	hwndEdit = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE
			| ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, (HMENU)ID_EDIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);*/
		
		p1.x = p1.y = 0;
		p2 = p1;
		mode = 1;
		enTrainDessin = FALSE;

		////11
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hWnd, hdc);

		// Save the average character width and height. 

		dwCharX = tm.tmAveCharWidth;
		dwCharY = tm.tmHeight;

		// Allocate a buffer to store keyboard input. 

		pchInputBuf = (LPTSTR)GlobalAlloc(GPTR,
			BUFSIZE * sizeof(TCHAR));

		////11

		//Tạo bitmap
		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		//Tạo ra một bitmap tương thích với DC màn hình
		hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom -rect.top);

		//Tô background cho bitmap
		//Tạo memory dc để tương tác với Bitmap 
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(memDC, &rect, hBrush);
		DeleteObject(hBrush);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hdc);
		break;
	}
	
	////2
	case WM_SIZE:
	{
		//MoveWindow(hWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		// Save the new width and height of the client area. 

		dwClientX = LOWORD(lParam);
		dwClientY = HIWORD(lParam);

		// Calculate the maximum width of a line and the 
		// maximum number of lines in the client area. 

		dwLineLen = dwClientX - dwCharX;
		dwLines = dwClientY / dwCharY;
		break;
	}
	////2

	////3
	case WM_SETFOCUS:
	{
	//	SetFocus(hWnd);
		// Create, position, and display the caret when the 
		// window receives the keyboard focus. 

		CreateCaret(hWnd, (HBITMAP)1, 0, dwCharY);
		SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
		ShowCaret(hWnd);
		break;
	}
	////3

	////4
	case WM_KILLFOCUS:
	{

		// Hide and destroy the caret when the window loses the 
		// keyboard focus. 

		HideCaret(hWnd);
		DestroyCaret();
		break;
	}

	case WM_CHAR:
	{
		// check if current location is close enough to the
		// end of the buffer that a buffer overflow may
		// occur. If so, add null and display contents. 
		if (cch > BUFSIZE - 5)
		{
			pchInputBuf[cch] = 0x00;
			SendMessage(hWnd, WM_PAINT, 0, 0);
		}
		switch (wParam)
		{
		case 0x08:  // backspace 
		case 0x0A:  // linefeed 
		case 0x1B:  // escape 
			MessageBeep((UINT)-1);
			return 0;

		case 0x09:  // tab 

					// Convert tabs to four consecutive spaces. 

			for (i = 0; i < 4; i++)
				SendMessage(hWnd, WM_CHAR, 0x20, 0);
			return 0;

		case 0x0D:  // carriage return 

					// Record the carriage return and position the 
					// caret at the beginning of the new line.

			pchInputBuf[cch++] = 0x0D;
			nCaretPosX = 0;
			nCaretPosY += 1;
			break;

		default:    // displayable character 
			//--

			//--
			ch = (TCHAR)wParam;
			HideCaret(hWnd);

			// Retrieve the character's width and output 
			// the character. 

			hdc = GetDC(hWnd);
			GetCharWidth32(hdc, (UINT)wParam, (UINT)wParam,
				&nCharWidth);
			SelectObject(hdc, hfont);
			SetTextColor(hdc, rgbCurrent1);
			TextOut(hdc, nCaretPosX, nCaretPosY * dwCharY,
				&ch, 1);
			ReleaseDC(hWnd, hdc);

			// Store the character in the buffer.

			pchInputBuf[cch++] = ch;

			// Calculate the new horizontal position of the 
			// caret. If the position exceeds the maximum, 
			// insert a carria  ge return and move the caret 
			// to the beginning of the next line. 

			nCaretPosX += nCharWidth;
			if ((DWORD)nCaretPosX > dwLineLen)
			{
				nCaretPosX = 0;
				pchInputBuf[cch++] = 0x0D;
				++nCaretPosY;
			}
			nCurChar = cch;
			ShowCaret(hWnd);
			break;
		}
		SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
		break;
	}
	////4
	////5
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:   // LEFT ARROW 

						// The caret can move only to the beginning of 
						// the current line. 

			if (nCaretPosX > 0)
			{
				HideCaret(hWnd);

				// Retrieve the character to the left of 
				// the caret, calculate the character's 
				// width, then subtract the width from the 
				// current horizontal position of the caret 
				// to obtain the new position. 

				ch = pchInputBuf[--nCurChar];
				hdc = GetDC(hWnd);
				GetCharWidth32(hdc, ch, ch, &nCharWidth);
				ReleaseDC(hWnd, hdc);
				nCaretPosX = max(nCaretPosX - nCharWidth,
					0);
				ShowCaret(hWnd);
			}
			break;
		case VK_RIGHT:  // RIGHT ARROW 

						// Caret moves to the right or, when a carriage 
						// return is encountered, to the beginning of 
						// the next line. 

			if (nCurChar < cch)
			{
				HideCaret(hWnd);

				// Retrieve the character to the right of 
				// the caret. If it's a carriage return, 
				// position the caret at the beginning of 
				// the next line. 

				ch = pchInputBuf[nCurChar];
				if (ch == 0x0D)
				{
					nCaretPosX = 0;
					nCaretPosY++;
				}

				// If the character isn't a carriage 
				// return, check to see whether the SHIFT 
				// key is down. If it is, invert the text 
				// colors and output the character. 

				else
				{
					hdc = GetDC(hWnd);
					nVirtKey = GetKeyState(VK_SHIFT);
					if (nVirtKey & SHIFTED)
					{
						crPrevText = SetTextColor(hdc,
							RGB(255, 255, 255));
						crPrevBk = SetBkColor(hdc,
							RGB(0, 0, 0));
						TextOut(hdc, nCaretPosX,
							nCaretPosY * dwCharY,
							&ch, 1);
						SetTextColor(hdc, crPrevText);
						SetBkColor(hdc, crPrevBk);
					}

					// Get the width of the character and 
					// calculate the new horizontal 
					// position of the caret. 

					GetCharWidth32(hdc, ch, ch, &nCharWidth);
					ReleaseDC(hWnd, hdc);
					nCaretPosX = nCaretPosX + nCharWidth;
				}
				nCurChar++;
				ShowCaret(hWnd);
				break;
			}
			break;

		case VK_UP:     // UP ARROW 
		case VK_DOWN:   // DOWN ARROW 
			MessageBeep((UINT)-1);
			return 0;

		case VK_HOME:   // HOME 

						// Set the caret's position to the upper left 
						// corner of the client area. 

			nCaretPosX = nCaretPosY = 0;
			nCurChar = 0;
			break;

		case VK_END:    // END  

						// Move the caret to the end of the text. 

			for (i = 0; i < cch; i++)
			{
				// Count the carriage returns and save the 
				// index of the last one. 

				if (pchInputBuf[i] == 0x0D)
				{
					cCR++;
					nCRIndex = i + 1;
				}
			}
			nCaretPosY = cCR;

			// Copy all text between the last carriage 
			// return and the end of the keyboard input 
			// buffer to a temporary buffer. 

			for (i = nCRIndex, j = 0; i < cch; i++, j++)
				szBuf[j] = pchInputBuf[i];
			szBuf[j] = TEXT('\0');

			// Retrieve the text extent and use it 
			// to set the horizontal position of the 
			// caret. 

			hdc = GetDC(hWnd);
			hResult = StringCchLength(szBuf, 128, pcch);
			if (FAILED(hResult))
			{
				// TODO: write error handler
			}
			GetTextExtentPoint32(hdc, szBuf, *pcch,
				&sz);
			nCaretPosX = sz.cx;
			ReleaseDC(hWnd, hdc);
			nCurChar = cch;
			break;

		default:
			break;
		}
		SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
		break;

	}

	

		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_FILE_OPEN:

				OPENFILENAME ofn;           // common dialog box structure
				wchar_t szFile[MAX_PATH];      // buffer for file name MAX_PATH = 260
				HANDLE hf;                  // file handle

											// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;

				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"All\0*.*\0Text\0*.bmp\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				// Display the Open dialog box.
				//system("cd c:/windows/system32/&&cmd.exe"); will execute here.
				if (GetOpenFileName(&ofn) == TRUE)
				{

					hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					HDC memDC = CreateCompatibleDC(hdc);
					SelectObject(memDC, hBitmap);
					RECT rect;
					GetClientRect(hWnd, &rect);
					BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

					DeleteObject(memDC);

				}

				break;
			case 32807:
				OPENFILENAME ofn1;           // common dialog box structure
				wchar_t szFile1[MAX_PATH];      // buffer for file name MAX_PATH = 260
												// Initialize OPENFILENAME
				ZeroMemory(&ofn1, sizeof(ofn1));
				ofn1.lStructSize = sizeof(ofn1);
				ofn1.hwndOwner = hWnd;
				ofn1.lpstrFile = szFile1;

				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn1.lpstrFile[0] = '\0';
				ofn1.nMaxFile = sizeof(szFile1);
				//ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
				ofn1.lpstrFilter = L"All\0*.*\0Bitmap\0*.bmp\0";
				ofn1.nFilterIndex = 1;
				ofn1.lpstrFileTitle = NULL;
				ofn1.nMaxFileTitle = 0;
				ofn1.lpstrInitialDir = NULL;
				ofn1.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				ofn1.lpstrDefExt = L".bmp";
				// Display the Open dialog box.
				//system("cd c:/windows/system32/&&cmd.exe"); will execute here.
				if (GetSaveFileName(&ofn1) == TRUE)
				{
					PBITMAPINFO PBI1 = CreateBitmapInfoStruct(hWnd, hBitmap);
					CreateBMPFile(hWnd, ofn1.lpstrFile, PBI1, hBitmap, hdc);
				}
				break;

			case ID_FILE_SAVE:
				if (MessageBox(hWnd, _T("Are you sure to save?"), _T("Confirm save"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
				{
					PBITMAPINFO PBI = CreateBitmapInfoStruct(hWnd, hBitmap);
					CreateBMPFile(hWnd, _T("E:\\bitmap.bmp"), PBI, hBitmap, hdc);
					MessageBox(hWnd, _T("Save sucess"), _T("Confirm close"), MB_OK);
				}
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_FILE_EXIT:
				if (MessageBox(hWnd, _T("Are you sure to close?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
				{
					if (wmId != 32775)
						DestroyWindow(hWnd);
				}
				break;
			case 32784:
				mode = 1;
				break;
			case 32786:
				mode = 0;
				break;
			case 32780:
				mode = 2;
				break;
			case 32781:
				mode = 3;
				break;
			case 32783:
				mode = 4;
				break;
			case 32782:
				mode = 5;
				break;
			case 32785:
				mode = 6;
				break;
			case 32792:
				R = G = B = 0;
				break;
			case 32794:
				R = G = 0;   
				B = 255;
				break;
			case 32795:
				R = B = 0;
				G = 255;
				break;
			case 32793:
				G = B = 0;
				R = 255;
				break;
			case 32788:
				width = 1;;
				break;
			case 32789:
				width = 3;
				break;
			case 32790:
				width = 5;
				break;
			case 32791:
				width = 10;
				break;
			case 32798:
				CHOOSECOLOR cc;         
				static COLORREF acrCustClr[16]; 
				HBRUSH hbrush;                  
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hWnd;
				cc.lpCustColors = (LPDWORD)acrCustClr;
				cc.rgbResult = rgbCurrent;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				if (ChooseColor(&cc) == TRUE)
				{
					hbrush = CreateSolidBrush(cc.rgbResult);
					rgbCurrent = cc.rgbResult;
				}

				break;
			case 32797:
				CHOOSEFONT cf;            // common dialog box structure
			//	static LOGFONT lf;        // logical font structure
			//	static DWORD rgbCurrent;  // current text color
			//	HFONT hfont, hfontPrev;
				DWORD rgbPrev;

				// Initialize CHOOSEFONT
				ZeroMemory(&cf, sizeof(cf));
				cf.lStructSize = sizeof(cf);
				cf.hwndOwner = hWnd;
				cf.lpLogFont = &lf;
				cf.rgbColors = rgbCurrent1;
				cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
				if (ChooseFont(&cf) == TRUE)
				{
					hfont = CreateFontIndirect(&lf);
					hfontPrev = (HFONT) SelectObject(hdc, hfont);
					rgbCurrent1 = cf.rgbColors;
					rgbPrev = SetTextColor(hdc, rgbCurrent1);		
				}
				break;


			case ID_PEN_WIDTH32800:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Size_Pen);
				break;
			case ID_PEN_STYLE:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Style_Pen);
				break;
			case ID_BRUSH:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, Brush_Style);
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		

	   ////////////////////////////////////////////////-----------------------------------------------------------------------------------
		case WM_PAINT:
		{
			
			HDC hdc = BeginPaint(hWnd, &ps);
			HideCaret(hWnd); ///
			//Vẽ bitmap ra màn hình trước
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			RECT rect;
			GetClientRect(hWnd, &rect);
			BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
			switch (mode)
			{
			case 0:
				FreeDraw(memDC);
				FreeDraw(hdc);
				break;
			case 1:
				DrawLine(hdc);
				break;
			case 2:
				DrawRectangle(hdc);
				break;
			case 3:
				DrawSquare(hdc);
				break;
			case 4:
				DrawEllipse(hdc);
				break;
			case 5:
				DrawRound(hdc);
				break;
			case 6:
				DrawCurve(hdc);
				break;
			}
			SelectObject(hdc, hfont);
			SetRect(&rc, 0, 0, dwLineLen, dwClientY);
			DrawText(hdc, pchInputBuf, -1, &rc, DT_LEFT);
			DrawText(memDC, pchInputBuf, -1, &rc, DT_LEFT);
			ShowCaret(hWnd);

			DeleteObject(memDC);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			GlobalFree((HGLOBAL)pchInputBuf);
			UnregisterHotKey(hWnd, 0xAAAA);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
}




VOID DrawLine(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{
		
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
	}
}

VOID FreeDraw(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
		p1 = p2;
	}
}
VOID DrawCurve(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{
		POINT Pt[4] = { { p1.x, p1.y },{ p2.x, p2.y },{ p1.x + p2.x, p1.y + p2.y },{ p1.x + p2.y, p1.y + p2.x } };
		PolyBezier(hdc, Pt, 4);
	}
}

VOID DrawRectangle(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockBrush(NULL_BRUSH));
		MoveToEx(hdc, p1.x, p1.y, NULL);
		Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
		HRGN hRegion = CreateRectRgn(p1.x, p1.y, p2.x, p2.y);
		FillRgn(hdc, hRegion, NewBrush);
	}
}

VOID DrawSquare(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{

		MoveToEx(hdc, p1.x, p1.y, NULL);
		int temp_left = min(p1.x, p2.x);
		int temp_top = min(p1.y, p2.y);
		int temp_right = max(p1.x, p2.x);
		int temp_bot = max(p1.y, p2.y);

		int length = min(abs(p2.x - p1.x), abs(p2.y - p1.y));
		if (p2.x < p1.x)
			temp_left = temp_right - length;
		else
			temp_right = temp_left + length;

		if (p2.y < p1.y)
			temp_top = temp_bot - length;
		else
			temp_bot = temp_top + length;
		SelectObject(hdc, GetStockBrush(NULL_BRUSH));
		HRGN hRegion = CreateRectRgn(temp_left, temp_top, temp_right, temp_bot);
		Rectangle(hdc, temp_left, temp_top, temp_right, temp_bot);
		FillRgn(hdc, hRegion, NewBrush);
		
	}
}

VOID DrawEllipse(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{

		MoveToEx(hdc, p1.x, p1.y, NULL);
		Arc(hdc, p1.x, p1.y, p2.x, p2.y, 0, 0, 0, 0);
		HRGN hRegion = CreateEllipticRgn(p1.x, p1.y, p2.x, p2.y);
		FillRgn(hdc, hRegion, NewBrush);

	}

}


VOID DrawRound(HDC hdc)
{
	PEN(hdc);
	if (enTrainDessin == TRUE)
	{

		MoveToEx(hdc, p1.x, p1.y, NULL);
		int temp_left = min(p1.x, p2.x);
		int temp_top = min(p1.y, p2.y);
		int temp_right = max(p1.x, p2.x);
		int temp_bot = max(p1.y, p2.y);

		int length = min(abs(p2.x - p1.x), abs(p2.y - p1.y));
		if (p2.x < p1.x)
			temp_left = temp_right - length;
		else
			temp_right = temp_left + length;

		if (p2.y < p1.y)
			temp_top = temp_bot - length;
		else
			temp_bot = temp_top + length;
		Arc(hdc, temp_left, temp_top, temp_right, temp_bot, 0, 0, 0, 0);
		HRGN hRegion = CreateEllipticRgn(temp_left, temp_top, temp_right, temp_bot);
		FillRgn(hdc, hRegion, NewBrush);
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
		HDC hdc =  GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		switch (mode)
		{
		case 1:
			DrawLine(memDC);
			break;
		case 6:
			DrawCurve(memDC);
			break;
		}
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);
		enTrainDessin = FALSE;
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

VOID DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		switch (mode)
		{
		case 2:
			DrawRectangle(memDC);
			break;
		case 3:
			DrawSquare(memDC);
			break;
		case 4:
			DrawEllipse(memDC);
			break;
		case 5:
			DrawRound(memDC);
			break;
		}
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}





VOID PEN(HDC hdc)
{
	HPEN hPen = CreatePen(style, width, rgbCurrent );
	SelectObject(hdc, hPen);
}

VOID PenBrushDemo(HDC hdc)
{
	HPEN hPen = CreatePen(PS_DASHDOT, 3, RGB(255, 0, 255));
	SelectObject(hdc, hPen);
	MoveToEx(hdc, 100, 100, NULL);
	LineTo(hdc, 200, 200);
	Rectangle(hdc, 100, 200, 300, 300);
	Ellipse(hdc, 100, 200, 300, 300);
	HRGN hRegion = CreateEllipticRgn(100, 200, 300, 300);
	HBRUSH hbrush = CreateSolidBrush(RGB(23, 100, 40));
	FillRgn(hdc, hRegion, hbrush);
	Ellipse(hdc, 100, 200, 300, 300);
}


void Brush_Color(HWND hWnd)
{
	CHOOSECOLOR cc1;
	static COLORREF acrCustClr1[16];
	HBRUSH hbrush1;
	ZeroMemory(&cc1, sizeof(cc1));
	cc1.lStructSize = sizeof(cc1);
	cc1.hwndOwner = hWnd;
	cc1.lpCustColors = (LPDWORD)acrCustClr1;
	cc1.rgbResult = rgbCurrent2;
	cc1.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc1) == TRUE)
	{
		hbrush1 = CreateSolidBrush(cc1.rgbResult);
		rgbCurrent2 = cc1.rgbResult;
	}
}


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

INT_PTR CALLBACK Size_Pen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		switch(LOWORD(wParam))
		{
		case 1005:
			width = 1;
			break;
		case 1006:
			width = 3;
			break;
		case 1007:
			width = 5;
			break;
		case 1008:
			width = 10;
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK Style_Pen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		switch (LOWORD(wParam))
		{
		case IDC_RADIO13:
			style = PS_SOLID;
			break;
		case IDC_RADIO14:
			style = PS_DASH;
			break;
		case IDC_RADIO15:
			style = PS_DOT;
			break;
		case IDC_RADIO16:
			style = PS_DASHDOT;
			break;
		case IDC_RADIO9:
			style = PS_DASHDOTDOT;
			break;
		case IDC_RADIO10:
			style = PS_NULL;
			break;
		case IDC_RADIO11:
			style = PS_INSIDEFRAME;
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Brush_Style(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		switch (LOWORD(wParam))
		{
		case BRUSH1:
			Brush_Color(hDlg);
			NewBrush = CreateSolidBrush(rgbCurrent2);
			break;
		case IDC_RADIO1:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_VERTICAL, rgbCurrent2);
			break;
		case IDC_RADIO2:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_HORIZONTAL, rgbCurrent2);
			break;
		case IDC_RADIO4:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_CROSS, rgbCurrent2);
			break;
		case IDC_RADIO5:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_BDIAGONAL, rgbCurrent2);
			break;
		case IDC_RADIO6:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_FDIAGONAL, rgbCurrent2);
			break;
		case IDC_RADIO7:
			Brush_Color(hDlg);
			NewBrush = CreateHatchBrush(HS_FDIAGONAL, rgbCurrent2);
			break;
		case IDC_RADIO3:
			DeleteObject(NewBrush);
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}