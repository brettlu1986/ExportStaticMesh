
#include "stdafx.h"
#include "FTexture.h"


FTexture::FTexture()
	:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_TEXTURE)
	,BitSize(0)
	,BitData(nullptr)
	,TexTransform(MathHelper::Identity4x4())
{
	XMStoreFloat4x4(&TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
}

FTexture::FTexture(E_FRESOURCE_TYPE Type)
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

void FTexture::InitializeTexture(const string& Name)
{


}

void FTexture::Destroy()
{

}