#pragma once

#include "stdafx.h"

class ApplicationMain;

class InputClass
{
public: 
	InputClass();
	~InputClass();

	void Initialize(ApplicationMain* application);
	bool Update();
	void Destroy();

	void OnKeyDown(UINT8 /*key*/) {}
	void OnKeyUp(UINT8 /*key*/) {}

	

private:

	ApplicationMain* MainApplication;
};