#pragma once

#include "stdafx.h"
#include "LResource.h"

class LVertexBuffer : public LResource
{
public:
	LVertexBuffer(const char* DataSource, UINT DataSize, UINT DataCount);
	virtual ~LVertexBuffer();

	const void* GetVertexByteData()
	{
		return VertexByteData;
	}

	UINT GetVertexCount()
	{
		return VertexCount;
	}

	UINT GetVertexDataSize()
	{
		return VertexDataSize;
	}
	
private:
	UINT VertexCount;
	UINT VertexDataSize;
	void* VertexByteData;
};