#pragma once

#pragma once

#include "stdafx.h"

class D3D12Device;
class D3DRenderTarget;
class D3DConstantBuffer;
class D3D12ResourceManager
{
public:
	D3D12ResourceManager(D3D12Device* InParent);
	virtual ~D3D12ResourceManager();
	void Initialize();
	void Clear();

	void CreateRenderTarget(UINT TargetCount);
	void CreateConstantBuffer(UINT BufferSize, void* pDataFrom, void** ppDataMap);

	D3DRenderTarget* GetRenderTarget() const {
		return RenderTarget;
	}
private:
	D3D12Device* ParentDevice;

	D3DRenderTarget* RenderTarget;
	D3DConstantBuffer* ConstantBuffer;
};