#pragma once

#include "stdafx.h"

class ApplicationMain;

class LInput
{
public: 
	LInput();
	~LInput();

	void Initialize();
	bool Update();
	void Destroy();

	void OnKeyDown(UINT8 /*key*/) {}
	void OnKeyUp(UINT8 /*key*/) {}

};