#pragma once

#pragma once

#include "stdafx.h"
#include "FDefine.h"

class FD3D12Device;
class FD3DConstantBuffer;
class FD3DConstantBufferView;
class FRHIView;
class FD3DShaderResource;

class FD3D12ResourceManager
{
public:
	FD3D12ResourceManager(FD3D12Device* InParent);
	virtual ~FD3D12ResourceManager();
	void Initialize();
	void Clear();

	void CreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize);
	void UpdateConstantBufferData(void* pUpdateData, UINT DataSize);
	FRHIView* CreateShaderResourceView(std::wstring TextureName);


	FD3DConstantBuffer* GetConstantBuffer() const 
	{
		return ConstantBuffer;
	}
private:
	void CreateConstantBufferView();

	FD3D12Device* ParentDevice;

	FD3DConstantBuffer* ConstantBuffer;
	FD3DConstantBufferView* ConstantBufferView;

	std::vector<FRHIView*> ShaderResourceViews;
};