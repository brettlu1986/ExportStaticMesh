#pragma once

#include "stdafx.h"
#include "ApplicationMain.h"

using namespace DirectX;
using namespace std;

#pragma pack(push)
#pragma pack(4)
struct CameraData {
	XMFLOAT3 Location;
	XMFLOAT3 Target;
	XMFLOAT3 Rotator; // {Pitch, Yaw , Roll}
	float Fov;
	float Aspect;
};

struct MeshData
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
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};
#pragma pack(pop)

class DataSource
{
public:
	DataSource();
	~DataSource();

	void Initialize(ApplicationMain* Application);

	const CameraData& GetCameraData() { return CameraDatas; }

	void GetPositionColorInput(std::vector<Vertex_PositionColor>& OutPut);
	const vector<UINT>& GetIndexDataInput() 
	{
		return MeshIndices;
	}

	const vector<UINT16>& GetIndexDataValueHalfInput()
	{
		return MeshIndicesHalf;
	}

	bool IsIndicesValueHalfInt32() const {
		return bUseHalfInt32;
	}
private: 
	std::wstring GetSaveDirectory();
	void ReadCameraDataFromFile(LPCWSTR FileName);
	void ReadMeshDataFromFile(LPCWSTR FileName);

	ApplicationMain* MainApplication;
	std::wstring SavePath;

	CameraData CameraDatas;

	vector<MeshData> MeshDatas;

	vector<UINT> MeshIndices;
	vector<UINT16> MeshIndicesHalf;
	bool bUseHalfInt32;
};