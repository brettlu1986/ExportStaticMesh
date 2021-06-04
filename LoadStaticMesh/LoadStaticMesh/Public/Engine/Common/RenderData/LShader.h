#pragma once

#include "LShader.h"
#include "LResource.h"

class LShader : public LResource
{
public:
	LShader();
	
	virtual ~LShader();

	void Init(UINT8* InData, UINT InLength);

	UINT8* GetShaderByteCode() const
	{
		return pData;
	}

	UINT GetDataLength() const
	{
		return DataLength;
	}

private:
	UINT8* pData;
	UINT DataLength;
};