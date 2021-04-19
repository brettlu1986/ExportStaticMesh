
#include "stdafx.h"
#include "FRenderResource.h"

FRenderResource::FRenderResource()
:ResourceType(E_RESOURCE_TYPE::TYPE_UNKNOWN)
{

}

FRenderResource::FRenderResource(E_RESOURCE_TYPE Type)
:ResourceType(Type)
{

}

FRenderResource::~FRenderResource()
{

}