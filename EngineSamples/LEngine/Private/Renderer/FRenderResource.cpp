
#include "pch.h"
#include "FRenderResource.h"

FRenderResource::FRenderResource()
:ResourceType(E_FRESOURCE_TYPE::F_TYPE_UNKNOWN)
{

}

FRenderResource::FRenderResource(E_FRESOURCE_TYPE Type)
:ResourceType(Type)
{

}

FRenderResource::~FRenderResource()
{

}