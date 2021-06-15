
#include "stdafx.h"
#include "LInputWindows.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LDefine.h"
#include "LDeviceWindows.h"
#include "LLog.h"
#include <windowsx.h>

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

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

void LInputWindows::GetCurrentCursorLocation(POINT& p)
{
	LDeviceWindows* DeviceWindows = static_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	GetCursorPos(&p);
	ScreenToClient(DeviceWindows->GetHwnd(), &p);
}

bool LInputWindows::Update() 
{
	LInput::Update();
	//'A' ¨C 'Z'  A - Z 0x41 - 0x5A
	//if (KEYDOWN('A') || KEYDOWN('S') || KEYDOWN('W') || KEYDOWN('D'))
	//{
	//	LLog::Log("ASWD down \n");
	//}

	//if (KEYUP('A') || KEYUP('S') || KEYUP('W') || KEYUP('D'))
	//{
	//	LLog::Log("ASWD UP \n");
	//}

	return true;
}

LRESULT CALLBACK LInputWindows::MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam)
{
	switch (Umsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_DOWN, 0, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		SetCapture(GetWindowsDevice()->GetHwnd());
	}
	return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_UP, 0, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		ReleaseCapture();
	}
	return 0;
	case WM_MOUSEMOVE:
	{
		if ((Wparam & MK_LBUTTON) != 0)
		{
			FInputResult Result = { E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_MOVE, 0, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
			GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
		}
	}
	return 0;
	case WM_KEYUP:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_UP, static_cast<UINT8>(Wparam), 0, 0 };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
	}
	return 0;
	case WM_KEYDOWN:
	{
		FInputResult Result = { E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_DOWN, static_cast<UINT8>(Wparam), 0, 0 };
		GetDispather().DispatchEvent(E_EVENT_KEY::EVENT_INPUT, Result);
	}
	return 0;
	default:
	{
		return ::DefWindowProc(Hwnd, Umsg, Wparam, Lparam);
	}
	}
}