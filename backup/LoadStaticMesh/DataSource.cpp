

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

	/*CameraData cameraTest = {};
	cameraTest.location = { 1.5534f, 1.3453f, 2.3345f };
	cameraTest.rotator = { 20.f, 30.f, -9.f };
	cameraTest.fov = 110.f;
	cameraTest.aspect = 1.7f;
	WriteCameraDataToFile(L"camera.bin", cameraTest);*/
	//ReadCameraDataFromFile(L"camera.bin");
	
	/*MeshData mesh = {};
	mesh.test1 = 1000;
	memset(mesh.test2, 0, sizeof(mesh.test2));
	const char tmpstr[] = "test info";
	memcpy(mesh.test2, tmpstr, sizeof(tmpstr));
	vector<float> tmpVertices{ 1.5f, 1.5f, 588.f, 554.f, 22.f, 22.f, -5.f, -6.f, -9.f };
	for( float f : tmpVertices)
		mesh.vertices.push_back(f) ;
	vector<UINT> tmpIndices{ 2, 4, 5 };
	for (UINT f : tmpIndices)
		mesh.indices.push_back(f);

	WriteMeshDataToFile(L"mesh.bin", mesh);*/
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
	
	/*wf.write((char*)&meshData.test1, sizeof(meshData.test1));
	wf.write((char*)&meshData.test2, sizeof(meshData.test2));*/

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

	/*char* test1 = new char[sizeof(int)];
	rf.read(test1, sizeof(int));
	m_mesh_data.test1 = *(int*)test1;

	char* test2 = new char[sizeof(m_mesh_data.test2)];
	rf.read(test2, sizeof(m_mesh_data.test2));
	strcpy_s(m_mesh_data.test2, test2);*/

	char* vetLenChar = new char[sizeof(size_t)];
	rf.read(vetLenChar, sizeof(size_t));
	UINT vecSize = *(UINT*)vetLenChar;

	m_mesh_data.vertices.resize(vecSize);
	rf.read((char*)m_mesh_data.vertices.data(), vecSize * sizeof(float));

	char* inLenChar = new char[sizeof(size_t)];
	rf.read(inLenChar, sizeof(size_t));
	UINT inSize = *(UINT*)inLenChar;

	m_mesh_data.indices.resize(inSize);
	rf.read((char*)m_mesh_data.indices.data(), inSize * sizeof(UINT));
	
	/*delete[] test1;
	delete[] test2;*/
	delete[] vetLenChar;

	rf.close();
	if (!rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

