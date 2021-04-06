

#include "LDeviceWindows.h"
#include <windowsx.h>

LDeviceWindows::LDeviceWindows(UINT InW, UINT InH, const char* Name)
:LDevice(InW, InH)
,hMainWnd(nullptr)
{
	CreateMainWindow(Name);
}

LDeviceWindows::~LDeviceWindows()
{

}

void LDeviceWindows::CreateMainWindow(const char* Name)
{
	size_t Len = strlen(Name) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, Name, _TRUNCATE);

	HINSTANCE hInstance = GetModuleHandle(0);
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = WStr;
	RegisterClassEx(&WindowClass);

	RECT WindowRect = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	hMainWnd = CreateWindowW(
		WindowClass.lpszClassName,
		WStr,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		nullptr);

	delete []WStr;
	ShowWindow(hMainWnd, SW_SHOWDEFAULT);
}

void LDeviceWindows::CreateInput()
{
	Input = new LInput();
	Input->Initialize();
}

LRESULT CALLBACK LDeviceWindows::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return LInput::MessageHandler(hWnd, Message, wParam, lParam);
		}
	}
}

bool LDeviceWindows::Run()
{
	MSG Msg = {};
	// Process any messages in the queue.
	if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.message != WM_QUIT;
}


void LDeviceWindows::Destroy()
{
	if(Input)
	{
		delete Input;
		Input = nullptr;
	}
}

