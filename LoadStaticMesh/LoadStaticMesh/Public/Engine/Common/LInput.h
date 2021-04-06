#pragma once

#include "stdafx.h"
#include "LDefine.h"

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

	static LRESULT CALLBACK MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam);

	bool IsKeyInput()
	{
		return Result.InputType == E_INPUT_TYPE::PC_KEYBORAD;
	}

	bool IsMouseInput()
	{
		return Result.InputType == E_INPUT_TYPE::PC_MOUSE_EVENT;
	}

	static LInput* GetInput()
	{
		return MainInput;
	}

	const FInputResult& GetInputResult()
	{
		return Result;
	}
private:
	static LInput* MainInput;

	void UpdateInputResult(E_INPUT_TYPE InputType, E_TOUCH_TYPE TouchType, E_KEY_MAP KeyMapType, int X = 0, int Y = 0)
	{
		Result.InputType = InputType;
		Result.TouchType = TouchType;
		Result.KeyMapType = KeyMapType;
		Result.X = X;
		Result.Y = Y;
	}

	FInputResult Result;
};