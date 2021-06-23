#pragma once

#include "stdafx.h"

class LDevice
{
public:
	LDevice(UINT InW, UINT InH);
	virtual ~LDevice();

	virtual bool Run();
	virtual void Destroy();

	UINT GetWidth() const 
	{
		return Width;
	}

	UINT GetHeight() const
	{
		return Height;
	}

protected:
	UINT Width;
	UINT Height;
};
