#pragma once

#include "stdafx.h"
#include "FBloomDef.h"
#include "FDynamicRHI.h"

class FTexture;
class FResourceView;
class FPostResources;
class FSunMerge
{
public:
	FSunMerge();
	~FSunMerge();

	void Initialize(FPostResources* PostResIns);
	void Render(FIndexBuffer* IndexBuffer);

	FResourceView* GetSRV()
	{
		return SRVSunMerge;
	}
private:
	FPostResources* PostRes;
	//sunmerge
	FRHIViewPort SunMergeViewPort;
	FTexture* SunMergeTex;
	FResourceView* SRVSunMerge;
	FResourceView* RTVSunMerge;
	FResourceView* CBVSunMerge;

};
