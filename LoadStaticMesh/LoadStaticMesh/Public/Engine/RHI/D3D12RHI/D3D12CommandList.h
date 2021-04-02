#pragma once
#include "stdafx.h"
#include "RHIDef.h"

using namespace Microsoft::WRL;

class D3D12Device;
class D3D12CommandListAllocator;
class D3D12CommandList : public RHICommandList
{
public:
	D3D12CommandList(D3D12Device* ParentDevice, D3D12CommandListAllocator* CommandAllocator,UINT InCommandListIndex);

	~D3D12CommandList();
	ID3D12GraphicsCommandList* GetD3DCommandList() 
	{
		return CommandList.Get();
	}

	virtual void Clear() override;
	virtual void Close() override;
	virtual void Reset() override;
	virtual void Excute() override;
	virtual void SetViewPort() override;
	virtual void SetScissorRect() override;
	virtual void TransitionToState(UINT TargetFrame, ETransitionState State) override;
	virtual void ClearRenderTargetAndDepthStencilView(UINT TargetFrame, RHIColor Color) override;
	virtual void SetGraphicRootDescripterTable() override;
	virtual void DrawWithVertexAndIndexBufferView(RHIView* VertexBufferView, RHIView* IndexBufferView) override;

private: 
	
	D3D12Device* ParentDevice;
	D3D12CommandListAllocator* CurrentCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
};
