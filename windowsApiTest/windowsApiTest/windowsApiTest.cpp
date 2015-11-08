// windowsApiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <unordered_map>


using namespace std;

/*

EnumWindows

BOOL WINAPI EnumWindows(
_In_ WNDENUMPROC lpEnumFunc,
_In_ LPARAM      lParam
);

BOOL CALLBACK EnumWindowsProc(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam
    );


BOOL WINAPI EnumChildWindows(
    _In_opt_ HWND        hWndParent,
    _In_     WNDENUMPROC lpEnumFunc,
    _In_     LPARAM      lParam
    );


BOOL CALLBACK EnumChildProc(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam
    );
    */




class WinApiData {
    int i;
    std::vector <HWND> winhandlelist;
    unordered_map <HWND, vector<HWND> > childWindowsHmap;

    void getChildWindowData(void);

public:
    void addWInHdl(HWND hdl);
    void showWindowTitle(HWND hdl);
    wstring getWindowTitle(HWND whdl);
    void showChildWindows();
    void shwoMe(void);
} winData;




BOOL CALLBACK enumChildProczzz(__in  HWND hwnd, __in  LPARAM lParam)
{

    return true;
}
void WinApiData::getChildWindowData(void)
{
    for (auto parentHandl : winhandlelist)
    {
        vector <HWND> childWinList;
        childWindowsHmap.insert({ parentHandl, childWinList });
        EnumChildWindows(parentHandl, enumChildProczzz, NULL);
        cout << "  EnumChildWindows(parentHandl, enumChildProczzz, NULL)" << endl;
    }
}


/*
void WinApiData::showChildWindows()
{
    for (auto item : winhandlelist)
    {
        bool result = EnumChildWindows(item, );
    }
}

*/


void WinApiData::addWInHdl(HWND hdl)
{
    winhandlelist.push_back(hdl);

}
void WinApiData::showWindowTitle(HWND whdl)
{
    int length = GetWindowTextLength(whdl);
    //if (0 == length) return TRUE;

    WCHAR*  buffer;
    buffer = new WCHAR[length + 1];
    memset(buffer, 0, (length + 1) * sizeof(WCHAR));

    GetWindowText(whdl, buffer, length + 1);


    wcout << whdl << TEXT(": ") << buffer << std::endl;
    delete[] buffer;

}


wstring WinApiData::getWindowTitle(HWND whdl)
{
    int length = GetWindowTextLength(whdl);

    wstring  title = L"";
    //if (0 == length) return TRUE;
    if (length > 0)
    {
        WCHAR*  buffer;
        buffer = new WCHAR[length + 1];
        memset(buffer, 0, (length + 1) * sizeof(WCHAR));

        GetWindowText(whdl, buffer, length + 1);

        //wcout << whdl << TEXT(": ") << buffer << std::endl;

        title = buffer;
        //wcout << whdl << TEXT("   zz :zz ") << title.c_str() << std::endl;
        //delete[] buffer;
    }
    return title;
}



void WinApiData::shwoMe(void)
{

    cout << "list size is " << winhandlelist.size() << endl;
    for (auto item : winhandlelist)
    {
        wstring title = getWindowTitle(item);
        int len = title.size();
        string isIcon = IsIconic(item) ? " iconic " : " ";
        if (len > 0)
        {
            wcout << item << " : " << isIcon.c_str() << " " << title.c_str() << endl << endl;
        }
    }
}



BOOL CALLBACK enumWindowsProczzz(__in   HWND hWnd, __in  LPARAM lParam)
{

    winData.addWInHdl(hWnd);
    /**/
    /*if (!  IsIconic(hWnd)) {
        return TRUE;
    }
    
     
    int length = GetWindowTextLength(hWnd);
    //if (0 == length) return TRUE;

    WCHAR*  buffer;
    buffer = new WCHAR[length + 1];
    memset(buffer, 0, (length + 1) * sizeof(TCHAR));

    GetWindowText(hWnd, buffer, length + 1);


    wcout << hWnd << TEXT(": ") << buffer << std::endl;
    delete[] buffer;
     */
    return true;
}


/*

*/

void testEnumWIndows()
{


    BOOL result = EnumWindows(enumWindowsProczzz, NULL);

    cout << "     BOOL result = EnumWindows(enumWindowsProczzz, NULL) " << endl;

    //winData.shwoMe();



}




int main()
{

    // EnumChildWindows
    testEnumWIndows();

    cin.get();

    winData.shwoMe();
    cin.get();

    return 0;
}

