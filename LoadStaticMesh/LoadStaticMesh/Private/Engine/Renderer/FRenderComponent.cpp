#include "FRenderComponent.h"
#include "RHI.h"

FRenderComponent::FRenderComponent()
:RHIVertexBufferView(nullptr)
,RHIIndexBufferView(nullptr)
,RHIShaderResourceView(nullptr)
{
}

void FRenderComponent::Render(RHICommandList& CommandList)
{
	GDynamicRHI->RHIDrawWithVertexAndIndexBufferView(CommandList, RHIVertexBufferView, RHIIndexBufferView);
}

void FRenderComponent::Init()
{

}

void FRenderComponent::Destroy()
{

}