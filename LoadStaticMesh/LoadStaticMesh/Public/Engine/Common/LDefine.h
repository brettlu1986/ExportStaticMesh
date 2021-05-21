#pragma once
#include "stdafx.h"

#define WINDOWS 

#if defined(WINDOWS) 
#define PLATFORM_WINDOWS
#elif defined(IPHONE)
#define PLATFORM_IOS
#elif defined(ANDROID)
#define PLATFORM_ANDROID
#else
#endif

//input def
typedef enum class E_DEVICE_PLATFORM :UINT8
{
	DEVICE_WINDOWS = 0,
	DEVICE_IOS,
	DEVICE_ANDROID,
}E_DEVICE_PLATFORM;

typedef enum class E_INPUT_TYPE : UINT8
{
	PC_KEYBORAD = 0, 
	PC_MOUSE_EVENT,
	INPUT_UNKNOWN,
}E_INPUT_TYPE;

typedef enum class E_TOUCH_TYPE : UINT8
{
	KEY_DOWN	= 0,
	KEY_UP,
	MOUSE_LEFT_DOWN,
	MOUSE_LEFT_MOVE,
	MOUSE_LEFT_UP,
	INPUT_UNKNOWN,
}E_TOUCH_TYPE;

typedef enum class E_CAMERA_TYPE : UINT8
{
	CAMERA_SCENE,
	CAMERA_THIRD_PERSON,
}E_CAMERA_TYPE;

typedef struct FInputResult
{
public:
	E_INPUT_TYPE InputType;
	E_TOUCH_TYPE TouchType;
	UINT8 KeyMapType;
	int X;
	int Y;

	FInputResult()
	:InputType(E_INPUT_TYPE::INPUT_UNKNOWN)
	, TouchType(E_TOUCH_TYPE::INPUT_UNKNOWN)
	, KeyMapType(0)
	,X(0)
	,Y(0)
	{
	}

	FInputResult(E_INPUT_TYPE InInput, E_TOUCH_TYPE InTouch, UINT8 InKey, int InX, int InY)
		:InputType(InInput)
		, TouchType(InTouch)
		, KeyMapType(InKey)
		, X(InX)
		, Y(InY)
	{
	}
	
}FInputResult;

static const float AnimFrameRate = 1 / 30.f;




