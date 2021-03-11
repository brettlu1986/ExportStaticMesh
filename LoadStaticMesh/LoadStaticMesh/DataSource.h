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

struct MeshDataNew
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex0;
	XMFLOAT2 Tex1;
	XMFLOAT4 Color;
};

//use for {Position, Color} format
struct Vertex_PositionColor
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};
#pragma pack(pop)

class DataSource
{
public:
	DataSource();
	~DataSource();

	void Initialize(ApplicationMain* application);

	const CameraData& GetCameraData() { return m_camera_data; }

	void GetPositionColorInput(std::vector<Vertex_PositionColor>& outPut);
	const vector<UINT>& GetIndexDataInput() 
	{
		return MeshIndices;
	}

private: 
	std::wstring GetSaveDirectory();
	void ReadCameraDataFromFile(LPCWSTR fileName);
	void ReadMeshDataFromFile(LPCWSTR fileName);

	ApplicationMain* m_application;
	std::wstring m_save_path;

	CameraData m_camera_data;

	vector<MeshDataNew> MeshDatas;
	vector<UINT> MeshIndices;
};