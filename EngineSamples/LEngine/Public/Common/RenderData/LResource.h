#pragma once

#include "pch.h"
#include "LDefine.h"

class LENGINE_API LResource
{
public:
	LResource(E_LRESOURCE_TYPE Type);
	virtual ~LResource();

protected:
	E_LRESOURCE_TYPE ResourceType;
};