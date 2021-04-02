
#include "ApplicationMain.h"
#include "LogicLoadModel.h"
#include "MainWindow.h"
#include "LInput.h"

ApplicationMain::ApplicationMain()
	:hMainInstance(nullptr)
	,Window(nullptr)
	,CurrentLogic(nullptr)
	,Input(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name)
{
	hMainInstance = hInstance; 

	Window = new MainWindow();
	Window->Initialize(this, Width, Height, Name);

	CurrentLogic = new LogicLoadModel();
	CurrentLogic->Initialize(this, Width, Height);

	Input = new LInput();
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
			CurrentLogic->Update();
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

	if (CurrentLogic)
	{
		CurrentLogic->Destroy();
		delete CurrentLogic;
		CurrentLogic = nullptr;
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
