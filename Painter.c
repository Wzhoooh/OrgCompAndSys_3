/*
    Стартовый модуль
*/

#include <windows.h>

#include "Painter.h"
#include "window.h"


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
		char *szCmdLine, int iCmdShow)
{
    // работаем с оконной структурой
    if (CreateWindowStructure(hInstance, iCmdShow))
    {
        MsgBoxError(TEXT("Не удалось создать главное окно программы"));
        return 0;
    }

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindowStructure();

    return msg.wParam;
}

