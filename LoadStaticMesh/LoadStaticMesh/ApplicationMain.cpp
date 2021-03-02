
#include "ApplicationMain.h"
#include "GraphicRender_ClearWindow.h"
#include "GraphicRender_LoadModel.h"
#include "MainWindow.h"
#include "InputClass.h"
#include "DataSource.h"

ApplicationMain::ApplicationMain()
	:m_hInstance(nullptr)
	,m_window(nullptr)
	,m_graphics(nullptr)
	,m_input(nullptr)
	,m_data_source(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(HINSTANCE hInstance, UINT width, UINT height, std::wstring name)
{
	m_hInstance = hInstance; 

	m_window = new MainWindow();
	m_window->Initialize(this, width, height, name);

	m_graphics = new GraphicRender_ClearWindow();//GraphicRender_ClearWindow();
	m_graphics->Initialize(this, width, height);

	m_input = new InputClass();
	m_input->Initialize(this);

	m_data_source = new DataSource();
	m_data_source->Initialize(this);
	return true;
}


void ApplicationMain::Run()
{
	if (m_window)
	{
		while (m_window->Run())
		{
			m_input->Update();
			m_graphics->Render();
		}
	}
}

void ApplicationMain::Destroy()
{
	if (m_window)
	{
		m_window->Destroy();
		delete m_window;
		m_window = nullptr;
	}

	if (m_graphics)
	{
		m_graphics->Destroy();
		delete m_graphics;
		m_graphics = nullptr;
	}

	if (m_input)
	{
		m_input->Destroy();
		delete m_input;
		m_input = nullptr;
	}
}

HWND ApplicationMain::GetHwnd()
{
	return m_window->GetHwnd();
}
