

#include "DataSource.h"

#include <iostream>
#include <fstream>

using namespace std;

static XMFLOAT4 DefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };

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

//void DataSource::WriteCameraDataToFile(LPCWSTR fileName, CameraData& cameraData)
//{
//	std::wstring fName = m_save_path + fileName;
//	ofstream wf(fName, ios::out | ios::binary );
//	if (!wf)
//	{
//		cout << "Cannot open file!" << endl;
//		return;
//	}
//	//1.转成byte写
//	UINT sizeCamera = sizeof(CameraData);
//	char* cameraBytes = new char[sizeCamera];
//	memset(cameraBytes, 0, sizeCamera);
//	memcpy(cameraBytes, (char*)&cameraData, sizeCamera);
//	wf.write(cameraBytes, sizeCamera);
//	//2.对于camera data 因为是基本数据类型，可以直接写
//	//wf.write((char*)&cameraData, sizeof(CameraData));
//	wf.close();
//	delete[] cameraBytes;
//	if (!wf.good())
//	{
//		cout << "Error occurred at writing time!" << endl;
//		return;
//	}
//}

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

//void DataSource::WriteMeshDataToFile(LPCWSTR fileName, MeshData& meshData)
//{
//	std::wstring fName = m_save_path + fileName;
//	ofstream wf(fName, ios::out | ios::binary);
//	if (!wf)
//	{
//		cout << "Cannot open file!" << endl;
//		return;
//	}
//
//	size_t vSize = meshData.vertices.size();
//	wf.write((char*)&vSize, sizeof(vSize));
//	wf.write((char*)&meshData.vertices[0], vSize * sizeof(float));
//
//	size_t cSize = meshData.colors.size();
//	wf.write((char*)&cSize, sizeof(cSize));
//	wf.write((char*)&meshData.colors[0], cSize * sizeof(float));
//
//	size_t iSize = meshData.indices.size();
//	wf.write((char*)&iSize, sizeof(iSize));
//	wf.write((char*)&meshData.indices[0], iSize * sizeof(UINT));
//
//	wf.close();
//	if (!wf.good())
//	{
//		cout << "Error occurred at writing time!" << endl;
//		return;
//	}
//}

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

	std::vector<float> vertices;
	vertices.resize(vecSize);
	rf.read((char*)vertices.data(), vecSize * sizeof(float));

	UINT vertexSize = vecSize / 3;
	m_mesh_data.vertices.reserve(vertexSize);
	for (size_t i = 0; i < vertexSize; i++)
	{
		size_t begin = i * 3;
		XMFLOAT3 location = { vertices[begin], vertices[begin + 1], vertices[begin + 2] };
		m_mesh_data.vertices.push_back(location);
	}

	//read color
	char* cLenChar = new char[sizeof(size_t)];
	rf.read(cLenChar, sizeof(size_t));
	UINT coSize = *(UINT*)cLenChar;

	std::vector<float> colors;
	colors.resize(coSize);
	rf.read((char*)colors.data(), coSize * sizeof(float));

	UINT colorSize = coSize / 4;
	m_mesh_data.colors.reserve(colorSize);
	for (size_t i = 0; i < colorSize; i++)
	{
		size_t begin = i * 4;
		XMFLOAT4 color = { colors[begin], colors[begin + 1], colors[begin + 2], colors[begin + 3] };
		m_mesh_data.colors.push_back(color);
	}

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

void DataSource::GetPositionColorInput(std::vector<Vertex_PositionColor>& outPut)
{
	if (m_mesh_data.vertices.size() == 0)
	{
		cout << "Mesh vertices is zero!" << endl;
		return;
	}
		
	UINT verticesSize = static_cast<UINT>(m_mesh_data.vertices.size());
	UINT colorSize = static_cast<UINT>(m_mesh_data.colors.size());
	outPut.reserve(verticesSize);
	for (size_t i = 0; i < verticesSize; i++)
	{
		XMFLOAT4 color = i >= colorSize ? DefaultColor : m_mesh_data.colors[i];
		outPut.push_back({ m_mesh_data.vertices[i], color });
	}

}

