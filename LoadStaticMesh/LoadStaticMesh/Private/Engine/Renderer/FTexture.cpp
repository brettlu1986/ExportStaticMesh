#include "FTexture.h"


FTexture::FTexture()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_TEXTURE)
{

}

FTexture::FTexture(E_RESOURCE_TYPE Type)
	: FRenderResource(Type)
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