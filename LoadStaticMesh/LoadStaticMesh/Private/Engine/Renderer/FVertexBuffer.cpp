
#include "FVertexBuffer.h"

FVertexBuffer::FVertexBuffer()
	:VertexCount(0)
	, VertexDataSize(0)
{

}

FVertexBuffer::FVertexBuffer(const char* DataSource, UINT DataSize, UINT DataCount)
:VertexCount(DataCount)
,VertexDataSize(DataSize)
{
	Init(DataSource, DataSize, DataCount);
}

void FVertexBuffer::Init(const char* DataSource, UINT DataSize, UINT DataCount)
{
	VertexCount = DataCount;
	VertexDataSize = DataSize;
	VertexData.resize(DataCount);
	memcpy(VertexData.data(), DataSource, DataSize);
}

FVertexBuffer::~FVertexBuffer()
{

}