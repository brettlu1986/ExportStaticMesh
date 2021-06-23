
#include "stdafx.h"
#include "ApplicationMain.h"

#include "Common/Event/LEvent.h"
#include "Common/LInput.h"
#include "Thread/FRenderThread.h"
#include <chrono>
#include <thread> 
#include "Common/Character/LCharacter.h"
#include "Common/LLog.h"
#include "Common/Camera/LSceneCamera.h"
#include "Common/LAssetManager.h"
#include "Common/LAssetDataLoader.h"
#include "Common/RenderData/LLight.h"
#include "Common/Character/LPlayerController.h"
#include "Common/Camera/LThirdPersonCamera.h"
#include "Common/RenderData/LScreenMesh.h"

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

	OnAssetsLoad();
	OnEngineInit(Desc);
	OnTouchInit();
	OnSceneInit();
	return true;
}

void ApplicationMain::OnAssetsLoad()
{
	//load assets
	LAssetManager::Get()->LoadAssets("AssetInfo/AssetsInfo.bin");
}

void ApplicationMain::OnEngineInit(LEngineDesc Desc)
{
	//create reader thread in engine init
	LEngine::InitEngine(Desc);
}

void ApplicationMain::OnTouchInit()
{
	EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
	EventDisp.RegisterEvent(new LEvent<LInputResult>(E_EVENT_KEY::EVENT_INPUT, &ApplicationMain::ProcessInput));
}

void ApplicationMain::OnSceneInit()
{
	LAssetManager* AssetMgr = LAssetManager::Get();
	vector<LMapStaticObjInfo> StaticObjs;
	vector<LMapSkeletalObjInfo> SkeletalObjs;
	LCameraData CameraData;
	vector<LDirectionLightData> LightsData;
	LAssetDataLoader::LoadMap("Maps/DefaultMap.bin", StaticObjs, SkeletalObjs, CameraData, LightsData);

	auto ScreenMesh = make_shared<LScreenMesh>();
	ScreenMesh->InitRenderThreadResource();
	DataScene->SetScreenMesh(ScreenMesh);
	//load static meshes
	for(auto& StaticObj : StaticObjs)
	{
		auto Mesh = make_shared<LMesh>();
		Mesh->SetName(StaticObj.ObjectName);
		Mesh->SetMaterial(AssetMgr->GetMaterial(StaticObj.RefMaterial));
		Mesh->SetMeshBufferInfo(AssetMgr->GetMeshBuffer(StaticObj.RefGeometry));
		Mesh->InitRenderThreadResource();

		Mesh->SetModelLocation(StaticObj.WorldLocation);
		Mesh->SetModelRotation(StaticObj.WorldRotator);
		Mesh->SetModelScale(StaticObj.WorldScale);

		DataScene->AddStaticMeshes(Mesh);
	}

	for(auto& SkeletalObj : SkeletalObjs)
	{
		auto Character = make_shared<LCharacter>();
		Character->SetName(SkeletalObj.ObjectName);
		LSkeletalMesh* SkeletalMesh = new LSkeletalMesh();
		SkeletalMesh->SetName(SkeletalObj.ObjectName);
		SkeletalMesh->SetMaterial(AssetMgr->GetMaterial(SkeletalObj.RefMaterial));
		SkeletalMesh->SetSkeletalMeshBuffer(AssetMgr->GetSkeletalMeshBuffer(SkeletalObj.RefGeometry));

		SkeletalMesh->InitRenderThreadResource();
		SkeletalMesh->SetModelLocation(SkeletalObj.WorldLocation);
		SkeletalMesh->SetModelRotation(SkeletalObj.WorldRotator);
		SkeletalMesh->SetModelScale(SkeletalObj.WorldScale);

		SkeletalMesh->SetSkeleton(AssetMgr->GetSkeletal(SkeletalObj.RefSkeleton));

		LAnimator* Animator = new LAnimator();
		for(size_t i = 0; i < SkeletalObj.RefAnims.size(); i++)
		{
			LAnimationSequence* Seq = AssetMgr->GetAnimationSeq(SkeletalObj.RefAnims[i]);
			Animator->AddAnimSequence(static_cast<E_ANIM_STATE>(i), *Seq);
		}

		Character->SetSkeletalMesh(SkeletalMesh);
		Character->SetAnimator(Animator);

		DataScene->AddCharacterToScene(Character);
	}

	//load lights
	for (size_t i = 0; i < LightsData.size(); i++)
	{
		auto Light = make_shared<LLight>();
		Light->Direction = LightsData[i].Direction;
		Light->Position = LightsData[i].Position;
		Light->Strength = LightsData[i].Strength;
		Light->LightIndex = (UINT)i;
		//only change the first light param
		Light->Init();
		Light->InitRenderThreadResource();
		DataScene->AddLightToScene(Light);
	}
	
	//load scene camera
	shared_ptr<LCamera> SceneCamera = make_shared<LSceneCamera>();
	SceneCamera->Init(CameraData);
	DataScene->AddCamera(SceneCamera);

	shared_ptr<LCamera> ThirdPersonCamera = make_shared<LThirdPersonCamera>();
	ThirdPersonCamera->Init(CameraData);
	ThirdPersonCamera->SetSocketOffset(XMFLOAT3(-2.f, 0.3f, 2.2f));
	ThirdPersonCamera->SetViewTarget(DataScene->GetCharacters()[2].get());
	DataScene->AddCamera(ThirdPersonCamera);

	//set the first character is the player
	LPlayerController* PlayerController = new LPlayerController();
	PlayerController->Possess(DataScene->GetCharacters()[2].get());

	DataScene->ActiveCamera(0);
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

void ApplicationMain::ProcessInput(LInputResult Input)
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

void ApplicationMain::ProcessKeyInput(LInputResult& Input)
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

void ApplicationMain::ProcessMouseInput(LInputResult& Input)
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

