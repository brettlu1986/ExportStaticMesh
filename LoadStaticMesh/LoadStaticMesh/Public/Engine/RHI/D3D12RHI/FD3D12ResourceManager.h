#pragma once

#pragma once

#include "stdafx.h"

class FD3D12Device;
class D3DRenderTarget;
class D3DConstantBuffer;
class D3DConstantBufferView;
class D3DDepthStencilBuffer;
class D3DDepthStencilView;
class FRHIView;
class D3DShaderResource;

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
	FRHIView* CreateVertexBufferView(const void* InitData, UINT StrideInByte, UINT DataSize);
	FRHIView* CreateIndexBufferView(const void* InitData, UINT DataSize, UINT IndicesCount, bool bUseHalfInt32);
	FRHIView* CreateShaderResourceView(std::wstring TextureName);

	D3DRenderTarget* GetRenderTarget() const {
		return RenderTarget;
	}

	D3DConstantBuffer* GetConstantBuffer() const 
	{
		return ConstantBuffer;
	}
private:
	void CreateConstantBufferView();
	void CreateDepthStencilView();

	FD3D12Device* ParentDevice;

	D3DRenderTarget* RenderTarget;
	D3DConstantBuffer* ConstantBuffer;
	D3DConstantBufferView* ConstantBufferView;

	D3DDepthStencilBuffer* DepthStencilBuffer;
	D3DDepthStencilView* DepthStencilView;

	std::vector<FRHIView*> VertexBufferViews;
	std::vector<FRHIView*> IndexBufferViews;
	std::vector<FRHIView*> ShaderResourceViews;
};