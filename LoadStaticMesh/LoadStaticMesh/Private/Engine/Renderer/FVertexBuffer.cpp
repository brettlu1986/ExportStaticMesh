
#include "stdafx.h"
#include "FVertexBuffer.h"

FVertexBuffer::FVertexBuffer()
:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_VERTEX_BUFFER)
,VertexCount(0)
,VertexDataSize(0)
, VertexByteData(nullptr)
{

}

FVertexBuffer::FVertexBuffer(E_FRESOURCE_TYPE Type)
:FRenderResource(Type)
,VertexCount(0)
,VertexDataSize(0)
, VertexByteData(nullptr)
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

	VertexByteData = (void*)(new char[VertexDataSize]);
	memset(VertexByteData, 0, VertexDataSize);
	memcpy(VertexByteData, DataSource, VertexDataSize);
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

