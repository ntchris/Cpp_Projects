// mouseEvent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>         


using namespace std;





wstring getWindowTitle(HWND whdl)
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




class BaiduAuto {

    static const wstring BaiDuWinTitle;
    //button to open transfer window
    tagPOINT m_transferButton;
    tagPOINT m_tryFastButton;

    static HWND m_baiduWinHandle;
private:
    void mouseClickRight(tagPOINT point);

public: BaiduAuto();
        bool init(wstring wintitle);
        //for main win
        static BOOL CALLBACK enumWindowsProczzz(__in   HWND hWnd, __in  LPARAM lParam);
        //for child win
        static BOOL CALLBACK enumChildProczzz(__in  HWND childWinId, __in  LPARAM parentId);

        void showBaiduCloudWin(void);
        HWND getWinHandleByTitle(wstring wintitle);


        void openTransferWindow(void);
        void enableFastSpeed(void);
};

//const wstring BaiduAuto::BaiDuWinTitle = L"欢迎使用百度";
HWND BaiduAuto::m_baiduWinHandle = NULL;



void BaiduAuto::enableFastSpeed(void)
{
    //the window is not found! maybe the app is not running
    if (m_baiduWinHandle == 0) return;

    mouseClickRight(m_tryFastButton);
}

void BaiduAuto::mouseClickRight(tagPOINT point)
{
    const int mousex = (int)(65536.0 / 1680 * point.x - 1); //convert to absolute coordinates
    const int mousey = (int)(65536.0 / 1050 * point.y - 1);

    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, mousex, mousey, 0, 0);
    std::this_thread::sleep_for(1s);

    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, mousex, mousey, 0, 0);
    std::this_thread::sleep_for(200ms);
    mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, mousex, mousey, 0, 0);
}



BOOL CALLBACK BaiduAuto::enumWindowsProczzz(__in   HWND hWnd, __in  LPARAM lParam)
{

    wstring title = getWindowTitle(hWnd);
    wstring *targetTitle = (wstring*)lParam;
    if (title.find(*targetTitle) != std::wstring::npos)
    {
        EnumChildWindows(hWnd, enumChildProczzz, (LPARAM)hWnd);
        return false;
    }
    return true;
}


BaiduAuto::BaiduAuto()
{
    /*const int transferWinButtonX = 1242;
    const int transferWinButtonY = 295;
    m_transferButton.x = (int)(65536.0 / 1680 * transferWinButtonX - 1); //convert to absolute coordinates
    m_transferButton.y = (int)(65536.0 / 1050 * transferWinButtonY - 1);
    */
    m_transferButton.x = 1242;
    m_transferButton.y = 295;

    m_tryFastButton.x = 1058;
    m_tryFastButton.y = 371;
}





BOOL CALLBACK BaiduAuto::enumChildProczzz(__in  HWND childWinId, __in  LPARAM parentId)
{
    HWND pid = (HWND)parentId;

    m_baiduWinHandle = pid;
    return false;

}

void sleep(void)
{
    std::this_thread::sleep_for(2s);
}


HWND BaiduAuto::getWinHandleByTitle(wstring wintitle)
{
  
    EnumWindows(enumWindowsProczzz, (LPARAM)&wintitle);
    if (m_baiduWinHandle == NULL)
    {
        wcout << " getWinHandleByTitle(wstring wintitle) , m_baiduWinHandle == NULL" << endl;
        std::this_thread::sleep_for(1s);
    }

    return m_baiduWinHandle;
}


bool BaiduAuto::init(wstring wintitle)
{
    const int MaxAttempt = 10;
    for (int i = 0; i < MaxAttempt; i++)
    {

        m_baiduWinHandle = BaiduAuto::getWinHandleByTitle(wintitle);
        if (m_baiduWinHandle != 0)
        {
            wcout << " found ! " << endl;
            break;
        }
        else {
            wcout << " not found ??? try again" << endl;
        }

    }

    if (m_baiduWinHandle == 0)
    {
        return false;
    }
    else
    {
        return true;
    }

}

void BaiduAuto::showBaiduCloudWin(void)
{
    // 1 get baidu handle
   // getBaiduWinHandle();

    //now m_baiduWinHandle = the id;

    // 2 show the window
    //the window is not found! maybe the app is not running
    if (m_baiduWinHandle == 0) return;
    // SW_SHOW |

    ShowWindow(m_baiduWinHandle, SW_RESTORE);
    std::this_thread::sleep_for(1s);

    SetForegroundWindow(m_baiduWinHandle);
    std::this_thread::sleep_for(1s);

    SetActiveWindow(m_baiduWinHandle);
    wcout << "setting window foreground " << m_baiduWinHandle << endl;
    std::this_thread::sleep_for(1s);
}



void BaiduAuto::openTransferWindow(void)
{
    //the window is not found! maybe the app is not running
    if (m_baiduWinHandle == 0) return;

    mouseClickRight(this->m_transferButton);
    std::this_thread::sleep_for(2s);

}




/*  UINT WINAPI SendInput(
    _In_ UINT    nInputs,
    _In_ LPINPUT pInputs,
    _In_ int     cbSize
    );


VOID WINAPI mouse_event(
    _In_ DWORD     dwFlags,
    _In_ DWORD     dx,
    _In_ DWORD     dy,
    _In_ DWORD     dwData,
    _In_ ULONG_PTR dwExtraInfo
    );



    HWND WINAPI SetActiveWindow(
    _In_ HWND hWnd
    );


    BOOL WINAPI SetForegroundWindow(
    _In_ HWND hWnd
    );




    BOOL WINAPI ShowWindow(
    _In_ HWND hWnd,
    _In_ int  nCmdShow
    );


    */






    /*
    BOOL WINAPI GetCursorPos(
        _Out_ LPPOINT lpPoint
        );


    BOOL WINAPI TrackMouseEvent(
        _Inout_ LPTRACKMOUSEEVENT lpEventTrack
        );
        */



void getMouseXY()
{

    LPPOINT  lpPoint = new tagPOINT();
    lpPoint->x = 0;
    lpPoint->y = 0;
    GetCursorPos(lpPoint);
    wcout << "x: " << lpPoint->x << " , y:" << lpPoint->y << endl;
}

void loopGetMouseXY()
{
    for (int i = 0; i < 10; i++)
    {
        getMouseXY();
        std::this_thread::sleep_for(1s);
    }

}



int main()
{
    const wstring BaiDuWinTitle = L"欢迎使用百度";
    //const wstring BaiDuWinTitle = L"foobar";

    locale loc("chs");

    wcout.imbue(loc);
    //loopGetMouseXY();
    //return 1;

    BaiduAuto *myBaiduAutop = new BaiduAuto();

    bool success = myBaiduAutop->init(BaiDuWinTitle);

    if (!success) {
        wcout << BaiDuWinTitle << " App is not running?" << endl;
        //cin.get();

        return 1;
    }

    myBaiduAutop->showBaiduCloudWin();

    myBaiduAutop->openTransferWindow();

    myBaiduAutop->enableFastSpeed();

   

    return 0;
}


//std::thread first(testEnumWIndows);

// EnumChildWindows
//testEnumWIndows();

//winData.shwoMe();

// mouseClickTransferList();

//use this to capture mouse xy
//std::thread mouseThread(loopGetMouseXY);


