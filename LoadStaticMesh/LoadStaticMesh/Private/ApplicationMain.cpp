
#include "stdafx.h"
#include "ApplicationMain.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LInput.h"
//#include "LAssetDataLoader.h"
#include "FRenderThread.h"
#include <chrono>
#include <thread> 
#include "SampleAssets.h"

#include "LLog.h"

#include "LSceneCamera.h"

ApplicationMain* ApplicationMain::Application = nullptr;

ApplicationMain::ApplicationMain()
:Timer(nullptr)
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
	SampleAssets::LoadSampleScene(&Scene);
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
	FRenderThread::Get()->UpdateRenderSceneResource(&Scene);
}

void ApplicationMain::OnRender()
{
	FRenderThread::Get()->OnRenderScene(&Scene);
}

void ApplicationMain::Run()
{
	const UINT FrameRate = 60;
	std::chrono::duration<float> FrameLen = std::chrono::duration<float>(1.f/60);
	while (LEngine::GetEngine()->Run())
	{
		Timer->Tick();
		Timer->Reset();

		Update(Timer->GetDeltaTime());
		OnRender();
		this_thread::sleep_for(FrameLen - Timer->GetChronoDeltaTime());
	}
}

void ApplicationMain::Destroy()
{
	SampleAssets::ReleaseAssets();
	FRenderThread::Get()->DestroyRenderScene(&Scene);
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

	if (Input.TouchType == E_TOUCH_TYPE::KEY_DOWN)
	{
		UINT8 Key = static_cast<UINT8>(Input.KeyMapType);
		if(Key == 96) //num key board 0
		{ 
			Scene.ActiveCamera(0);//scene camera
			return;
		}
		else if(Key == 97)//num key board 1
		{
			Scene.ActiveCamera(1);
			return;
		}
	}

	if(Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	{
		LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(Scene.GetActiveCamera());
		Camera->ProcessCameraKeyInput(Input);
	}
	else if(Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	{
		LCharacter* LocalPlayer = Scene.GetLocalControlPlayer();
		LocalPlayer->ProcessKeyInput(Input);
	}
}

void ApplicationMain::ProcessMouseInput(FInputResult& Input)
{

	if (Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	{
		LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(Scene.GetActiveCamera());
		Camera->ProcessCameraMouseInput(Input);
	}
	else if (Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	{
		LCharacter* LocalPlayer = Scene.GetLocalControlPlayer();
		LocalPlayer->ProcessMouseInput(Input);
	}
}

