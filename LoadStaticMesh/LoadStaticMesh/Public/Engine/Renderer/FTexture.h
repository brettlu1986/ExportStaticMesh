#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "FRenderResource.h"

class FTexture : public FRenderResource 
{
public:
	FTexture();
	FTexture(E_RESOURCE_TYPE Type);
	virtual ~FTexture();

	virtual void Destroy() override;
	virtual void Initialize() override;

	std::string& GetTextureName()
	{
		return TextureName;
	}

	void SetTextureName(const std::string& Name)
	{
		TextureName = Name;
	}
private:
	std::string TextureName;
};