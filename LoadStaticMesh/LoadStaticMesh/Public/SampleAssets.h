#pragma once

#include "stdafx.h"
#include "DirectXColors.h"
#include "FScene.h"

#include "LScene.h"

namespace SampleAssets
{
	// assets
	//skeleton
	const UINT DDSTexResourcesCount = 2;
	struct BaseAssetRefInfo
	{
		std::string RefName;
		std::string FileName;
	};
	const BaseAssetRefInfo SkeletonResource = {"BaseSke", "ThirdPersonCharacter_SK.bin"};
	//shader
	const UINT ShaderResCount = 8;
	struct ShaderAssetRefInfo
	{
		std::string RefName;
		LPCWSTR FileName;
	};
	const ShaderAssetRefInfo ShaderResources[] =
	{
		{"ShaderTexVs",			L"ShaderTexVs.cso" },
		{"ShaderTexPs",			L"ShaderTexPs.cso" },
		{"ShaderVs",			L"ShaderVs.cso"},
		{"ShaderPs",			L"ShaderPs.cso"},
		{"SampleDepthShaderVs", L"SampleDepthShaderVs.cso"},
		{"SampleDepthShaderPs", L"SampleDepthShaderPs.cso"},
		{"SkeletalVs",			L"SkeletalVs.cso"},
		{"SkeletalPs",			L"SkeletalPs.cso"},
	};

	//material
	const BaseAssetRefInfo DDSTexResources[] = {
		{"MaleDiffuse", "Resource/T_Male_Mask.dds"},
		{"ChairDiffuse", "Resource/T_Chair_M.dds"}
	};

	const UINT SamepleCount = 10;
	const string SampleResources[] = 
	{
		"BP_Chair.bin",
		"Linear_Stair_StaticMesh.bin",
		"Sphere.bin",
		"Floor.bin",
		"Bump_StaticMesh.bin",
		"LeftArm_StaticMesh.bin",
		"Wall6.bin",
		"Wall7.bin",
		"Wall8.bin",
		"Wall9.bin",
	};

	const string CameraBin = "CameraActor.bin";

	//light
	const string SceneLightsFile = "SceneLights.bin";

	//skeletal 
	const UINT SampleSkeletalMeshCount = 3;
	const string SkeletalMeshResource[] =
	{
		"ThirdPersonCharacter_SKM.bin",
		"ThirdPersonCharacter2_SKM.bin",
		"ThirdPersonCharacter3_SKM.bin",
	};
	
	//animation
	const string SkeletalAnim[] = 
	{	
		"ThirdPersonCharacter_ThirdPersonIdle.bin",
		"ThirdPersonCharacter_ThirdPersonWalk.bin",
		"ThirdPersonCharacter_ThirdPersonRun.bin",
		"ThirdPersonCharacter_ThirdPersonJump_Start.bin",
		"ThirdPersonCharacter_ThirdPersonJump_Loop.bin",
		"ThirdPersonCharacter_ThirdPerson_Jump.bin",
	};

	void LoadSampleSceneData(LScene& Scene);
	
}