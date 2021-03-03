
#include "MainWindow.h"
#include "ApplicationMain.h"
#include "InputClass.h"

ApplicationMain* MainWindow::m_application = nullptr;

MainWindow::MainWindow()
	:m_hwnd(nullptr)

{

}

MainWindow::~MainWindow()
{

}

void MainWindow::Initialize(ApplicationMain* application,  UINT width, UINT height, std::wstring name)
{
	m_application = application; 

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = m_application->GetHInstace();
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"MainWindow";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		name.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		m_application->GetHInstace(),
		nullptr);

	ShowWindow(m_hwnd, SW_SHOWDEFAULT);
}

bool MainWindow::Run()
{
	MSG msg = {};
	// Process any messages in the queue.
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

void MainWindow::Destroy()
{
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
		{
			if (m_application)
			{
				m_application->GetInput()->OnKeyDown(static_cast<UINT8>(wParam));
			}
		}
		return 0;
		case WM_KEYUP:
		{
			if (m_application)
			{
				m_application->GetInput()->OnKeyUp(static_cast<UINT8>(wParam));
			}
		}
		return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}
