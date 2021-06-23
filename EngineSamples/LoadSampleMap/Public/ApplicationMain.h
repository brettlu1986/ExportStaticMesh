#pragma once

#include "stdafx.h"
#include "LEngine.h"
#include "Common/LDefine.h"
#include "Common/LGameTimer.h"
#include "Common/RenderData/LScene.h"


class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(UINT Width, UINT Height, string WndName);
	void Run();

	void Update(float DeltaTime);
	void Destroy();
	
	static ApplicationMain* Get() { return Application; }
	static void ProcessInput(LInputResult Input);
	void ProcessMouseInput(LInputResult& Input);
	void ProcessKeyInput(LInputResult& Input);

private:
	static ApplicationMain* Application;
	void OnTouchInit();
	void OnAssetsLoad();
	void OnSceneInit();
	void OnEngineInit(LEngineDesc Desc);

private: 
	LGameTimer* Timer;
	unique_ptr<LScene> DataScene;
};