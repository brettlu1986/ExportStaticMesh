

#include "DataSource.h"
#include "MathHelper.h"

#include <DirectXColors.h>
#include <iostream>
#include <fstream>

using namespace std;

DataSource::DataSource()
	:MainApplication(nullptr)
	,CameraDatas(CameraData())
	,bUseHalfInt32(false)
{
}

DataSource::~DataSource()
{
}

void DataSource::Initialize(ApplicationMain* Application)
{
	MainApplication = Application;
	SavePath = GetSaveDirectory();

	ReadCameraDataFromFile(L"camera.bin");
	ReadMeshDataFromFile(L"mesh.bin");
}

std::wstring DataSource::GetSaveDirectory()
{
	WCHAR CurPath[512];
	GetCurrentDirectory(_countof(CurPath), CurPath);
	std::wstring Path = CurPath;
	return Path + L"\\Save\\";
}

void DataSource::ReadCameraDataFromFile(LPCWSTR FileName)
{
	std::wstring FName = SavePath + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	char* CameraBuffer = new char[sizeof(CameraData)];
	memset(CameraBuffer, '\0', sizeof(CameraData));
	Rf.read(CameraBuffer, sizeof(CameraData));
	CameraDatas = *(CameraData*)(CameraBuffer);
	
	//calculate model matrix : scale * rotation * translation
	//in ue4: ue4x = Forward ue4y = Right ue4z = Up
	//in direct x: use left hand coordinate, x = Right, y = Up, z = Forward
	//we have the conversion: x = ue4y, y = ue4z, z = ue4x, Yaw Pitch Roll stay the same
	CameraDatas.Location = MathHelper::GetUe4ConvertLocation(CameraDatas.Location);
	CameraDatas.Target = MathHelper::GetUe4ConvertLocation(CameraDatas.Target);

	delete[] CameraBuffer;
	Rf.close();
	if (!Rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return ;
	}
}

void DataSource::ReadMeshDataFromFile(LPCWSTR FileName)
{
	std::wstring FName = SavePath + FileName;
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
	if(bUseHalfInt32)
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

void DataSource::GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut)
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
