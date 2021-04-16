#pragma once

#include "stdafx.h"
#include "FRenderer.h"

class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	void RenderFrameResource(FFrameResource& FrameResource) ;
};
