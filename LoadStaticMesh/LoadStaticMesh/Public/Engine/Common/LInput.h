#pragma once

#include "stdafx.h"
#include "LDefine.h"

class LInput
{
public: 
	LInput();
	virtual~LInput();

	virtual void Initialize();
	virtual bool Update();

	static void CreateInput();
	static void Destroy();

	bool IsKeyInput(FInputResult& Result)
	{
		return Result.InputType == E_INPUT_TYPE::PC_KEYBORAD;
	}

	bool IsMouseInput(FInputResult& Result)
	{
		return Result.InputType == E_INPUT_TYPE::PC_MOUSE_EVENT;
	}

	static LInput* GetInput()
	{
		return MainInput;
	}

private:
	static LInput* MainInput;

};