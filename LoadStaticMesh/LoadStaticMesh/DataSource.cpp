

#include "DataSource.h"

#include <iostream>
#include <fstream>

using namespace std;

DataSource::DataSource()
	:m_application(nullptr)
	,m_camera_data(CameraData())
	,m_mesh_data(MeshData())
{
}

DataSource::~DataSource()
{
}

void DataSource::Initialize(ApplicationMain* application)
{
	m_application = application;
	m_save_path = GetSaveDirectory();

	//WriteCameraDataToFile(L"camera.bin", cameraTest);
	//WriteMeshDataToFile(L"mesh.bin", mesh);
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

void DataSource::WriteCameraDataToFile(LPCWSTR fileName, CameraData& cameraData)
{
	std::wstring fName = m_save_path + fileName;
	ofstream wf(fName, ios::out | ios::binary );
	if (!wf)
	{
		cout << "Cannot open file!" << endl;
		return;
	}
	//1.转成byte写
	UINT sizeCamera = sizeof(CameraData);
	char* cameraBytes = new char[sizeCamera];
	memset(cameraBytes, 0, sizeCamera);
	memcpy(cameraBytes, (char*)&cameraData, sizeCamera);
	wf.write(cameraBytes, sizeCamera);
	//2.对于camera data 因为是基本数据类型，可以直接写
	//wf.write((char*)&cameraData, sizeof(CameraData));
	wf.close();
	delete[] cameraBytes;
	if (!wf.good())
	{
		cout << "Error occurred at writing time!" << endl;
		return;
	}
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

void DataSource::WriteMeshDataToFile(LPCWSTR fileName, MeshData& meshData)
{
	std::wstring fName = m_save_path + fileName;
	ofstream wf(fName, ios::out | ios::binary);
	if (!wf)
	{
		cout << "Cannot open file!" << endl;
		return;
	}

	size_t vSize = meshData.vertices.size();
	wf.write((char*)&vSize, sizeof(vSize));
	wf.write((char*)&meshData.vertices[0], vSize * sizeof(float));

	size_t cSize = meshData.colors.size();
	wf.write((char*)&cSize, sizeof(cSize));
	wf.write((char*)&meshData.colors[0], cSize * sizeof(float));

	size_t iSize = meshData.indices.size();
	wf.write((char*)&iSize, sizeof(iSize));
	wf.write((char*)&meshData.indices[0], iSize * sizeof(UINT));

	wf.close();
	if (!wf.good())
	{
		cout << "Error occurred at writing time!" << endl;
		return;
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

	//vertex positions
	char* vetLenChar = new char[sizeof(size_t)];
	rf.read(vetLenChar, sizeof(size_t));
	UINT vecSize = *(UINT*)vetLenChar;

	m_mesh_data.vertices.resize(vecSize);
	rf.read((char*)m_mesh_data.vertices.data(), vecSize * sizeof(float));

	//read color
	char* cLenChar = new char[sizeof(size_t)];
	rf.read(cLenChar, sizeof(size_t));
	UINT coSize = *(UINT*)cLenChar;

	m_mesh_data.colors.resize(coSize);
	rf.read((char*)m_mesh_data.colors.data(), coSize * sizeof(UINT));

	//indices
	char* inLenChar = new char[sizeof(size_t)];
	rf.read(inLenChar, sizeof(size_t));
	UINT inSize = *(UINT*)inLenChar;

	m_mesh_data.indices.resize(inSize);
	rf.read((char*)m_mesh_data.indices.data(), inSize * sizeof(UINT));
	
	delete[] vetLenChar;
	delete[] cLenChar;
	delete[] inLenChar;
	rf.close();
	if (!rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

