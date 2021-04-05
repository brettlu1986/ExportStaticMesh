#pragma once

#pragma once

#include "stdafx.h"
#include "FDefine.h"

class FD3D12Device;
class FRHIView;
class FD3DShaderResource;


class FD3D12ResourceManager
{
public:
	FD3D12ResourceManager(FD3D12Device* InParent);
	virtual ~FD3D12ResourceManager();
	void Initialize();
	void Clear();

	FRHIView* CreateShaderResourceView(std::wstring TextureName);

private:

	FD3D12Device* ParentDevice;


	std::vector<FRHIView*> ShaderResourceViews;
};