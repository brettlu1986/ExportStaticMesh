#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"

FMesh::FMesh()
:VertexBuffer(FVertexBuffer())
,IndexBuffer(FIndexBuffer())
{

}

FMesh::~FMesh()
{

}

void FMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	 //XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f) * XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
	return XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void FMesh::InitRenderResource()
{
	//create model vertex buffer view
	std::vector<FVertex_PositionTex0> TriangleVertices;
	VertexBuffer.GetPositionTex0Input(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(FVertex_PositionTex0));
	const UINT StrideInByte = sizeof(FVertex_PositionTex0);
	RHIVertexBufferView = GDynamicRHI->RHICreateVertexBufferView(TriangleVertices.data(), StrideInByte, VertexBufferSize);

	//create model index buffer view
	RHIIndexBufferView = GDynamicRHI->RHICreateIndexBufferView(IndexBuffer.GetIndicesData(), IndexBuffer.GetIndicesDataSize(),
		IndexBuffer.GetIndicesCount(), IndexBuffer.GetIndicesType());

	//create model shader resource view
	RHIShaderResourceView = GDynamicRHI->RHICreateShaderResourceView(TexName);
}

void FMesh::Render(FRHICommandList& CommandList)
{	
	GDynamicRHI->RHIDrawWithVertexAndIndexBufferView(CommandList, RHIVertexBufferView, RHIIndexBufferView);
}

void FMesh::EndRender()
{

}