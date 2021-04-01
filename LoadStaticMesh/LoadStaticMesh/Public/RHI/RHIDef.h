#pragma once
#include "stdafx.h"
#include "RHIResource.h"

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

struct RHIColor
{
	float R;
	float G;
	float B;
	float A;
};

class RHICommandList
{
public:
	RHICommandList() { CommandListIndex = 0;};
	virtual ~RHICommandList() {};

	virtual void Clear() = 0;
	virtual void Close() = 0;
	virtual void Reset() = 0;
	virtual void Excute() = 0;
	virtual void SetViewPort() = 0;
	virtual void SetScissorRect() = 0;
	virtual void TransitionToState(UINT TargetFrame, ETransitionState State) = 0;
	virtual void ClearRenderTargetAndDepthStencilView(UINT TargetFrame, RHIColor Color) = 0;

	virtual void SetGraphicRootDescripterTable() = 0;
	virtual void DrawWithVertexAndIndexBufferView(RHIView* VertexBufferView, RHIView* IndexBufferView) = 0;

protected:
	UINT CommandListIndex;
};

