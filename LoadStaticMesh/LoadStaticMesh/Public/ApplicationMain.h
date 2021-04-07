#pragma once

#include "stdafx.h"

class LInput;
class LogicStaticModel;

class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(HINSTANCE hInstance, UINT Width, UINT Height, std::wstring Name, std::string WndName);
	void Run();
	void Destroy();

	const HINSTANCE& GetHInstace() const { return hMainInstance; }

	const HWND& GetHwnd();
private:
	 
	HINSTANCE hMainInstance;
	LogicStaticModel* CurrentLogic;
};