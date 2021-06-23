#pragma once

#include "pch.h"
#include "FBloomDef.h"
#include "FDynamicRHI.h"

class FTexture;
class FResourceView;
class FPostResources;
class FToneMapping
{
public:
	FToneMapping();
	~FToneMapping();

	void Initialize(FPostResources* PostResIns);
	void Render(FIndexBuffer* IndexBuffer);

private:
	FPostResources* PostRes;

	FTexture* ToneMapTex;
	FResourceView* SRVToneMap;
	FResourceView* RTVToneMap;
};
