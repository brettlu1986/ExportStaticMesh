

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

	//
	/*CameraData cameraTest = {};
	cameraTest.location = { 1.5534f, 1.3453f, 2.3345f };
	cameraTest.rotator = { 20.f, 30.f, -9.f };
	cameraTest.fov = 110.f;
	cameraTest.aspect = 1.7f;
	WriteCameraDataToFile(L"camera.bin", cameraTest);*/
	ReadCameraDataFromFile(L"camera.bin");
	
	/*MeshData mesh = {};
	float tmpVertices[9] = { 1.5f, 1.5f, 588.f, 554.f, 22.f, 22.f, -5.f, -6.f, -9.f };
	UINT tmpIndices[3] = { 2, 4, 5 };
	for (UINT i = 0; i < 9; ++i)
		mesh.vertices[i] = tmpVertices[i];
	for (UINT i = 0; i < 3; ++i)
		mesh.indices[i] = tmpIndices[i];
	WriteMeshDataToFile(L"mesh.bin", mesh);
	ReadMeshDataFromFile(L"mesh.bin");*/
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
	wf.write((char*)&cameraData, sizeof(CameraData));
	wf.close();
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
	wf.write((char*)&meshData, sizeof(MeshData));
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

	char* meshBuffer = new char[sizeof(MeshData)];
	memset(meshBuffer, '\0', sizeof(MeshData));
	rf.read(meshBuffer, sizeof(MeshData));
	m_mesh_data = *(MeshData*)(meshBuffer);

	delete[] meshBuffer;
	rf.close();
	if (!rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

