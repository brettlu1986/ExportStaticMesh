
#include "ApplicationMain.h"
#include "LEngine.h"
#include "LEvent.h"
#include "LInput.h"
#include "LAssetDataLoader.h"
#include <DirectXMath.h>
#include "FRenderThread.h"

#include <dxgidebug.h>
#include <d3dcompiler.h>

using namespace DirectX;

ApplicationMain* ApplicationMain::Application = nullptr;

ApplicationMain::ApplicationMain()
	:LastMousePoint({0, 0})
{
	Application = this;
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

void ApplicationMain::Update()
{
	while(FRenderThread::Get()->ShouldWaitRender())
	{
		continue;
	}
	Scene.Update();
	FRenderThread::Get()->NotifyRenderThreadExcute();
	//LEngine::GetEngine()->WaitForRenderThread();
	//LEngine::GetEngine()->UpdateRenderThreadScene(&Scene);
}

void ApplicationMain::Render()
{	
	//LEngine::GetEngine()->DrawThreadThreadScene(&Scene);
}

void ApplicationMain::Run()
{
	while (LEngine::GetEngine()->Run())
	{
		Update();
		Render();
	}
}

void ApplicationMain::Destroy()
{
	LEngine::GetEngine()->Destroy();
}

void ApplicationMain::ProcessInput(FInputResult Input)
{
	if (LInput::IsMouseInput(Input))
	{
		ApplicationMain::Get()->ProcessMouseInput(Input);
	}
}

void ApplicationMain::ProcessMouseInput(FInputResult& Input)
{
	if (Input.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_DOWN)
	{
		LastMousePoint.x = Input.X;
		LastMousePoint.y = Input.Y;
	}
	else if (Input.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(Input.X - LastMousePoint.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(Input.Y - LastMousePoint.y));

		Scene.UpdateCameraMatrix(dx, dy);
		LastMousePoint.x = Input.X;
		LastMousePoint.y = Input.Y;
	}
}

