#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "FRenderResource.h"

class FTexture : public FRenderResource 
{
public:
	FTexture();
	FTexture(E_FRESOURCE_TYPE Type);
	virtual ~FTexture();

	virtual void InitializeTexture(FTextureInitializer Initializer) = 0;

	virtual void Destroy() override;
	virtual void Initialize() override;
	virtual void InitializeTexture(const string& Name) = 0;

	const XMFLOAT4X4& GetTextureTransform() const 
	{
		return TexTransform;
	}
protected:
	uint8_t* BitData;
	size_t BitSize;
	string TextureName;
	XMFLOAT4X4 TexTransform;
};