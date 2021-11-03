/*
    Рисование оконных структур
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "Painter.h"
#include "window.h"
#include "drawing.h"


// имя Этой программы
static TCHAR szAppName[] = TEXT("Painter");

// обработчик сообщений
static LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

// основное окно
static HWND hwnd = NULL;


// интерфейс к сообщению об ошибке
int MsgBoxError (const TCHAR *str)
{
    return MessageBox(hwnd, str, szAppName,
        MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION | MB_TOPMOST);
}


// интерфейс к информационному сообщению
int MsgBoxInfo (const TCHAR *str)
{
    return MessageBox(hwnd, str, szAppName,
        MB_OK | MB_APPLMODAL | MB_ICONINFORMATION | MB_TOPMOST);
}


// создаем главное окно
int CreateWindowStructure (HINSTANCE hInstance, int iCmdShow)
{
    // класс главного окна данной программы
    WNDCLASS wndclass = { 0 };
    WNDCLASSEX wndclassex = { 0 };

    // регистрируем класс главного окна
    wndclassex.cbSize = sizeof(wndclassex);
    wndclassex.style         = wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = wndclass.lpfnWndProc   = WndProc;
    wndclassex.cbClsExtra    = wndclass.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = wndclass.cbWndExtra    = 0;
    wndclassex.hInstance     = wndclass.hInstance     = hInstance;
    // дефолтовая иконка (SM_CXICON*SM_CYICON)
    wndclassex.hIcon         = wndclass.hIcon         = LoadIcon(hInstance, TEXT("APPICON"));
    // маленькая иконка (SM_CXSMICON*SM_CYSMICON)
    wndclassex.hIconSm = LoadImage(hInstance, TEXT("APPICON"), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    wndclassex.hCursor       = wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclassex.hbrBackground = wndclass.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wndclassex.lpszMenuName  = wndclass.lpszMenuName  = NULL;
    wndclassex.lpszClassName = wndclass.lpszClassName = szAppName;
    // в зависимости от версии win32 будет работать разная функция
    if ((RegisterClassEx(&wndclassex) == 0) && (RegisterClass(&wndclass) == 0))
        return 1;

    // создаем главное окно
    hwnd = CreateWindowEx(0,
                szAppName,              // window class name
                szAppName,              // window caption
                WS_OVERLAPPEDWINDOW,    // window style
                CW_USEDEFAULT,          // initial x position
                CW_USEDEFAULT,          // initial y position
                CW_USEDEFAULT,          // initial x size
                CW_USEDEFAULT,          // initial y size
                NULL,                   // parent window handle
                NULL,                   // window menu handle
                hInstance,              // program instance handle
                NULL);                  // creation parameters


    // RECT winRect;
    // GetClientRect(hwnd, &winRect);
    // FrameInfo frameInfo = getFrameInfo(X_FROM, X_TO, Y_FROM, Y_TO, winRect);
    // TCHAR info[300] = { 0 };
    // tsprintf(info, 300, TEXT("meas: (%f %f), (%f %f)\npix: (%d %d), (%d %d)\ncent: (%d %d)\n\
    //     pix in meas: %f\nwind size: (%d %d)"), 
    //     frameInfo.topLeftMeas.X, frameInfo.topLeftMeas.Y, 
    //     frameInfo.bottomRightMeas.X, frameInfo.bottomRightMeas.Y, 
    //     frameInfo.topLeftPix.X, frameInfo.topLeftPix.Y, 
    //     frameInfo.bottomRightPix.X, frameInfo.bottomRightPix.Y, 
    //     frameInfo.centerPix.X, frameInfo.centerPix.Y, 
    //     frameInfo.numPixInMeas, 
    //     winRect.right - winRect.left, winRect.bottom - winRect.top);

    // HWND hwndStat1 = CreateWindowEx(0,
    //             TEXT("STATIC"), info,
    //             WS_CHILD | WS_VISIBLE | SS_RIGHT,
    //             5, 5, 300, 100,
    //             hwnd, NULL, hInstance, NULL);
    // SetParent(hwndStat1, hwnd);


    // показываем созданную структуру окон
    ShowWindow(hwnd, iCmdShow);

    return 0;
}


// удаляем ту структуру окон, которая не может разрушиться автоматически
void DestroyWindowStructure (void)
{
}

void drawLines(HDC hdc);

// обработчик сообщений
static LRESULT CALLBACK WndProc (HWND lhwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        HDC hdc;
        PAINTSTRUCT ps;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            RECT winRect;
            GetClientRect(hwnd, &winRect);
            FrameInfo frameInfo = getFrameInfo(X_FROM, X_TO, Y_FROM, Y_TO, winRect);
            // drawFrame(hdc, frameInfo, RGB(0, 255, 0));
            drawAxes(hdc, frameInfo, RGB(255, 0, 0));
            drawGraph(hdc, func, frameInfo, RGB(0, 0, 255));
            TCHAR sVal[100] = { 0 };
            COORDDouble maxPoint = findMaxPoint(func, frameInfo);
            tsprintf(sVal, 100, TEXT("Максимальное значение на промежутке [%.3f, %.3f] равно %.3f в точке %.3f"), 
                frameInfo.topLeftMeas.X, frameInfo.bottomRightMeas.X, 
                maxPoint.Y, maxPoint.X);
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 5, 5, sVal, tstrlen(sVal));
            EndPaint(hwnd, &ps);
            break;

        case WM_QUIT:  // Indicates a request to terminate an application, and is generated when the application calls the PostQuitMessage function. This message causes the GetMessage function to return zero.
            // это сообщение, посланное любому нашему окну завершает жизнь программы
            DestroyWindowStructure();
            return 0;

        case WM_DESTROY:  // Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen. This message is sent first to the window being destroyed and then to the child windows (if any) as they are destroyed. During the processing of the message, it can be assumed that all child windows still exist.
            DestroyWindowStructure();
            // если любое наше окно разрушено, завершаем работу
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(lhwnd, iMsg, wParam, lParam);
}

void drawLines(HDC hdc)
{
    MoveToEx(hdc, 50, 50, NULL);
    LineTo(hdc, 250, 50);

    HPEN hWhitePen = GetStockObject(WHITE_PEN);
    HPEN hRedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = SelectObject(hdc, hRedPen);

    MoveToEx(hdc, 50, 100, NULL);
    LineTo(hdc, 250, 100);

    SelectObject(hdc, hOldPen);
}