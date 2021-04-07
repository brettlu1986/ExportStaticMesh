
#include "ApplicationMain.h"
#include "LogicStaticModel.h"

#include "LEngine.h"
#include "LDeviceWindows.h"

ApplicationMain::ApplicationMain()
	:CurrentLogic(nullptr)
{
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(UINT Width, UINT Height, std::string WndName)
{
	LEngineDesc Desc = 
	{
		Width, 
		Height, 
		WndName,
	};
	LEngine::InitEngine(Desc);

	CurrentLogic = new LogicStaticModel();
	CurrentLogic->Initialize();

	return true;
}

void ApplicationMain::Run()
{
	while (LEngine::GetEngine()->Run())
	{
		CurrentLogic->Update();
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

