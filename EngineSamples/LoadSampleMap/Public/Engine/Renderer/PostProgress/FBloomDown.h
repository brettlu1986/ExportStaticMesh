#pragma once

#include "stdafx.h"
#include "FBloomDef.h"
#include "FDynamicRHI.h"

class FPostResources;
class FTexture;
class FResourceView;
class FBloomDown
{
public:
	FBloomDown();
	~FBloomDown();

	void Initialize(FPostResources* PostResIns);
	void Render(FIndexBuffer* IndexBuffer);

	FResourceView* GetSRV(UINT Index)
	{
		return SRVBloomDown[Index];
	}
private:
	FPostResources* PostRes;

	FRHIViewPort BloomDowmViewPort[DOWN_SAMPLE_COUNT];
	//bloom down
	FTexture* BloomDownTex[DOWN_SAMPLE_COUNT];
	FResourceView* SRVBloomDown[DOWN_SAMPLE_COUNT];
	FResourceView* RTVBloomDown[DOWN_SAMPLE_COUNT];
	FResourceView* CBVBloomDown[DOWN_SAMPLE_COUNT];
};
