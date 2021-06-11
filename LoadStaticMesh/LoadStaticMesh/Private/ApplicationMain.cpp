
#include "ApplicationMain.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LInput.h"
#include "FRenderThread.h"
#include <chrono>
#include <thread> 
//#include "SampleAssets.h"
#include "LCharacter.h"
#include "LLog.h"
#include "LSceneCamera.h"
#include "LAssetManager.h"
#include "LAssetDataLoader.h"

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
	LAssetManager::Get()->LoadAssets("AssetInfo/AssetsInfo.bin");

	vector<LMapStaticObjInfo> StaticObjs;
	vector<LMapSkeletalObjInfo> SkeletalObjs;
	LCameraData CameraData;
	vector<LDirectionLightData> LightsData;
	LAssetDataLoader::LoadMap("Maps/DefaultMap.bin", StaticObjs, SkeletalObjs, CameraData, LightsData);

	//load static meshes
	for(auto& StaticObj : StaticObjs)
	{
		auto Mesh = make_shared<LMesh>();

	}
	//SampleAssets::LoadSampleSceneData(*DataScene);
}

void ApplicationMain::Update(float DeltaTime)
{
	DataScene->Update(DeltaTime);
	FRenderThread::Get()->WaitForRenderThread();
}

void ApplicationMain::Run()
{
	const UINT FramePerSec = 60;
	chrono::duration<float> FrameRate = chrono::duration<float>(1.f / FramePerSec);
	while (LEngine::GetEngine()->Run())
	{
		Timer->Tick();
		Timer->Reset();

		Update(Timer->GetDeltaTime());
	   // LLog::Log("fps:: %f \n", Timer->GetDeltaTime());

		this_thread::sleep_for(FrameRate - Timer->GetChronoDeltaTime());
		//chrono::duration<float> FpsDelTime = Timer->GetCurrentTime() - Timer->GetStartTime();
		//LLog::Log("fps:: %f \n", 1 / FpsDelTime.count());
	}
}

void ApplicationMain::Destroy()
{
	DataScene = nullptr;
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
			return;
		}
		else if(Key == 97)//num key board 1
		{
			DataScene->ActiveCamera(1);
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
}

