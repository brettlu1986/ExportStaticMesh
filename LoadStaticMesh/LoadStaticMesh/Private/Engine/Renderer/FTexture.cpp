#include "FTexture.h"


FTexture::FTexture()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_TEXTURE)
	,BitSize(0)
	,BitData(nullptr)
{
}

FTexture::FTexture(E_RESOURCE_TYPE Type)
	: FRenderResource(Type)
	, BitSize(0)
	, BitData(nullptr)
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