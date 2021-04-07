

#include "LInputWindows.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LDefine.h"
#include "LDeviceWindows.h"
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

inline EventDispatcher& GetDispather()
{
	return LEngine::GetEngine()->GetEventDispacher();
}

inline LDeviceWindows* GetWindowsDevice()
{
	return static_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
}

LRESULT CALLBACK LInputWindows::MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam)
{
	switch (Umsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_DOWN, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		SetCapture(GetWindowsDevice()->GetHwnd());
	}
	return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_UP, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		ReleaseCapture();
	}
	return 0;
	case WM_MOUSEMOVE:
	{
		if ((Wparam & MK_LBUTTON) != 0)
		{
			FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_MOVE, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
			GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		}
	}
	return 0;
	case WM_KEYDOWN:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_DOWN, E_KEY_MAP::DOWN, 0, 0 };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
	}
	return 0;
	case WM_KEYUP:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_UP, E_KEY_MAP::UP, 0, 0 };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
	}
	return 0;
	default:
	{
		return ::DefWindowProc(Hwnd, Umsg, Wparam, Lparam);
	}
	}
}