#pragma once

#include "stdafx.h"
#include "FMesh.h"
#include "LCamera.h"
#include "FScene.h"
#include "FSkeletalMesh.h"
#include "LAnimationSequence.h"

#include "LMesh.h"

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

	static void LoadMeshVertexDataFromFile(string FileName, FMesh* Mesh);

	static void LoadSkeletalMeshVertexDataFromFile(string FileName, FSkeletalMesh* SkeletalMesh);

	static void LoadSkeletonFromFile(string FileName, LSkeleton* Skeleton);

	static void LoadAnimationSquence(string SequenceName, LAnimationSequence& Seq);

	static void LoadCameraDataFromFile(string FileName, LCamera* Camera);

	static void LoadDirectionLights(string FileName, vector<DirectionLightData>&LightsData);

	static wstring GetAssetFullPath(LPCWSTR AssetName);

	static void LoadMeshFromFile(string FileName, LMesh& Mesh);

};