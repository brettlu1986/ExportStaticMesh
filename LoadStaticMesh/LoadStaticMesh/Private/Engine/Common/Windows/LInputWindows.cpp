

#include "LInputWindows.h"
#include "LEngine.h"
//#include "LEvent.h"
#include "LDefine.h"
#include <windowsx.h>

LInputWindows::LInputWindows()
{

}


LInputWindows::~LInputWindows()
{

}

void LInputWindows::Initialize()
{
}

LRESULT CALLBACK LInputWindows::MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam)
{
	switch (Umsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		//FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_DOWN, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };

		//LEngine::GetEngine()->GetEventDispather().dispatchEvent
		/*GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_DOWN, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
		SetCapture(Device->GetHwnd());*/
	}
	return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	{
		/*GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_UP, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
		ReleaseCapture();*/
	}
	return 0;
	case WM_MOUSEMOVE:
	{
		//GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_MOVE, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
	}
	return 0;
	case WM_KEYDOWN:
	{
		//GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_DOWN, E_KEY_MAP::DOWN);
	}
	return 0;
	case WM_KEYUP:
	{
		//GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_UP, E_KEY_MAP::UP);
	}
	return 0;
	default:
	{
		return ::DefWindowProc(Hwnd, Umsg, Wparam, Lparam);
	}
	}
}