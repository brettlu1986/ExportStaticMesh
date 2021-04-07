
#include "ApplicationMain.h"
#include "LogicStaticModel.h"
//#include "LInput.h"

#include "LEngine.h"
#include "LDeviceWindows.h"

ApplicationMain::ApplicationMain()
	:hMainInstance(nullptr)
	,CurrentLogic(nullptr)
	//,Input(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name, std::string WndName)
{
	hMainInstance = hInstance; 

	LEngineDesc Desc = 
	{
		Width, 
		Height, 
		WndName,
	};
	LEngine::InitEngine(Desc);

	CurrentLogic = new LogicStaticModel();
	CurrentLogic->Initialize(this, Width, Height);

	return true;
}

void ApplicationMain::Run()
{
	while (LEngine::GetEngine()->Run())
	{
		CurrentLogic->Update();
		CurrentLogic->Render();
	}
}

void ApplicationMain::Destroy()
{
	if (CurrentLogic)
	{
		CurrentLogic->Destroy();
		delete CurrentLogic;
		CurrentLogic = nullptr;
	}

	LEngine::GetEngine()->Destroy();
}

const HWND& ApplicationMain::GetHwnd()
{
	LDeviceWindows* Device = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	return Device->GetHwnd(); 
}
