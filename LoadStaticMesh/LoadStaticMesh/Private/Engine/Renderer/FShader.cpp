
#include "FShader.h"

FShader::FShader()
:FRenderResource(E_RESOURCE_TYPE::TYPE_SHADER)
, pData(nullptr)
, DataLength(0)
{

}

FShader::FShader(UINT8* InData, UINT InLength)
{
	pData = new UINT8[InLength];
	memset(pData, 0, InLength);
	memcpy(pData, InData, InLength);
}

FShader::~FShader()
{
	Destroy();
}

void FShader::Destroy()
{
	delete[] pData;
	pData = nullptr;
}

void FShader::Initialize()
{

}