
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
	DataScene = make_unique<LScene>();
}

ApplicationMain::~ApplicationMain()
{

}

bool ApplicationMain::Initialize(UINT Width, UINT Height, string WndName)
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
	SampleAssets::LoadSampleSceneData(*DataScene);
	//
	//SampleAssets::LoadSampleScene(&Scene);
	//FRenderThread::Get()->InitRenderThreadScene(&Scene);
}

void ApplicationMain::Update(float DeltaTime)
{
	//while(FRenderThread::Get()->ShouldWaitRender())
	//{
	//	continue;
	//}

	//Scene.Update(DeltaTime);
	DataScene->Update(DeltaTime);
	//FRenderThread::Get()->NotifyRenderThreadExcute();
	FRenderThread::Get()->WaitForRenderThread();
	//FRenderThread::Get()->UpdateRenderSceneResource(&Scene);
}

//void ApplicationMain::OnRender()
//{
//	//FRenderThread::Get()->OnRenderScene(&Scene);
//}

void ApplicationMain::Run()
{
	const UINT FrameRate = 60;
	chrono::duration<float> FrameLen = chrono::duration<float>(1.f/ FrameRate);
	while (LEngine::GetEngine()->Run())
	{
		Timer->Tick();
		Timer->Reset();

		Update(Timer->GetDeltaTime());
		//OnRender();

	//	LLog::Log("Real FrameRate:: %f", 1/ Timer->GetDeltaTime());
		this_thread::sleep_for(FrameLen - Timer->GetChronoDeltaTime());
	}
}

void ApplicationMain::Destroy()
{
	SampleAssets::ReleaseAssets();
	//FRenderThread::Get()->DestroyRenderScene(&Scene);
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
			DataScene->ActiveCamera(0);
			//Scene.ActiveCamera(0);//scene camera
			return;
		}
		else if(Key == 97)//num key board 1
		{
			DataScene->ActiveCamera(1);
			//Scene.ActiveCamera(1);
			return;
		}
	}


	if (DataScene->GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	{
		LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(DataScene->GetActiveCamera());
		Camera->ProcessCameraKeyInput(Input);
	}
	else if(DataScene->GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	{
		LCharacter* LocalPlayer = DataScene->GetLocalControlPlayer();
		LocalPlayer->ProcessKeyInput(Input);
	}

	//if(Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	//{
	//	LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(Scene.GetActiveCamera());
	//	Camera->ProcessCameraKeyInput(Input);
	//}
	//else if(Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	//{
	//	LCharacter* LocalPlayer = Scene.GetLocalControlPlayer();
	//	LocalPlayer->ProcessKeyInput(Input);
	//}
}

void ApplicationMain::ProcessMouseInput(FInputResult& Input)
{	
	if (DataScene->GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	{
		LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(DataScene->GetActiveCamera());
		Camera->ProcessCameraMouseInput(Input);
	}
	else if (DataScene->GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	{
		LCharacter* LocalPlayer = DataScene->GetLocalControlPlayer();
		LocalPlayer->ProcessMouseInput(Input);
	}
	/*if (Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_SCENE)
	{
		LSceneCamera* Camera = dynamic_cast<LSceneCamera*>(Scene.GetActiveCamera());
		Camera->ProcessCameraMouseInput(Input);
	}
	else if (Scene.GetActiveCamera()->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	{
		LCharacter* LocalPlayer = Scene.GetLocalControlPlayer();
		LocalPlayer->ProcessMouseInput(Input);
	}*/
}

