#pragma once

#include "stdafx.h"
#include "LDefine.h"

class LResource
{
public:
	LResource(E_LRESOURCE_TYPE Type);
	virtual ~LResource();

	virtual void InitRenderResource() = 0;
	virtual void DestroyRenderResource() = 0;

protected:
	E_LRESOURCE_TYPE ResourceType;
};