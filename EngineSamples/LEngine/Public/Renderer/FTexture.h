#pragma once

#include "pch.h"
#include "FDefine.h"
#include "FRenderResource.h"

#include "LTexture.h"

class FTexture : public FRenderResource 
{
public:
	FTexture();
	FTexture(E_FRESOURCE_TYPE Type);
	virtual ~FTexture();

	virtual void InitializeTexture(FTextureInitializer Initializer) = 0;

	virtual void Destroy() override;
	virtual void Initialize() override;
	void InitializeTexture(LTexture* TexData)
	{
		TextureData = TexData;
	}

	E_RESOURCE_STATE GetInitState()
	{
		return InitResourceState;
	}

protected:
	LTexture* TextureData;

	E_RESOURCE_STATE InitResourceState;
};