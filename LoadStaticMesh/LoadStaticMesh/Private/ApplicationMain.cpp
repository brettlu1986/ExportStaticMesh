
#include "stdafx.h"
#include "ApplicationMain.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LInput.h"
#include "LAssetDataLoader.h"
#include "FRenderThread.h"

ApplicationMain* ApplicationMain::Application = nullptr;

ApplicationMain::ApplicationMain()
{
	Application = this;
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(UINT Width, UINT Height, std::string WndName)
{
	Timer = LGameTimer::Get();
	LEngineDesc Desc = 
	{
		Width, 
		Height, 
		WndName,
	};
	//create reader thread in engine init
	LEngine::InitEngine(Desc);
	OnTouchInit();
	OnSceneInit();
	return true;
}

void ApplicationMain::OnTouchInit()
{
	EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
	EventDisp.RegisterEvent(new LEvent<FInputResult>(E_EVENT_KEY::EVENT_INPUT, &ApplicationMain::ProcessInput));
}

void ApplicationMain::OnSceneInit()
{
	LAssetDataLoader::LoadSampleScene(&Scene);
	FRenderThread::Get()->InitRenderThreadScene(&Scene);
}

void ApplicationMain::Update(float DeltaTime)
{
	//while(FRenderThread::Get()->ShouldWaitRender())
	//{
	//	continue;
	//}
	Scene.Update(DeltaTime);
	//FRenderThread::Get()->NotifyRenderThreadExcute();
	FRenderThread::Get()->WaitForRenderThread();
}

void ApplicationMain::Run()
{
	while (LEngine::GetEngine()->Run())
	{
		Timer->Tick();
		Timer->Reset();

		Update(Timer->GetDeltaTime());
	}
}

void ApplicationMain::Destroy()
{
	Timer->Release();
	LEngine::GetEngine()->Destroy();
}

void ApplicationMain::ProcessInput(FInputResult Input)
{
	if (LInput::IsMouseInput(Input))
	{
		ApplicationMain::Get()->ProcessMouseInput(Input);
	}
	else if(LInput::IsKeyInput(Input))
	{
		ApplicationMain::Get()->ProcessKeyInput(Input);
	}
}

void ApplicationMain::ProcessKeyInput(FInputResult& Input)
{
	//LCamera& Camera = Scene.GetCamera();
	//Camera.ProcessCameraKeyInput(Input);
}

void ApplicationMain::ProcessMouseInput(FInputResult& Input)
{
	//LCamera& Camera = Scene.GetCamera();
	//Camera.ProcessCameraMouseInput(Input);
}

