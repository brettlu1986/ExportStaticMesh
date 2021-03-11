#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"


class MainWindow
{

public: 
	MainWindow();
	~MainWindow();

	void Initialize(ApplicationMain* Application, UINT Width, UINT Height, std::wstring Name);
	bool Run();
	void Destroy();

	HWND GetHwnd() { return hMainWnd; }

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	HWND hMainWnd;
	static ApplicationMain* MainApplication;
};