
#include "ApplicationMain.h"
#include "LogicStaticModel.h"
//#include "MainWindow.h"
#include "LInput.h"

#include "LEngine.h"
#include "LDeviceWindows.h"

ApplicationMain::ApplicationMain()
	:hMainInstance(nullptr)
	//,Window(nullptr)
	,CurrentLogic(nullptr)
	,Input(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name, std::string WndName)
{
	hMainInstance = hInstance; 

	//Window = new MainWindow();
	//Window->Initialize(this, Width, Height, Name);

	LEngineDesc Desc = 
	{
		Width, 
		Height, 
		WndName,
	};
	LEngine::InitEngine(Desc);

	CurrentLogic = new LogicStaticModel();
	CurrentLogic->Initialize(this, Width, Height);

	Input = new LInput();
	Input->Initialize();
	
	return true;
}

void ApplicationMain::Run()
{
	//if (Window)
	//{
		while (LEngine::GetEngine()->Run())
		{
			Input->Update();
			CurrentLogic->Update();
			CurrentLogic->Render();
		}
	//}
}

void ApplicationMain::Destroy()
{
	/*if (Window)
	{
		Window->Destroy();
		delete Window;
		Window = nullptr;
	}*/

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

	LEngine::GetEngine()->Destroy();
}

const HWND& ApplicationMain::GetHwnd()
{
	LDeviceWindows* Device = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	return Device->GetHwnd();
}
