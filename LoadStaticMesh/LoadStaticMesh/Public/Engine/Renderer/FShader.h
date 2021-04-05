#pragma once

#include "FRenderResource.h"

class FShader : public FRenderResource
{
public:
	FShader(); 
	FShader(UINT8* InData, UINT InLength);
	virtual ~FShader();

	virtual void Destroy() override;
	virtual void Initialize() override;

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