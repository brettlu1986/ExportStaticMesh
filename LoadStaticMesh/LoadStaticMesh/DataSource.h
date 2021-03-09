#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"

using namespace DirectX;
using namespace std;

#pragma pack(push)
#pragma pack(4)
struct CameraData {
	XMFLOAT3 location;
	XMFLOAT3 target;
	XMFLOAT3 rotator; // {Pitch, Yaw , Roll}
	float fov;
	float aspect;
};

struct MeshData
{
	vector<XMFLOAT3> vertices;
	vector<XMFLOAT4> colors;
	vector<UINT16> indices;
};
#pragma pack(pop)

struct Vertex_PositionColor
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

class DataSource
{
public:
	DataSource();
	~DataSource();

	void Initialize(ApplicationMain* application);

	const CameraData& GetCameraData() { return m_camera_data; }
	const MeshData& GetMeshData() { return m_mesh_data; }

	void GetPositionColorInput(std::vector<Vertex_PositionColor>& outPut);
	void GetIndexDataInput(std::vector<UINT16>& outPut);
private: 
	std::wstring GetSaveDirectory();
	//void WriteCameraDataToFile(LPCWSTR fileName, CameraData& cameraData);
	void ReadCameraDataFromFile(LPCWSTR fileName);

	//void WriteMeshDataToFile(LPCWSTR fileName, MeshData& meshData);
	void ReadMeshDataFromFile(LPCWSTR fileName);

	ApplicationMain* m_application;
	std::wstring m_save_path;

	CameraData m_camera_data;
	MeshData m_mesh_data;
};