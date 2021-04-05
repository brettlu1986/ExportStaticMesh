#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "FRenderResource.h"

class FTexture : public FRenderResource 
{
public:
	FTexture();
	virtual ~FTexture();

	virtual void Destroy() override;
	virtual void Initialize() override;

	std::wstring& GetTextureName()
	{
		return TextureName;
	}

	void SetTextureName(const std::wstring& Name)
	{
		TextureName = Name;
	}
private:
	std::wstring TextureName;
};