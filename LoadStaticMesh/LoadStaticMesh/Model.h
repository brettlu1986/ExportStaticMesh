#pragma once

#include "GraphicHelper.h"
#include "MathHelper.h"

using namespace DirectX;
using namespace std;

#pragma pack(push)
#pragma pack(4)
struct MeshData
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex0;
	XMFLOAT2 Tex1;
	XMFLOAT4 Color;
};
#pragma pack(pop)

//use for {Position, Color} format
struct Vertex_PositionColor
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

struct Vertex_PositionTex0
{
	XMFLOAT3 Position;
	XMFLOAT2 Tex0;
};

class Model
{

public:
	Model();
	~Model();

	void Init();
	void Destroy();
	void UpdateConstantBuffers(FXMMATRIX View, CXMMATRIX Projection);

	void CreateVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateConstantBuffer(ID3D12Device* Device);
	void CreateDDSTextureFomFile(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const
	{
		return VertexBufferView;
	}

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const 
	{
		return IndexBufferView;
	}

	ID3D12Resource* GetTextureResource() const
	{
		return TextureResource.Get();
	}

	UINT GetIndicesCount() const { return IndicesCount; }

	D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferDesc()
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
		CbvDesc.BufferLocation = ConstantBuffer->GetGPUVirtualAddress();
		CbvDesc.SizeInBytes = ConstantBufferSize;
		return CbvDesc;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = TextureResource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = TextureResource->GetDesc().MipLevels;
		return srvDesc;
	}

	void SetModelLocation(XMFLOAT3 Location);

	XMMATRIX GetModelMatrix();
private: 
	void GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut);
	void GetPositionTex0Input(std::vector<Vertex_PositionTex0>& OutPut);
	void ReadMeshDataFromFile(LPCWSTR FileName);

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> VertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> IndexBufferUpload;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	ComPtr<ID3D12Resource> ConstantBuffer;
	ObjectConstants ObjectConstant;
	UINT8* pCbvDataBegin;

	ComPtr<ID3D12Resource> TextureResource ;
	ComPtr<ID3D12Resource> TextureResourceUpload ;

	UINT IndiceSize;
	UINT IndicesCount;
	UINT ConstantBufferSize;

	vector<MeshData> MeshDatas;
	vector<UINT> MeshIndices;
	vector<UINT16> MeshIndicesHalf;
	bool bUseHalfInt32;

	XMFLOAT3 ModelLocation;

	
};