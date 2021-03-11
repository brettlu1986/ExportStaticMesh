

#include "DataSource.h"

#include <DirectXColors.h>

#include <iostream>
#include <fstream>

using namespace std;

DataSource::DataSource()
	:m_application(nullptr)
	,m_camera_data(CameraData())
{
}

DataSource::~DataSource()
{
}

void DataSource::Initialize(ApplicationMain* application)
{
	m_application = application;
	m_save_path = GetSaveDirectory();

	ReadCameraDataFromFile(L"camera.bin");
	ReadMeshDataFromFile(L"mesh.bin");
}

std::wstring DataSource::GetSaveDirectory()
{
	WCHAR curPath[512];
	GetCurrentDirectory(_countof(curPath), curPath);
	std::wstring path = curPath; 
	return path + L"\\Save\\";
}

void DataSource::ReadCameraDataFromFile(LPCWSTR fileName)
{
	std::wstring fName = m_save_path + fileName;
	ifstream rf(fName, ios::out | ios::binary);
	if (!rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	char* cameraBuffer = new char[sizeof(CameraData)];
	memset(cameraBuffer, '\0', sizeof(CameraData));
	rf.read(cameraBuffer, sizeof(CameraData));
	m_camera_data = *(CameraData*)(cameraBuffer);
	
	delete[] cameraBuffer;
	rf.close();
	if (!rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return ;
	}
}

void DataSource::ReadMeshDataFromFile(LPCWSTR fileName)
{
	std::wstring fName = m_save_path + fileName;
	ifstream rf(fName, ios::out | ios::binary);
	if (!rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	char* MeshDataLenChar = new char[sizeof(UINT)];
	rf.read(MeshDataLenChar, sizeof(UINT));
	UINT MeshDataLen = *(UINT*)MeshDataLenChar;

	MeshDatas.resize(MeshDataLen);
	rf.read((char*)MeshDatas.data(), MeshDataLen * sizeof(MeshDataNew));

	char* IndicesLenChar = new char[sizeof(UINT)];
	rf.read(IndicesLenChar, sizeof(UINT));
	UINT IndicesSize = *(UINT*)IndicesLenChar;

	MeshIndices.resize(IndicesSize);
	rf.read((char*)MeshIndices.data(), IndicesSize * sizeof(UINT32));
	
	delete[] MeshDataLenChar;
	delete[] IndicesLenChar;
	rf.close();
	if (!rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

void DataSource::GetPositionColorInput(std::vector<Vertex_PositionColor>& outPut)
{
	if (MeshDatas.size() == 0)
	{
		cout << "Mesh vertices is zero!" << endl;
		return;
	}
		
	UINT verticesSize = static_cast<UINT>(MeshDatas.size());
	outPut.reserve(verticesSize);
	for (size_t i = 0; i < verticesSize; i++)
	{
		outPut.push_back({ MeshDatas[i].Position, MeshDatas[i].Color });
	}
}
