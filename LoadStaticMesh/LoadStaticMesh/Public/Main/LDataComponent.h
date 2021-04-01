#pragma once

#include "stdafx.h"
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

class LDataComponent
{
public:
	LDataComponent();

	void Init();
	void Destroy();
	void SetModelLocation(XMFLOAT3 Location);

	XMMATRIX GetModelMatrix();
	void GetPositionTex0Input(std::vector<Vertex_PositionTex0>& OutPut);

	bool IsUseHalfInt32() const 
	{
		return bUseHalfInt32;
	}

	UINT IndiceSize;
	UINT IndicesCount;

	vector<UINT> MeshIndices;
	vector<UINT16> MeshIndicesHalf;
	
private:
	void GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut);
	void ReadMeshDataFromFile(LPCWSTR FileName);
	
	std::vector<MeshData> MeshDatas;

	bool bUseHalfInt32;
	XMFLOAT3 ModelLocation;
};


