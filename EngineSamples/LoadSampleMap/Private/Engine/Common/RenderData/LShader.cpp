
#include "LShader.h"

LShader::LShader()
:LResource(E_LRESOURCE_TYPE::L_TYPE_SHADER)
, pData(nullptr)
, DataLength(0)
{

}

void LShader::Init(UINT8* InData, UINT InLength)
{
	pData = new UINT8[InLength];
	memset(pData, 0, InLength);
	memcpy(pData, InData, InLength);
	DataLength = InLength;
}

LShader::~LShader()
{
	delete[] pData;
	pData = nullptr;
}

