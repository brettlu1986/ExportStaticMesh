#pragma once
#include "stdafx.h"

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


static UINT CalcConstantBufferByteSize(UINT byteSize)
{
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

