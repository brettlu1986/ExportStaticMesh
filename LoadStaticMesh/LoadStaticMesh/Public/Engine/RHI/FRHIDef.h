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

class FRHICommandList
{
public:
	FRHICommandList() { CommandListIndex = 0;};
	virtual ~FRHICommandList() {};

	virtual void Clear() = 0;
	virtual void Close() = 0;
	virtual void Reset() = 0;
	virtual void Excute() = 0;
	virtual void SetViewPort() = 0;
	virtual void SetScissorRect() = 0;
	virtual void TransitionToState(UINT TargetFrame, ETransitionState State) = 0;
	virtual void ClearRenderTargetAndDepthStencilView(UINT TargetFrame, FRHIColor Color) = 0;

	virtual void SetGraphicRootDescripterTable() = 0;

protected:
	UINT CommandListIndex;
};

