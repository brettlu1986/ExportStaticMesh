#pragma once

#pragma once

#include "stdafx.h"

class D3D12Device;
class D3DRenderTarget;
class D3DConstantBuffer;
class D3DConstantBufferView;
class D3DDepthStencilBuffer;
class D3DDepthStencilView;
class RHIView;
class D3DShaderResource;

class D3D12ResourceManager
{
public:
	D3D12ResourceManager(D3D12Device* InParent);
	virtual ~D3D12ResourceManager();
	void Initialize();
	void Clear();

	void CreateRenderTarget(UINT TargetCount);
	void CreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize);
	void UpdateConstantBufferData(void* pUpdateData, UINT DataSize);
	void CreateDepthStencilBuffer(UINT Width, UINT Height);
	RHIView* CreateVertexBufferView(const void* InitData, UINT StrideInByte, UINT DataSize);
	RHIView* CreateIndexBufferView(const void* InitData, UINT DataSize, UINT IndicesCount, bool bUseHalfInt32);
	RHIView* CreateShaderResourceView(std::wstring TextureName);

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

	D3D12Device* ParentDevice;

	D3DRenderTarget* RenderTarget;
	D3DConstantBuffer* ConstantBuffer;
	D3DConstantBufferView* ConstantBufferView;

	D3DDepthStencilBuffer* DepthStencilBuffer;
	D3DDepthStencilView* DepthStencilView;

	std::vector<RHIView*> VertexBufferViews;
	std::vector<RHIView*> IndexBufferViews;
	std::vector<RHIView*> ShaderResourceViews;
};