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

typedef enum class E_ANIM_STATE : UINT8
{
	IDLE = 0,
	WALK,
	RUN,
	JUMP_START,
	JUMP_LOOP,
	JUMP_END,
	STATE_COUNT,
}E_ANIM_STATE;


static const float AnimFrameRate = 1 / 30.f;

//LResourceDef 
enum class E_LRESOURCE_TYPE : UINT8
{
	L_TYPE_UNKNOWN = 0,
	L_TYPE_INDEX_BUFFER,
	L_TYPE_VERTEX_BUFFER,
	L_TYPE_MESH,
	L_TYPE_LIGHT,
	L_TYPE_TEXTURE,
	L_TYPE_SHADER,
	L_TYPE_MATERIAL,
	L_TYPE_PIPLINE,
};

enum class E_INDEX_TYPE : UINT8
{
	TYPE_UINT_16 = 0,
	TYPE_UINT_32,
};




