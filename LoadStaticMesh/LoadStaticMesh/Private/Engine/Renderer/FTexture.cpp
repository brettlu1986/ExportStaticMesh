
#include "stdafx.h"
#include "FTexture.h"


FTexture::FTexture()
	:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_TEXTURE)
	,TextureData(nullptr)
{
}

FTexture::FTexture(E_FRESOURCE_TYPE Type)
	: FRenderResource(Type)
	, TextureData(nullptr)
{
}


FTexture::~FTexture()
{
	
}

void FTexture::Initialize()
{
}


void FTexture::Destroy()
{

}