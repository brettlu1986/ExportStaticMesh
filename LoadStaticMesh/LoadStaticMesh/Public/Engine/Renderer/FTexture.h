#pragma once

#include "stdafx.h"
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

	const XMFLOAT4X4& GetTextureTransform() const 
	{
		return TexTransform;
	}

protected:
	XMFLOAT4X4 TexTransform;

	LTexture* TextureData;
};