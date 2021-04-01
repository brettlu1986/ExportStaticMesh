
#include "LDataComponent.h"
#include "D3D12Helper.h"

#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace std;

static const wstring ModelBinName = L"mesh.bin";

LDataComponent::LDataComponent()
:bUseHalfInt32(false)
, IndiceSize(0)
, IndicesCount(0)
, IndicesData(nullptr)
{
	ModelLocation = XMFLOAT3(0, 0, 0);
}

void LDataComponent::Init()
{
	ReadMeshDataFromFile(ModelBinName.c_str());

	IndicesCount = bUseHalfInt32 ? static_cast<UINT>(MeshIndicesHalf.size()) : static_cast<UINT>(MeshIndices.size());
	IndiceSize = bUseHalfInt32 ?IndicesCount * sizeof(UINT16) : IndicesCount * sizeof(UINT);
	IndicesData = bUseHalfInt32 ? reinterpret_cast<void*>(MeshIndicesHalf.data()) : reinterpret_cast<void*>(MeshIndices.data());
}

void LDataComponent::Destroy()
{

}

void LDataComponent::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
}

XMMATRIX LDataComponent::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	 //XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f) * XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
	return XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void LDataComponent::ReadMeshDataFromFile(LPCWSTR FileName)
{
	std::wstring FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	UINT MeshDataLen;
	Rf.read((char*)&MeshDataLen, sizeof(UINT));

	MeshDatas.resize(MeshDataLen);
	Rf.read((char*)MeshDatas.data(), MeshDataLen * sizeof(MeshData));
	Rf.read((char*)&bUseHalfInt32, sizeof(UINT8));

	UINT IndicesSize;
	Rf.read((char*)&IndicesSize, sizeof(UINT));

	if (bUseHalfInt32)
	{
		UINT HalfSize = IndicesSize * sizeof(UINT16);
		MeshIndicesHalf.resize(IndicesSize);
		Rf.read((char*)MeshIndicesHalf.data(), HalfSize);
	}
	else
	{
		UINT Size = IndicesSize * sizeof(UINT32);
		MeshIndices.resize(IndicesSize);
		Rf.read((char*)MeshIndices.data(), Size);
	}

	Rf.close();
	if (!Rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

void LDataComponent::GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut)
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

void LDataComponent::GetPositionTex0Input(std::vector<Vertex_PositionTex0>& OutPut)
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
		OutPut.push_back({ MeshDatas[i].Position, MeshDatas[i].Tex0 });
	}
}