// windowsApiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>

using namespace std;

/*
BOOL CALLBACK EnumWindowsProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
	);

    */


BOOL CALLBACK enumWindowsProczzz(
    __in  HWND hWnd,
    __in  LPARAM lParam
    ) 
{
    if (!::IsIconic(hWnd)) {
        return TRUE;
    }

    int length = ::GetWindowTextLength(hWnd);
    if (0 == length) return TRUE;

    char* buffer;
    buffer = new char[length + 1];
    memset(buffer, 0, (length + 1) * sizeof(TCHAR));

    GetWindowText(hWnd, buffer, length + 1);
    string windowTitle =  (buffer);   
    delete[] buffer;

    wcout << hWnd << TEXT(": ") << windowTitle.c_str() << std::endl;

    return TRUE;
}



int main()
{

    BOOL result = ::EnumWindows(enumWindowsProczzz, NULL);
	// EnumChildWindows

    return 0;
}

