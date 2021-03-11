
#include "ApplicationMain.h"
#include "GraphicRender_ClearWindow.h"
#include "GraphicRender_LoadModel.h"
#include "MainWindow.h"
#include "InputClass.h"
#include "DataSource.h"

ApplicationMain::ApplicationMain()
	:hMainInstance(nullptr)
	,Window(nullptr)
	,CurrentGraphic(nullptr)
	,Input(nullptr)
	,CurrentDataSource(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name)
{
	hMainInstance = hInstance; 

	CurrentDataSource = new DataSource();
	CurrentDataSource->Initialize(this);

	Window = new MainWindow();
	Window->Initialize(this, Width, Height, Name);

	CurrentGraphic = new GraphicRender_LoadModel();//GraphicRender_ClearWindow();
	CurrentGraphic->Initialize(this, Width, Height);

	Input = new InputClass();
	Input->Initialize(this);
	
	return true;
}


void ApplicationMain::Run()
{
	if (Window)
	{
		while (Window->Run())
		{
			Input->Update();
			CurrentGraphic->Update();
			CurrentGraphic->Render();
		}
	}
}

void ApplicationMain::Destroy()
{
	if (Window)
	{
		Window->Destroy();
		delete Window;
		Window = nullptr;
	}

	if (CurrentGraphic)
	{
		CurrentGraphic->Destroy();
		delete CurrentGraphic;
		CurrentGraphic = nullptr;
	}

	if (Input)
	{
		Input->Destroy();
		delete Input;
		Input = nullptr;
	}
}

const HWND& ApplicationMain::GetHwnd()
{
	return Window->GetHwnd();
}
