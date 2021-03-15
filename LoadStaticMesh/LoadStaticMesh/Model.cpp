#include "Model.h"
#include "GraphicHelper.h"

#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace std;

static const wstring ModelBinName = L"mesh.bin";

Model::Model()
	:bUseHalfInt32(false)
	,VertexBufferView(D3D12_VERTEX_BUFFER_VIEW())
	,IndexBufferView(D3D12_INDEX_BUFFER_VIEW())
	,IndiceSize(0)
	,IndicesCount(0)
	,pCbvDataBegin(nullptr)
	,ObjectConstant({})
	,ConstantBufferSize(0)
{
	ModelLocation = XMFLOAT3(0, 0, 0);
}

Model::~Model()
{
	
}

void Model::Destroy()
{
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
	IndexBuffer.Reset();
	IndexBufferUpload.Reset();
	ConstantBuffer.Reset();
}

void Model::Init()
{
	ReadMeshDataFromFile(ModelBinName.c_str());
}

void Model::CreateVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	std::vector<Vertex_PositionColor> TriangleVertices;
	// Create the vertex buffer.
	// Define the geometry for a triangle.  read the vertices data
	GetPositionColorInput(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(Vertex_PositionColor));//sizeof(TriangleVertices);

	const CD3DX12_HEAP_PROPERTIES VertexDefaultProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC VertexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertexDefaultProp,
		D3D12_HEAP_FLAG_NONE,
		&VertexDefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&VertexBuffer)));
	NAME_D3D12_OBJECT(VertexBuffer);

	//create vertex upload heap
	const CD3DX12_HEAP_PROPERTIES VertextUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC VertextUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertextUploadProp,
		D3D12_HEAP_FLAG_NONE,
		&VertextUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&VertexUploadBuffer)));
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	D3D12_SUBRESOURCE_DATA SubResourceData;
	SubResourceData.pData = TriangleVertices.data();
	SubResourceData.RowPitch = VertexBufferSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	const D3D12_RESOURCE_BARRIER RbVertex1 = CD3DX12_RESOURCE_BARRIER::Transition(VertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->ResourceBarrier(1, &RbVertex1);
	UpdateSubresources<1>(CommandList, VertexBuffer.Get(), VertexUploadBuffer.Get(), 0, 0, 1, &SubResourceData);
	const D3D12_RESOURCE_BARRIER RbVertex2 = CD3DX12_RESOURCE_BARRIER::Transition(VertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	CommandList->ResourceBarrier(1, &RbVertex2);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(Vertex_PositionColor);
	VertexBufferView.SizeInBytes = VertexBufferSize;
}

void Model::CreateIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	//create index buffer 
	if (!bUseHalfInt32)
	{
		IndicesCount = static_cast<UINT>(MeshIndices.size());
		IndiceSize = IndicesCount * sizeof(UINT);
	}
	else
	{
		IndicesCount = static_cast<UINT>(MeshIndicesHalf.size());
		IndiceSize = IndicesCount * sizeof(UINT16);
	}

	const CD3DX12_HEAP_PROPERTIES IndexDefaultPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC IndexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(IndiceSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&IndexDefaultPro,
		D3D12_HEAP_FLAG_NONE,
		&IndexDefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&IndexBuffer)));
	NAME_D3D12_OBJECT(IndexBuffer);

	const CD3DX12_HEAP_PROPERTIES IndexUploadPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC IndexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(IndiceSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&IndexUploadPro,
		D3D12_HEAP_FLAG_NONE,
		&IndexUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&IndexBufferUpload)));
	NAME_D3D12_OBJECT(IndexBufferUpload);

	D3D12_SUBRESOURCE_DATA SubResourceData;
	if (bUseHalfInt32)
		SubResourceData.pData = MeshIndicesHalf.data();
	else
		SubResourceData.pData = MeshIndices.data();

	SubResourceData.RowPitch = IndiceSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	const D3D12_RESOURCE_BARRIER RbIndex1 = CD3DX12_RESOURCE_BARRIER::Transition(IndexBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->ResourceBarrier(1, &RbIndex1);
	UpdateSubresources<1>(CommandList, IndexBuffer.Get(), IndexBufferUpload.Get(), 0, 0, 1, &SubResourceData);
	const D3D12_RESOURCE_BARRIER RbIndex2 = CD3DX12_RESOURCE_BARRIER::Transition(IndexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	CommandList->ResourceBarrier(1, &RbIndex2);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = bUseHalfInt32 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndiceSize;
}

void Model::CreateConstantBuffer(ID3D12Device* Device)
{
	//constant buffer size must 256's multiple

	ConstantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(ConstantBufferSize);

	ThrowIfFailed(Device->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));

	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin)));
	memcpy(pCbvDataBegin, &ObjectConstant, sizeof(ObjectConstant));
	NAME_D3D12_OBJECT(ConstantBuffer);
}

void Model::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
}

XMMATRIX Model::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	return XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f) * XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void Model::UpdateConstantBuffers(FXMMATRIX View, CXMMATRIX Projection)
{
	XMMATRIX WorldViewProj = GetModelMatrix() * View * Projection;

	 //Update the constant buffer with the latest WorldViewProj matrix.
	XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	memcpy(pCbvDataBegin, &ObjectConstant, sizeof(ObjectConstant));
}

void Model::ReadMeshDataFromFile(LPCWSTR FileName)
{
	std::wstring FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	char* MeshDataLenChar = new char[sizeof(UINT)];
	Rf.read(MeshDataLenChar, sizeof(UINT));
	UINT MeshDataLen = *(UINT*)MeshDataLenChar;

	MeshDatas.resize(MeshDataLen);
	Rf.read((char*)MeshDatas.data(), MeshDataLen * sizeof(MeshData));

	char* IndicesLenChar = new char[sizeof(UINT)];
	Rf.read(IndicesLenChar, sizeof(UINT));
	UINT IndicesSize = *(UINT*)IndicesLenChar;

	UINT Current = (UINT)Rf.tellg();
	Rf.seekg(0, ios::end);
	UINT End = (UINT)Rf.tellg();
	UINT Left = End - Current;
	Rf.seekg(Current, ios::beg);

	UINT HalfSize = IndicesSize * sizeof(UINT16);
	UINT Size = IndicesSize * sizeof(UINT32);
	bUseHalfInt32 = Left <= HalfSize;
	if (bUseHalfInt32)
	{
		MeshIndicesHalf.resize(IndicesSize);
		Rf.read((char*)MeshIndicesHalf.data(), HalfSize);
	}
	else
	{
		MeshIndices.resize(IndicesSize);
		Rf.read((char*)MeshIndices.data(), Size);
	}

	delete[] MeshDataLenChar;
	delete[] IndicesLenChar;
	Rf.close();
	if (!Rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

void Model::GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut)
{
	if (MeshDatas.size() == 0)
	{
		cout << "Mesh vertices is zero!" << endl;
		return;
	}

	UINT VerticesSize = static_cast<UINT>(MeshDatas.size());
	OutPut.reserve(VerticesSize);
	for (size_t i = 0; i < VerticesSize; i++)
	{
		OutPut.push_back({ MeshDatas[i].Position, MeshDatas[i].Color });
	}
}