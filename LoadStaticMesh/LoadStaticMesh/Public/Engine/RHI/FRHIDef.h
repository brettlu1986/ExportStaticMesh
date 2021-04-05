#pragma once
#include "stdafx.h"
#include "FRHIResource.h"

typedef enum class EDynamicModuleType : UINT8
{
	MODULE_D3D12 = 0,
	MODULE_D3D11,
	MODULE_ANDROID, 
} EDynamicModuleType ;

typedef enum class ETransitionState : UINT8
{
	STATE_RENDER = 0,
	STATE_PRESENT,
} ETransitionState;

struct FRHIColor
{
	float R;
	float G;
	float B;
	float A;
};


