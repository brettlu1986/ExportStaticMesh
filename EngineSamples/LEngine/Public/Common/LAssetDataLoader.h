#pragma once

#include "pch.h"
#include "LCamera.h"
#include "LAnimationSequence.h"

#include "LSkeletalMesh.h"
#include "LMesh.h"
#include "LTexture.h"
#include "LShader.h"
#include "LMaterial.h"
#include "LMaterialInstance.h"
#include "LAssetManager.h"

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
		Vec3 Scale;
		Vec3 Rotate;
		Vec3 Translate;
	};

	struct BoneNameIndex
	{
		char Name[64];
		INT32 Index;
	};
	
public:

	LENGINE_API static void LoadAssetsFromFile(string FileName, vector<LAssetDef>& Skeletons, vector<LAssetDef>& ShaderFiles, vector<LAssetDef>& Textures,
		vector<LAssetDef>& Materials, vector<LAssetDef>& MaterialInstances, vector<LAssetDef>& Animations,
		vector<LAssetDef>& MeshGeometries, vector<LAssetDef>& SkeletalMeshGeometries);

	LENGINE_API static void LoadMap(string MapFile, vector<LMapStaticObjInfo>& MapStaticObjInfos, vector<LMapSkeletalObjInfo>& MapSkeletalObjInfos, LCameraData& CameraData, vector<LDirectionLightData>& LightsData);

	LENGINE_API static void LoadSkeletonFromFile(string FileName, LSkeleton* Skeleton);

	LENGINE_API static void LoadDDSTextureFromFile(string FileName, LTexture* Texture);

	LENGINE_API static void LoadShaderFromeFile(string FileName, LShader* Shader);

	LENGINE_API static void LoadMaterial(string FileName, LMaterial* Material);

	LENGINE_API static void LoadMaterialInstance(string FileName, LMaterialInstance* MaterialIns);

	LENGINE_API static void LoadAnimationSquence(string SequenceName, LAnimationSequence* Seq);

	LENGINE_API static void LoadDirectionLights(string FileName, vector<LDirectionLightData>&LightsData);

	LENGINE_API static wstring GetAssetFullPath(LPCWSTR AssetName);

	LENGINE_API static void LoadSkeletalMeshVertexDataFromFile(string FileName, LSkeletalMeshBuffer* SkeletalMeshBuffer);

	LENGINE_API static void LoadMeshFromFile(string FileName, LStaticMeshBuffer* MeshBuffer);

	LENGINE_API static void LoadCameraDataFromFile(string FileName, LCameraData& CameraData);

};