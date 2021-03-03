#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"


class MainWindow
{

public: 
	MainWindow();
	~MainWindow();

	void Initialize(ApplicationMain* application, UINT width, UINT height, std::wstring name);
	bool Run();
	void Destroy();

	HWND GetHwnd() { return m_hwnd; }

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;
	std::wstring m_title;
	static ApplicationMain* m_application;
};