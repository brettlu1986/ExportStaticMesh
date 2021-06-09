#pragma once

#include "stdafx.h"
#include "LCamera.h"
#include "LAnimationSequence.h"

#include "LSkeletalMesh.h"
#include "LMesh.h"
#include "LTexture.h"
#include "LShader.h"

struct DirectionLightData
{
	XMFLOAT3 Strength;
	XMFLOAT3 Direction;
	XMFLOAT3 Position;
};

class LAssetDataLoader
{
private:

	struct BoneInfo
	{
		char Name[64];
		INT32 ParentIndex;
	};

	struct BonePose
	{
		XMFLOAT3 Scale;
		XMFLOAT3 Rotate;
		XMFLOAT3 Translate;
	};

	struct BoneNameIndex
	{
		char Name[64];
		INT32 Index;
	};
	
	
public:

	static void LoadSkeletonFromFile(string FileName, LSkeleton* Skeleton);

	static void LoadDDSTextureFromFile(string FileName, LTexture* Texture);

	static void LoadShaderFromeFile(string FileName, LShader* Shader);

	static void LoadAnimationSquence(string SequenceName, LAnimationSequence& Seq);

	static void LoadDirectionLights(string FileName, vector<DirectionLightData>&LightsData);

	static wstring GetAssetFullPath(LPCWSTR AssetName);

	static void LoadSkeletalMeshVertexDataFromFile(string FileName, LSkeletalMesh& SkeletalMesh, XMFLOAT3& Location, XMFLOAT3& Rotation, XMFLOAT3& Scale);

	static void LoadMeshFromFile(string FileName, LMesh& Mesh, XMFLOAT3& Location, XMFLOAT3& Rotation, XMFLOAT3& Scale);

	static void LoadCameraDataFromFile(string FileName, LCamera& Camera);

};