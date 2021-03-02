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

	bool Initialize(HINSTANCE hInstance, UINT width, UINT height, std::wstring name);
	void Run();
	void Destroy();

	HINSTANCE GetHInstace() { return m_hInstance; }
	GraphicRender* GetGraphicRender() { return m_graphics; }
	InputClass* GetInput() { return m_input; }
	MainWindow* GetWindow() { return m_window; }
	DataSource* GetDataSource() { return m_data_source; }

	HWND GetHwnd();
private:
	 
	HINSTANCE m_hInstance;
	MainWindow* m_window;
	GraphicRender* m_graphics;
	InputClass* m_input;
	DataSource* m_data_source;
};