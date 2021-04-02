#pragma once

#include "stdafx.h"

class LInput;
class LogicStaticModel;
class MainWindow;

class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name);
	void Run();
	void Destroy();

	const HINSTANCE& GetHInstace() const { return hMainInstance; }
	LogicStaticModel* GetGraphicRender() const { return CurrentLogic; }
	LInput* GetInput() const { return Input; }
	MainWindow* GetWindow() const { return Window; }

	const HWND& GetHwnd();
private:
	 
	HINSTANCE hMainInstance;
	MainWindow* Window;
	LogicStaticModel* CurrentLogic;
	LInput* Input;
};