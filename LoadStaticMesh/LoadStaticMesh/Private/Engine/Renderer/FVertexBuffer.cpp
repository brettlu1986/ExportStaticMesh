
#include "stdafx.h"
#include "FVertexBuffer.h"

FVertexBuffer::FVertexBuffer()
:FRenderResource(E_RESOURCE_TYPE::TYPE_VERTEX_BUFFER)
,VertexCount(0)
,VertexDataSize(0)
{

}

FVertexBuffer::FVertexBuffer(E_RESOURCE_TYPE Type)
:FRenderResource(Type)
,VertexCount(0)
,VertexDataSize(0)
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

void FVertexBuffer::Destroy()
{

}

void FVertexBuffer::Initialize()
{

}

