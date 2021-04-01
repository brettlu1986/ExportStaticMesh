#pragma once

#include "RHIResource.h"
#include "RHIDef.h"
class FRenderComponent
{
public:
	FRenderComponent();
	void Init();
	void Destroy();
	void Render(RHICommandList& CommandList);
	
	RHIView* RHIVertexBufferView;
	RHIView* RHIIndexBufferView;
	RHIView* RHIShaderResourceView;
};
