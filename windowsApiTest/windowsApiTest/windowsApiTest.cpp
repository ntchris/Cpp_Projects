// windowsApiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <thread>         // std::thread

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


    //BOOL CALLBACK  enumChildProczzz(__in  HWND hwnd, __in  LPARAM lParam);


class WinApiData {
    int i;
    std::vector <HWND> winhandlelist;

    unordered_map <HWND, vector<HWND> *  > childWindowsHmap;
    //unordered_map <HWND,   HWND  > childWindowsHmap;

public:

    void getChildWindowData(void);

    void addWInHdl(HWND hdl);
    void showWindowTitle(HWND hdl);
    void showChildWindows();
    void shwoMe(void);

    void addChildWinTo(HWND parentId);

    static BOOL CALLBACK enumChildProczzz(__in  HWND hwnd, __in  LPARAM lParam);
    static   wstring getWindowTitle(HWND whdl);

} winData;



BOOL  CALLBACK     WinApiData::enumChildProczzz(__in  HWND childWinId, __in  LPARAM parentId)
{
    HWND pid = (HWND)parentId;
    auto mypair = (winData.childWindowsHmap).find(pid);

    if (mypair == winData.childWindowsHmap.end())
    {
        //make a new child list if it's not there yet
        std::pair <HWND, vector<HWND> * >  newHashMapItem; //  parentID, childList
        vector<HWND> *childWinList_p = new vector<HWND>;
        newHashMapItem.first = (HWND)parentId;
        newHashMapItem.second = childWinList_p;
        childWinList_p->push_back(childWinId);
        winData.childWindowsHmap.insert(newHashMapItem);

        return true;
    }
    else
    {
        //get the vector of child win and add the new one
        mypair->second->push_back(childWinId);

    }

    return true;
}



//"BOOL (__stdcall WinApiData::*)(HWND hwnd, LPARAM lParam)" is incompatible with parameter of type "WNDENUMPROC"	windowsApiTest	h : \GitHubRoot\Cpp_Projects\windowsApiTest\windowsApiTest\windowsApiTest.cpp	101

 //   typedef BOOL(*WNDENUMPROC)(HWND, LPARAM)
void WinApiData::getChildWindowData(void)
{
    // iterate the current stored win(main windows) list
    for (auto parentHandl : winhandlelist)
    {


        vector <HWND> *childWinListp = new  vector <HWND>;

        LPARAM lparam = (LPARAM)parentHandl;

        wstring title = WinApiData::getWindowTitle(parentHandl);

        //if (title.find(L"百度") != std::wstring::npos)
         //wcout << "  EnumChildWindows(parentHandl, enumChildProczzz, aaa)" << parentId << endl;
        EnumChildWindows(parentHandl, enumChildProczzz, lparam);
         
    }

}

///    BOOL result = EnumWindows(enumWindowsProczzz, NULL);




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
    int count = 0;
    wcout << "list size is " << winhandlelist.size() << endl;
    for (auto parentID : winhandlelist)
    {
        wstring title = WinApiData::getWindowTitle(parentID);
        //deal with child windows

        auto childInfoPair = this->childWindowsHmap.find(parentID);
        if (childInfoPair != childWindowsHmap.end())
        {
            auto childList = childInfoPair->second;
            for (auto childId : *childList)
            {
                // note :  not all main window has child win/ or in the hashmap
                wstring title = WinApiData::getWindowTitle(childId);

                wcout << "Child Win is " << childId << " !!" << title << " !! "<<endl;
            }
        }
    }

}



BOOL CALLBACK enumWindowsProczzz(__in   HWND hWnd, __in  LPARAM lParam)
{
    wstring title = WinApiData::getWindowTitle(hWnd);
    if (title.find(L"百度") != std::wstring::npos)
    {
        winData.addWInHdl(hWnd);
    }
    return true;
}


/*

*/

void testEnumWIndows()
{


    BOOL result = EnumWindows(enumWindowsProczzz, NULL);

    wcout << "     BOOL result = EnumWindows(enumWindowsProczzz, NULL) " << endl;

    winData.getChildWindowData();


    winData.shwoMe();




}




int main()
{
    // important: otherwise can not output or output end prematurely
    locale loc("chs");

    wcout.imbue(loc);

    //std::thread first(testEnumWIndows);

    // EnumChildWindows
    testEnumWIndows();



    //winData.shwoMe();



    cin.get();
    return 0;
}

