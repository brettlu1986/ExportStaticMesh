#pragma once

#pragma once

#include "stdafx.h"
#include "FDefine.h"

class FD3D12Device;
class FD3DRenderTarget;
class FD3DConstantBuffer;
class FD3DConstantBufferView;
class FD3DDepthStencilBuffer;
class FD3DDepthStencilView;
class FRHIView;
class FD3DShaderResource;

class FD3D12ResourceManager
{
public:
	FD3D12ResourceManager(FD3D12Device* InParent);
	virtual ~FD3D12ResourceManager();
	void Initialize();
	void Clear();

	void CreateRenderTarget(UINT TargetCount);
	void CreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize);
	void UpdateConstantBufferData(void* pUpdateData, UINT DataSize);
	void CreateDepthStencilBuffer(UINT Width, UINT Height);
	FRHIView* CreateShaderResourceView(std::wstring TextureName);

	FD3DRenderTarget* GetRenderTarget() const {
		return RenderTarget;
	}

	FD3DConstantBuffer* GetConstantBuffer() const 
	{
		return ConstantBuffer;
	}
private:
	void CreateConstantBufferView();
	void CreateDepthStencilView();

	FD3D12Device* ParentDevice;

	FD3DRenderTarget* RenderTarget;
	FD3DConstantBuffer* ConstantBuffer;
	FD3DConstantBufferView* ConstantBufferView;

	FD3DDepthStencilBuffer* DepthStencilBuffer;
	FD3DDepthStencilView* DepthStencilView;

	std::vector<FRHIView*> VertexBufferViews;
	std::vector<FRHIView*> IndexBufferViews;
	std::vector<FRHIView*> ShaderResourceViews;
};