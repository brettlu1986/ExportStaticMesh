#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"

using namespace DirectX;
using namespace std;

#pragma pack(push)
#pragma pack(4)
struct Location
{
	float x;
	float y;
	float z;
};

struct Rotator
{
	float Pitch;
	float Yaw;
	float Roll;
};

struct Color
{
	float R;
	float G;
	float B;
	float A;
};

struct CameraData {
	Location location;
	Rotator rotator;
	float fov;
	float aspect;
};

struct MeshData
{
	//int test1;
	//char test2[20];
	vector<float> vertices;
	vector<float> colors;
	vector<UINT> indices;
};

#pragma pack(pop)

class DataSource
{
public:
	DataSource();
	~DataSource();

	void Initialize(ApplicationMain* application);

	const CameraData& GetCameraData() { return m_camera_data; }
	const MeshData& GetMeshData() { return m_mesh_data; }

private: 
	std::wstring GetSaveDirectory();
	void WriteCameraDataToFile(LPCWSTR fileName, CameraData& cameraData);
	void ReadCameraDataFromFile(LPCWSTR fileName);

	void WriteMeshDataToFile(LPCWSTR fileName, MeshData& meshData);
	void ReadMeshDataFromFile(LPCWSTR fileName);

	ApplicationMain* m_application;
	std::wstring m_save_path;

	CameraData m_camera_data;
	MeshData m_mesh_data;
};