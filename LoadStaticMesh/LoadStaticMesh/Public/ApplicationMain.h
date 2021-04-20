#pragma once

#include "stdafx.h"
#include "LDefine.h"
#include "FScene.h"

class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(UINT Width, UINT Height, std::string WndName);
	void Run();

	void Update();
	void Destroy();
	
	static ApplicationMain* Get() { return Application; }
	static void ProcessInput(FInputResult Input);
	void ProcessMouseInput(FInputResult& Input);
	void ProcessKeyInput(FInputResult& Input);

private:
	static ApplicationMain* Application;
	void OnTouchInit();
	void OnSceneInit();

private: 
	FScene Scene;
};