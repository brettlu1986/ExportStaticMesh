#pragma once

#include "stdafx.h"

class LogicStaticModel;

class ApplicationMain
{
public:
	ApplicationMain();
	~ApplicationMain();

	bool Initialize(UINT Width, UINT Height, std::string WndName);
	void Run();
	void Destroy();

private:
	 
	LogicStaticModel* CurrentLogic;
};