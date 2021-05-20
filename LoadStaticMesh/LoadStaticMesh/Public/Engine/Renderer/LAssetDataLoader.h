#pragma once

#include "stdafx.h"
#include "FMesh.h"
#include "LCamera.h"
#include "FScene.h"
#include "FSkeletalMesh.h"
#include "LAnimationSequence.h"

class LAssetDataLoader
{
private:

	struct DirectionLightData
	{
		XMFLOAT3 Strength;
		XMFLOAT3 Direction;
		XMFLOAT3 Position;
	};

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

	static void LoadMeshVertexDataFromFile(std::string FileName, FMesh* Mesh);

	static void LoadSkeletalMeshVertexDataFromFile(std::string FileName, FSkeletalMesh* SkeletalMesh, LSkeleton* Skeleton);

	static void LoadSkeletonFromFile(std::string FileName, LSkeleton* Skeleton);

	static void LoadAnimationSquence(std::string SequenceName, LAnimationSequence& Seq);

	static void LoadCameraDataFromFile(std::string FileName, LCamera* Camera);

	static void LoadSceneLights(std::string FileName, FScene* Scene);

	static std::wstring GetAssetFullPath(LPCWSTR AssetName);

	static void LoadSampleScene(FScene* Scene);
};