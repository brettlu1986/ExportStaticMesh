
#include "MainWindow.h"
#include "ApplicationMain.h"
#include "LInput.h"
#include "LogicStaticModel.h"
#include <windowsx.h>

ApplicationMain* MainWindow::MainApplication = nullptr;

MainWindow::MainWindow()
	:hMainWnd(nullptr)

{

}

MainWindow::~MainWindow()
{

}

void MainWindow::Initialize(ApplicationMain* Application, UINT Width, UINT Height, std::wstring Name)
{
	MainApplication = Application;

	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = MainApplication->GetHInstace();
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = L"MainWindow";
	RegisterClassEx(&WindowClass);

	RECT WindowRect = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	hMainWnd = CreateWindow(
		WindowClass.lpszClassName,
		Name.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		MainApplication->GetHInstace(),
		nullptr);

	ShowWindow(hMainWnd, SW_SHOWDEFAULT);
}

bool MainWindow::Run()
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

void MainWindow::Destroy()
{
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			MainApplication->GetGraphicRender()->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		{
			MainApplication->GetGraphicRender()->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		return 0;
		case WM_MOUSEMOVE:
		{
			MainApplication->GetGraphicRender()->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		return 0;
		case WM_KEYDOWN:
		{
			MainApplication->GetInput()->OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;
		case WM_KEYUP:
		{
			MainApplication->GetInput()->OnKeyUp(static_cast<UINT8>(wParam));
		}
		return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	return ::DefWindowProc(hWnd, Message, wParam, lParam);
}
