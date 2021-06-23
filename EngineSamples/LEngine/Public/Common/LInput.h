#pragma once

#include "pch.h"
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

	virtual void GetCurrentCursorLocation(POINT& p) = 0;

	static inline bool IsKeyInput(LInputResult& Result)
	{
		return Result.InputType == E_INPUT_TYPE::PC_KEYBORAD;
	}

	static inline bool IsMouseInput(LInputResult& Result)
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