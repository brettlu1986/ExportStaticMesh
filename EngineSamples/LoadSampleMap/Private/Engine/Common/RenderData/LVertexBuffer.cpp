
#include "LVertexBuffer.h"

LVertexBuffer::LVertexBuffer(const char* DataSource, UINT DataSize, UINT DataCount)
:LResource(E_LRESOURCE_TYPE::L_TYPE_VERTEX_BUFFER)
,VertexCount(DataCount)
,VertexDataSize(DataSize)
,VertexByteData(nullptr)
{
	VertexByteData = (void*)(new char[VertexDataSize]);
	memset(VertexByteData, 0, VertexDataSize);
	memcpy(VertexByteData, DataSource, VertexDataSize);
}

LVertexBuffer::~LVertexBuffer()
{
	delete[] VertexByteData;
	VertexByteData = nullptr;
}

