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
#include <Commdlg.h>
#include <Strsafe.h>
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


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT p1, p2, p2_ancien;
int width, R, G, B;
HBITMAP hBitmap;
BOOL enTrainDessin;
LOGFONT lf;
HFONT hfont, hfontPrev;
DWORD rgbCurrent, rgbCurrent1;

int mode; //mode de dessin==> 0 pour ligne, 1 pour libre
		  // Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID				DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK size(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawLine(HDC hdc);
VOID FreeDraw(HDC hdc);
VOID DrawCurve(HDC hdc);
VOID DrawRectangle(HDC hdc);
VOID DrawSquare(HDC hdc);
VOID PenBrushDemo(HDC hdc);
VOID DrawEllipse(HDC hdc);
VOID DrawRound(HDC hdc);
VOID PEN(HDC hdc, int width, int R, int G, int B);
//VOID errhandler(LPCTSTR sz, HWND hwnd);
//PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
//VOID CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
	//HBITMAP hBMP, HDC hDC);
BOOL  BrushDlgProc(HWND hdlg, UINT message, LONG wParam,
	LONG lParam);
int GetStrLngth(LPTSTR cArray);


DWORD RetrieveWidth(LPTSTR cArray, int iLength);

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
/*
//--------------------
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;
	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		errhandler(TEXT("GetObject"), hwnd);
	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;
	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  
	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD)* (1 << cClrBits));
	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));
	// Initialize the fields in the BITMAPINFO structure.  
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;
	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}
VOID CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp;
	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	if (!lpBits)
		errhandler(_T("GlobalAlloc"), hwnd);
	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		errhandler(_T("GetDIBits"), hwnd);
	}
	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
		errhandler(_T("CreateFile"), hwnd);
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
								// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);
	// Copy the BITMAPFILEHEADER into the .BMP file.  
	if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL))
	{
		errhandler(_T("WriteFile"), hwnd);
	}
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL)))
		errhandler(_T("WriteFile"), hwnd);
	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
		errhandler(_T("WriteFile"), hwnd);
	// Close the .BMP file.  
	if (!CloseHandle(hf))
		errhandler(_T("CloseHandle"), hwnd);
	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}
VOID errhandler(LPCTSTR sz, HWND hwnd)
{
	//DO SOMETHING
}

//----------------------------

*/
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
	BrushDlgProc(hWnd, message, wParam, lParam);
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

	////

	switch (message)
	{
	case WM_CREATE:
	{
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
		hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

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
	////5

	

		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_FILE_OPEN:
				/*
				hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

		//Tô background cho bitmap
		//Tạo memory dc để tương tác với Bitmap
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(memDC, &rect, hBrush);
		DeleteObject(hBrush);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hdc);
				*/
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
					//MessageBox(hWnd, ofn.lpstrFile, NULL, MB_OKCANCEL | MB_ICONQUESTION);
					/*hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
						0, (LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);*/
					//BITMAP bitmap = new bitmap(ofn.lpstrFile);
					//e.Graphics.DrawImage(bitmap, 60, 10);

					hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					HDC memDC = CreateCompatibleDC(hdc);
					SelectObject(memDC, hBitmap);
					RECT rect;
					GetClientRect(hWnd, &rect);
					BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

					DeleteObject(memDC);
				
				}
				//MessageBox(hWnd, ofn.lpstrFile,NULL, MB_OKCANCEL | MB_ICONQUESTION);
				/*OF_READWRITE (hf == (HANDLE)-1)
				{
				MessageBox(NULL, L"Could not open this file", L"File I/O Error", MB_ICONSTOP);
				break;
				}*/

				break;
			case ID_FILE_SAVEAS:
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
					CreateBMPFile(hWnd, _T("D:\\bitmap.bmp"), PBI, hBitmap, hdc);
					MessageBox(hWnd, _T("Save sucess"), _T("Confirm close"), MB_OK);
				}
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_FILE_NEW:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, size);
				break;
			case ID_FILE_EXIT:
				if (MessageBox(hWnd, _T("Are you sure to close?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
				{
					/*if (saved == false)
						wmId = 32775;*/
					if (wmId != 32775)
						DestroyWindow(hWnd);
				}
				break;
			case 32778:
				mode = 1;
				break;
			case 32780:
				mode = 0;
				break;
			case 32781:
				mode = 2;
				break;
			case 32782:
				mode = 3;
				break;
			case 32784:
				mode = 4;
				break;
			case 32783:
				mode = 5;
				break;
			case 32779:
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
			case 32804:
				width = 1;
				break;
			case 32805:
				width = 3;
				break;
			case 32806:
				width = 5;
				break;
			case 32807:
				width = 10;
				break;
			case 32799:
				CHOOSECOLOR cc;                 // common dialog box structure 
				static COLORREF acrCustClr[16]; // array of custom colors 
				//HWND hwnd;                      // owner window
				HBRUSH hbrush;                  // brush handle
				//static DWORD rgbCurrent;        // initial color selection

												// Initialize CHOOSECOLOR 
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
			case 32798:
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
					//SelectObject(hdc, hfont);
					static TCHAR szString[] = TEXT( "SDBFUWDC");
				//	TextOut(hdc, 0, 0, szString, lstrlen(szString));
						
						
				}

				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case 32802:
			 CreateDialog(hInst, TEXT("BRUSHBOX"), hWnd, About);
			//BrushDlgProc(hWnd, message, wParam, lParam);
			break;
	
		case WM_PAINT:
		{
			//PAINTSTRUCT ps;
			//saved = false;
			// TODO: Add any drawing code that uses hdc here...
		//	if (cch == 0)       /// nothing in input buffer 
		//		break;
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
				//Vẽ tự do thì ta vẽ luôn trên màn hình và bitmap
				FreeDraw(memDC);
				FreeDraw(hdc);
				break;
			case 1:
				//Vẽ đường thẳng thì ta chỉ vẽ lên màn hình
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
			//SelectObject(hDc, hPen);

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
	PEN(hdc, width, R, G, B);
	if (enTrainDessin == TRUE)
	{
		
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);


	}
}

VOID FreeDraw(HDC hdc)
{
	PEN(hdc, width, R, G, B);
	if (enTrainDessin == TRUE)
	{
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
		p1 = p2;
	}
}
VOID DrawCurve(HDC hdc)
{
	PEN(hdc, width, R, G, B);
	if (enTrainDessin == TRUE)
	{
		POINT Pt[4] = { { p1.x, p1.y },{ p2.x, p2.y },{ p1.x + p2.x, p1.y + p2.y },{ p1.x + p2.y, p1.y + p2.x } };

		//MoveToEx(hdc, p1.x, p1.y, NULL);
		PolyBezier(hdc, Pt, 4);


	}
}

VOID DrawRectangle(HDC hdc)
{
	PEN(hdc, width, R, G, B);
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockBrush(NULL_BRUSH));
		MoveToEx(hdc, p1.x, p1.y, NULL);
		HRGN hRegion = CreateRectRgn(p1.x, p1.y, p2.x, p2.y);
			Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
		//HBRUSH hbrush = CreateSolidBrush(RGB(23, 100, 40));
		//	HBRUSH hbrush = CreateHatchBrush(HS_CROSS, RGB(0, 0, 0));
		//	HBRUSH hbrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
			
		//FillRgn(hdc, hRegion, hbrush);
	}
}

VOID DrawSquare(HDC hdc)
{
	PEN(hdc, width, R, G, B);

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
		Rectangle(hdc, temp_left, temp_top, temp_right, temp_bot);
	}
}

VOID DrawEllipse(HDC hdc)
{
	PEN(hdc, width, R, G, B);
	if (enTrainDessin == TRUE)
	{

		MoveToEx(hdc, p1.x, p1.y, NULL);
		Arc(hdc, p1.x, p1.y, p2.x, p2.y, 0, 0, 0, 0);
	}

}


VOID DrawRound(HDC hdc)
{
	PEN(hdc, width, R, G, B);
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

		//Tiến hành cho vẽ lên bitmap
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

		//Tiến hành cho vẽ lên bitmap
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





VOID PEN(HDC hdc, int width, int R, int G, int B)
{
	HPEN hPen = CreatePen(PS_SOLID, width, rgbCurrent );
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

INT_PTR CALLBACK size(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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


BOOL BrushDlgProc(HWND hdlg, UINT message, LONG wParam,
	LONG lParam)
{
	static HWND hwndGrid;       // grid-window control  
	static HWND hwndBrush;      // pattern-brush control  
	static RECT rctGrid;        // grid-window rectangle  
	static RECT rctBrush;       // pattern-brush rectangle  
	static UINT bBrushBits[8];  // bitmap bits  
	static RECT rect[64];       // grid-cell array  
	static HBITMAP hbm;         // bitmap handle  
	HBRUSH hbrush;              // current brush  
	HBRUSH hbrushOld;           // default brush  
	HRGN hrgnCell;              // test-region handle  
	HDC hdc;                    // device context (DC) handle  
	int x, y, deltaX, deltaY;   // drawing coordinates  
	POINTS ptlHit;              // mouse coordinates  
	int i;                      // count variable  

	switch (message)
	{
	case WM_INITDIALOG:

		// Retrieve a window handle for the grid-window and  
		// pattern-brush controls.  

		hwndGrid = GetDlgItem(hdlg, IDD_GRID);
		hwndBrush = GetDlgItem(hdlg, IDD_RECT);

		// Initialize the array of bits that defines the  
		// custom brush pattern with the value 1 to produce a  
		// solid white brush.  

		for (i = 0; i<8; i++)
			bBrushBits[i] = 0xFF;

		// Retrieve dimensions for the grid-window and pattern- 
		// brush controls.  

		GetClientRect(hwndGrid, &rctGrid);
		GetClientRect(hwndBrush, &rctBrush);

		// Determine the width and height of a single cell.  

		deltaX = (rctGrid.right - rctGrid.left) / 8;
		deltaY = (rctGrid.bottom - rctGrid.top) / 8;

		// Initialize the array of cell rectangles.  

		for (y = rctGrid.top, i = 0; y < rctGrid.bottom; y += deltaY)
		{
			for (x = rctGrid.left; x < (rctGrid.right - 8) && i < 64;
				x += deltaX, i++)
			{
				rect[i].left = x; rect[i].top = y;
				rect[i].right = x + deltaX;
				rect[i].bottom = y + deltaY;
			}
		}
		return FALSE;


	case WM_PAINT:

		// Draw the grid.  

		hdc = GetDC(hwndGrid);

		for (i = rctGrid.left; i<rctGrid.right;
			i += (rctGrid.right - rctGrid.left) / 8)
		{
			MoveToEx(hdc, i, rctGrid.top, NULL);
			LineTo(hdc, i, rctGrid.bottom);
		}
		for (i = rctGrid.top; i<rctGrid.bottom;
			i += (rctGrid.bottom - rctGrid.top) / 8)
		{
			MoveToEx(hdc, rctGrid.left, i, NULL);
			LineTo(hdc, rctGrid.right, i);
		}
		ReleaseDC(hwndGrid, hdc);
		return FALSE;


	case WM_LBUTTONDOWN:

		// Store the mouse coordinates in a POINT structure.  

		ptlHit = MAKEPOINTS(lParam);

		// Create a rectangular region with dimensions and  
		// coordinates that correspond to those of the grid  
		// window.  

		hrgnCell = CreateRectRgn(rctGrid.left, rctGrid.top,
			rctGrid.right, rctGrid.bottom);

		// Retrieve a window DC for the grid window.  

		hdc = GetDC(hwndGrid);

		// Select the region into the DC.  

		SelectObject(hdc, hrgnCell);

		// Test for a button click in the grid-window rectangle.  

		if (PtInRegion(hrgnCell, ptlHit.x, ptlHit.y))
		{
			// A button click occurred in the grid-window  
			// rectangle; isolate the cell in which it occurred.  

			for (i = 0; i<64; i++)
			{
				DeleteObject(hrgnCell);

				hrgnCell = CreateRectRgn(rect[i].left,
					rect[i].top,
					rect[i].right, rect[i].bottom);

				if (PtInRegion(hrgnCell, ptlHit.x, ptlHit.y))
				{
					InvertRgn(hdc, hrgnCell);

					// Set the appropriate brush bits.  

					if (i % 8 == 0)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x80;
					else if (i % 8 == 1)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x40;
					else if (i % 8 == 2)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x20;
					else if (i % 8 == 3)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x10;
					else if (i % 8 == 4)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x08;
					else if (i % 8 == 5)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x04;
					else if (i % 8 == 6)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x02;
					else if (i % 8 == 7)
						bBrushBits[i / 8] = bBrushBits[i / 8] ^ 0x01;

					// Exit the "for" loop after the bit is set.  

					break;
				} // end if  

			} // end for  

		} // end if  

		  // Release the DC for the control.  

		ReleaseDC(hwndGrid, hdc);
		return TRUE;


	case WM_COMMAND:
		switch (wParam)
		{
		case IDD_PAINTRECT:

			hdc = GetDC(hwndBrush);

			// Create a monochrome bitmap.  

			hbm = CreateBitmap(8, 8, 1, 1,
				(LPBYTE)bBrushBits);

			// Select the custom brush into the DC.  

			hbrush = CreatePatternBrush(hbm);

			hbrushOld =(HBRUSH) SelectObject(hdc, hbrush);

			// Use the custom brush to fill the rectangle.  

			Rectangle(hdc, rctBrush.left, rctBrush.top,
				rctBrush.right, rctBrush.bottom);

			// Clean up memory.  
			SelectObject(hdc, hbrushOld);
			DeleteObject(hbrush);
			DeleteObject(hbm);

			ReleaseDC(hwndBrush, hdc);
			return TRUE;

		case IDD_OK:

		case IDD_CANCEL:
			EndDialog(hdlg, TRUE);
			return TRUE;

		} // end switch  
		break;
	default:
		return FALSE;
	}
}


int GetStrLngth(LPTSTR cArray)
{
	int i = 0;

	while (cArray[i++] != 0);
	return i - 1;

}

DWORD RetrieveWidth(LPTSTR cArray, int iLength)
{
	int i, iTmp;
	double dVal, dCount;

	dVal = 0.0;
	dCount = (double)(iLength - 1);
	for (i = 0; i<iLength; i++)
	{
		iTmp = cArray[i] - 0x30;
		dVal = dVal + (((double)iTmp) * pow(10.0, dCount--));
	}

	return (DWORD)dVal;
}

