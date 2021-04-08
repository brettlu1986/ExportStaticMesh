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
	virtual void InitializeTexture(const std::string& Name);

protected:
	uint8_t* BitData;
	size_t BitSize;
	std::string TextureName;
};