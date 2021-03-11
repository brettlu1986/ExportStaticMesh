#pragma once

#include "stdafx.h"

class DataSource;
class InputClass;
class GraphicRender;
class MainWindow;

class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name);
	void Run();
	void Destroy();

	HINSTANCE GetHInstace() { return hMainInstance; }
	GraphicRender* GetGraphicRender() { return CurrentGraphic; }
	InputClass* GetInput() { return Input; }
	MainWindow* GetWindow() { return Window; }
	DataSource* GetDataSource() { return CurrentDataSource; }

	HWND GetHwnd();
private:
	 
	HINSTANCE hMainInstance;
	MainWindow* Window;
	GraphicRender* CurrentGraphic;
	InputClass* Input;
	DataSource* CurrentDataSource;
};