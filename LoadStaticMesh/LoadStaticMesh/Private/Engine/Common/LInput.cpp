

#include "LInput.h"
#include <windowsx.h>
#include "LDeviceWindows.h"
#include "LEngine.h"

LInput* LInput::MainInput = nullptr;

LInput::LInput()
{
	MainInput = this;
}

LInput::~LInput()
{

}

void LInput::Initialize()
{
}

LRESULT CALLBACK LInput::MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam)
{
	switch(Umsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{	
		LDeviceWindows* Device = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
		GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_DOWN, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
		SetCapture(Device->GetHwnd());
	}
	return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	{
		GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_UP, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
		ReleaseCapture();
	}
	return 0;
	case WM_MOUSEMOVE:
	{
		GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_MOUSE_EVENT, E_TOUCH_TYPE::MOUSE_LEFT_MOVE, E_KEY_MAP::INPUT_UNKNOWN, GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
	}
	return 0;
	case WM_KEYDOWN:
	{
		GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_DOWN, E_KEY_MAP::DOWN);
	}
	return 0;
	case WM_KEYUP:
	{
		GetInput()->UpdateInputResult(E_INPUT_TYPE::PC_KEYBORAD, E_TOUCH_TYPE::KEY_UP, E_KEY_MAP::UP);
	}
	return 0;
	default:
	{
		return ::DefWindowProc(Hwnd, Umsg, Wparam, Lparam);
	}
	}
}

bool LInput::Update()
{

	return true;
}

void LInput::Destroy()
{
}