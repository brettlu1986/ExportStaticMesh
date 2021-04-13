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

	void SetTextureHeapIndex(UINT Index) { TexHeapIndex = Index; }
	UINT GetTextureHeapIndex() const { return TexHeapIndex; }

protected:
	uint8_t* BitData;
	size_t BitSize;
	std::string TextureName;
	//this index is the offset from first tex in heap
	UINT TexHeapIndex;
};