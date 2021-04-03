#pragma once

#include "stdafx.h"
#include "FMesh.h"
#include "LCamera.h"

class LAssetDataLoader
{
public:

	static void LoadMeshVertexDataFromFile(LPCWSTR FileName, FIndexBuffer& IndexBuffer, FVertexBuffer& VertexBuffer);

	static void LoadCameraDataFromFile(LPCWSTR FileName, LCamera& Camera);
};