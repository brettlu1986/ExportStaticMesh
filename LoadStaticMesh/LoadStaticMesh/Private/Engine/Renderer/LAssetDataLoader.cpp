
#include "LAssetDataLoader.h"

#include <iostream>
#include <fstream>

using namespace std;

inline void GetAssetsPath(_Out_writes_(PathSize) WCHAR* Path, UINT PathSize)
{
	if (Path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, Path, PathSize);
	if (size == 0 || size == PathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* LastSlash = wcsrchr(Path, L'\\');
	if (LastSlash)
	{
		*(LastSlash + 1) = L'\0';
	}
}

inline std::string GetSaveDirectory()
{
	char CurPath[512];
	GetCurrentDirectoryA(_countof(CurPath), CurPath);
	std::string Path = CurPath;
	return Path + "\\Save\\";
}

std::wstring LAssetDataLoader::GetAssetFullPath(LPCWSTR AssetName)
{
	std::wstring AssetsPath;
	WCHAR AssetsPathChar[512];
	GetAssetsPath(AssetsPathChar, _countof(AssetsPathChar));
	AssetsPath = AssetsPathChar;
	return AssetsPath + AssetName;
}


void LAssetDataLoader::LoadMeshVertexDataFromFile(std::string FileName, FMesh* Mesh)
{
	FVertexBuffer* VertexBuffer = Mesh->GetVertexBuffer();
	FIndexBuffer* IndexBuffer = Mesh->GetIndexBuffer();

	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}
	XMFLOAT3 Location;
	Rf.read((char*)&Location, sizeof(XMFLOAT3));
	Mesh->SetModelLocation(Location);

	XMFLOAT3 Rotation;
	Rf.read((char*)&Rotation, sizeof(XMFLOAT3));
	Mesh->SetModelRotation(Rotation);

	XMFLOAT3 Scale;
	Rf.read((char*)&Scale, sizeof(XMFLOAT3));
	Mesh->SetModelScale(Scale);

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	std::vector<FVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData));
	VertexBuffer->Init((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData), VertexCount);

	bool bUseHalfInt32;
	Rf.read((char*)&bUseHalfInt32, sizeof(UINT8));

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT> Indices;
	vector<UINT16> IndicesHalf;
	UINT IndicesByteSize = bUseHalfInt32 ? IndicesCount * sizeof(UINT16) :
		IndicesCount * sizeof(UINT32);

	if (bUseHalfInt32)
	{
		IndicesHalf.resize(IndicesCount);
		Rf.read((char*)IndicesHalf.data(), IndicesByteSize);
	}
	else
	{
		Indices.resize(IndicesCount);
		Rf.read((char*)Indices.data(), IndicesByteSize);
	}

	IndexBuffer->Init(IndicesCount, IndicesByteSize,
		bUseHalfInt32 ? E_INDEX_TYPE::TYPE_UINT_16 : E_INDEX_TYPE::TYPE_UINT_32,
		bUseHalfInt32 ? reinterpret_cast<void*>(IndicesHalf.data()) : reinterpret_cast<void*>(Indices.data()));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadCameraDataFromFile(std::string FileName, LCamera& Camera)
{
	FCameraData& CameraDatas = Camera.GetCameraData();
	std::string FName = GetSaveDirectory() + FileName;
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
