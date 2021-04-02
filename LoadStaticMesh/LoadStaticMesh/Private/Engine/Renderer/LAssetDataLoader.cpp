
#include "LAssetDataLoader.h"

#include <iostream>
#include <fstream>

using namespace std;

inline std::wstring GetSaveDirectory()
{
	WCHAR CurPath[512];
	GetCurrentDirectory(_countof(CurPath), CurPath);
	std::wstring Path = CurPath;
	return Path + L"\\Save\\";
}

void LAssetDataLoader::LoadMeshVertexDataFromFile(LPCWSTR FileName, FMesh& Mesh)
{
	FVertexBuffer& VertexBuffer = Mesh.GetVertexBuffer();
	FIndexBuffer& IndexBuffer = Mesh.GetIndexBuffer();

	std::wstring FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}
	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	std::vector<FVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData));
	VertexBuffer.Init((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData), VertexCount);

	bool bUseHalfInt32;
	Rf.read((char*)&bUseHalfInt32, sizeof(UINT8));

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT> Indices;
	vector<UINT16> IndicesHalf;
	UINT IndicesByteSize = bUseHalfInt32? IndicesCount * sizeof(UINT16) :
		IndicesCount * sizeof(UINT32);

	if(bUseHalfInt32)
	{
		IndicesHalf.resize(IndicesCount);
		Rf.read((char*)IndicesHalf.data(), IndicesByteSize);
	}
	else 
	{
		Indices.resize(IndicesCount);
		Rf.read((char*)Indices.data(), IndicesByteSize);
	}

	IndexBuffer.Init(IndicesCount, IndicesByteSize, 
		bUseHalfInt32? E_INDEX_TYPE::TYPE_UINT_16 : E_INDEX_TYPE::TYPE_UINT_32, 
		bUseHalfInt32 ? reinterpret_cast<void*>(IndicesHalf.data()) : reinterpret_cast<void*>(Indices.data()));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadCameraDataFromFile(LPCWSTR FileName, LCamera& Camera)
{
	FCameraData& CameraDatas = Camera.GetCameraData();
	std::wstring FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	char* CameraBuffer = new char[sizeof(FCameraData)];
	memset(CameraBuffer, '\0', sizeof(FCameraData));
	Rf.read(CameraBuffer, sizeof(FCameraData));
	CameraDatas = *(FCameraData*)(CameraBuffer);

	delete[] CameraBuffer;
	Rf.close();
	if (!Rf.good()) {
		return;
	}
}
