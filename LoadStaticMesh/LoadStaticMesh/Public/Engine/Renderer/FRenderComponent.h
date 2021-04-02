#pragma once

#include "FRHIResource.h"
#include "FRHIDef.h"
class FRenderComponent
{
public:
	FRenderComponent();
	void Init();
	void Destroy();
	void Render(FRHICommandList& CommandList);
	
	FRHIView* RHIVertexBufferView;
	FRHIView* RHIIndexBufferView;
	FRHIView* RHIShaderResourceView;
};
