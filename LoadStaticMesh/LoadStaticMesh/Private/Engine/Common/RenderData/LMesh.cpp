

#include "LMesh.h"

LMesh::LMesh()
:LResource(E_LRESOURCE_TYPE::L_TYPE_MESH)
{

}

LMesh::~LMesh()
{

}

void LMesh::SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount)
{
	VertexBufferData = make_unique<LVertexBuffer>(DataSource, DataSize, DataCount);
}

void LMesh::SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData)
{
	IndexBufferData = make_unique<LIndexBuffer>(InCount, InByteSize, InType, InData);
}

void LMesh::InitRenderResource()
{

}

void LMesh::DestroyRenderResource()
{

}



