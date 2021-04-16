#pragma once

#include "FDynamicRHI.h"

class FRenderer
{
public:
	FRenderer();
	virtual ~FRenderer();

	virtual void RenderFrameResource(FFrameResource& FrameResource) = 0;
	
};