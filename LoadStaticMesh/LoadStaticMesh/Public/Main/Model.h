#pragma once

#include "D3D12Helper.h"
#include "MathHelper.h"

#include "RHIResource.h"

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

	void CreateVertexBufferView();
	void CreateIndexBufferView();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const
	{
		return VertexBufferView;
	}

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const 
	{
		return IndexBufferView;
	}

	UINT GetIndicesCount() const { return IndicesCount; }


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

	void GetPositionTex0Input(std::vector<Vertex_PositionTex0>& OutPut);

	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> VertexUploadBuffer;
	UINT VertexBufferSize;

	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> IndexBufferUpload;
	UINT IndiceSize;
	UINT IndicesCount;
	bool bUseHalfInt32;

	vector<UINT> MeshIndices;
	vector<UINT16> MeshIndicesHalf;

	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> TextureResourceUpload;

	//
	RHIView* RHIVertexBufferView;
	RHIView* RHIIndexBufferView;
	RHIView* RHIShaderResourceView;
	//

private: 
	void GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut);
	
	void ReadMeshDataFromFile(LPCWSTR FileName);
	
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	vector<MeshData> MeshDatas;
	XMFLOAT3 ModelLocation;
};