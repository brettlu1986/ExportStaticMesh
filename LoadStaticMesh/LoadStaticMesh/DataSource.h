#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"

using namespace DirectX;

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

struct CameraData {
	Location location;
	Rotator rotator;
	float fov;
	float aspect;
};

static const UINT INDICES_COUNT = 512;
static const UINT MAX_VERTEX_COUNT = INDICES_COUNT * 3;

struct MeshData
{
	float vertices[MAX_VERTEX_COUNT];
	UINT indices[INDICES_COUNT];
	MeshData()
	{
		memset(vertices, 0, MAX_VERTEX_COUNT);
		memset(indices, 0, INDICES_COUNT);
	}
};

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