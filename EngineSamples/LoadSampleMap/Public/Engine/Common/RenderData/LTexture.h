#pragma once

#include "LTexture.h"

#include "LResource.h"
#include "FDDSTextureLoader.h"

class LTexture : public LResource
{
public:
	LTexture();
	virtual ~LTexture();

	uint8_t* BitData;
	size_t BitSize;
	DDS_HEADER* Header;
	unique_ptr<uint8_t[]> DdsData;
};
