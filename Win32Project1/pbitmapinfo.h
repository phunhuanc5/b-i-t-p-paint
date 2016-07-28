#pragma once
#include "stdafx.h"
#include "Win32Project1.h"
#include "Windowsx.h"
#include <iostream>
#include <Windows.h>
#include "Commdlg.h"
#include <Strsafe.h>
using namespace std;

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
VOID CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC);
//bool LoadBitmap(LPCWSTR szFileName, HDC hWinDC);