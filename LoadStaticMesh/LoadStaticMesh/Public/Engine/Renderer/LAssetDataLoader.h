#pragma once

#include "stdafx.h"
#include "FMesh.h"
#include "LCamera.h"

class LAssetDataLoader
{
public:

	static void LoadMeshVertexDataFromFile(std::string FileName, FMesh* Mesh);

	static void LoadCameraDataFromFile(std::string FileName, LCamera& Camera);

	static std::wstring GetAssetFullPath(LPCWSTR AssetName);
};