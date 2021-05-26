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

	virtual void InitializeTexture(FTextureInitializer Initializer) = 0;

	virtual void Destroy() override;
	virtual void Initialize() override;
	virtual void InitializeTexture(const std::string& Name) = 0;

	void SetTextureHeapIndex(UINT Index) { TexHeapIndex = Index; }
	UINT GetTextureHeapIndex() const { return TexHeapIndex; }

	const XMFLOAT4X4& GetTextureTransform() const 
	{
		return TexTransform;
	}
protected:
	uint8_t* BitData;
	size_t BitSize;
	std::string TextureName;
	//this index is the offset from first tex in heap
	UINT TexHeapIndex;

	XMFLOAT4X4 TexTransform;
};