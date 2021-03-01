
#include "ApplicationMain.h"
#include "MainWindow.h"
#include "GraphicRender_ClearWindow.h"
#include "InputClass.h"

ApplicationMain::ApplicationMain()
	:m_window(nullptr)
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

	m_graphics = new GraphicRender_ClearWindow();
	m_graphics->Initialize(this, width, height);

	m_input = new InputClass();
	m_input->Initialize(this);
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