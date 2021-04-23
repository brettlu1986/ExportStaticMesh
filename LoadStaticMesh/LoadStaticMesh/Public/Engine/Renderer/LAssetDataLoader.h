#pragma once

#include "stdafx.h"
#include "FMesh.h"
#include "LCamera.h"
#include "FScene.h"

class LAssetDataLoader
{
private:

	struct DirectionLightData
	{
		XMFLOAT3 Strength;
		XMFLOAT3 Direction;
		XMFLOAT3 Position;
	};


public:

	static void LoadMeshVertexDataFromFile(std::string FileName, FMesh* Mesh);

	static void LoadCameraDataFromFile(std::string FileName, LCamera& Camera);

	static void LoadSceneLights(std::string FileName, FScene* Scene);

	static std::wstring GetAssetFullPath(LPCWSTR AssetName);

	static void LoadSampleScene(FScene* Scene);
};