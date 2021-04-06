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

typedef enum class E_KEY_MAP : UINT
{
	W = 0,
	A,
	S,
	D,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	INPUT_UNKNOWN,
}E_KEY_MAP;

typedef struct FInputResult
{
public:
	E_INPUT_TYPE InputType;
	E_TOUCH_TYPE TouchType;
	E_KEY_MAP KeyMapType;
	int X;
	int Y;

	FInputResult()
	:InputType(E_INPUT_TYPE::INPUT_UNKNOWN)
	, TouchType(E_TOUCH_TYPE::INPUT_UNKNOWN)
	, KeyMapType(E_KEY_MAP::INPUT_UNKNOWN)
	,X(0)
	,Y(0)
	{
	}
	
}FInputResult;

