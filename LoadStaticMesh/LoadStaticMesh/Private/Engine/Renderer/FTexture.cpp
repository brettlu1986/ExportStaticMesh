
#include "stdafx.h"
#include "FTexture.h"


FTexture::FTexture()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_TEXTURE)
	,BitSize(0)
	,BitData(nullptr)
	, TexTransform(MathHelper::Identity4x4())
{
	XMStoreFloat4x4(&TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
}

FTexture::FTexture(E_RESOURCE_TYPE Type)
	: FRenderResource(Type)
	, BitSize(0)
	, BitData(nullptr)
	, TexTransform(MathHelper::Identity4x4())
{
}


FTexture::~FTexture()
{
	
}

void FTexture::Initialize()
{
}

void FTexture::InitializeTexture(const std::string& Name)
{


}

void FTexture::Destroy()
{

}