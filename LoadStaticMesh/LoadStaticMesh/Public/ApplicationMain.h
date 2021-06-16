#pragma once

#include "stdafx.h"
#include "LDefine.h"
#include "LGameTimer.h"
#include "LScene.h"
#include "LEngine.h"

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
	static void ProcessInput(FInputResult Input);
	void ProcessMouseInput(FInputResult& Input);
	void ProcessKeyInput(FInputResult& Input);

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