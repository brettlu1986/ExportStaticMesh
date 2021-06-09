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
	const BaseAssetRefInfo SkeletonResource = {"BaseSke", "SkeletalMeshes/ThirdPersonCharacter_SK.bin"};

	//shader
	const UINT ShaderResCount = 8;
	const BaseAssetRefInfo ShaderResources[] =
	{
		{"ShaderTexVs",			"ShaderTexVs.cso" },
		{"ShaderTexPs",			"ShaderTexPs.cso" },
		{"ShaderVs",			"ShaderVs.cso"},
		{"ShaderPs",			"ShaderPs.cso"},
		{"SampleDepthShaderVs", "SampleDepthShaderVs.cso"},
		{"SampleDepthShaderPs", "SampleDepthShaderPs.cso"},
		{"SkeletalVs",			"SkeletalVs.cso"},
		{"SkeletalPs",			"SkeletalPs.cso"},
	};

	//material
	const BaseAssetRefInfo DDSTexResources[] = {
		{"MaleColor", "Textures/T_Male_Mask.dds"},
		{"ChairColor", "Textures/T_Chair_M.dds"}
	};

	const UINT SamepleCount = 16;
	const string SampleResources[] = 
	{
		"StaticMeshes/BP_Chair.bin",
		"StaticMeshes/Linear_Stair_StaticMesh.bin",
		"StaticMeshes/Sphere.bin",
		"StaticMeshes/Floor.bin",
		"StaticMeshes/Bump_StaticMesh.bin",
		"StaticMeshes/LeftArm_StaticMesh.bin",
		"StaticMeshes/Wall6.bin",
		"StaticMeshes/Wall7.bin",
		"StaticMeshes/Wall8.bin",
		"StaticMeshes/Wall9.bin",
		"StaticMeshes/BP_Chair2.bin",
		"StaticMeshes/BP_Chair3.bin",
		"StaticMeshes/BP_Chair4.bin",
		"StaticMeshes/BP_Rock.bin",
		"StaticMeshes/BP_TableRound.bin",
		"StaticMeshes/BP_Lamp_Ceiling.bin"
	};

	const string CameraBin = "Cameras/CameraActor.bin";

	//light
	const string SceneLightsFile = "Lights/SceneLights.bin";

	//skeletal 
	const UINT SampleSkeletalMeshCount = 3;
	const string SkeletalMeshResource[] =
	{
		"SkeletalMeshes/ThirdPersonCharacter_SKM.bin",
		"SkeletalMeshes/ThirdPersonCharacter_SKM.bin",
		"SkeletalMeshes/ThirdPersonCharacter_SKM.bin",
	};
	
	//animation
	const string SkeletalAnim[] = 
	{	
		"Animations/ThirdPersonCharacter_ThirdPersonIdle.bin",
		"Animations/ThirdPersonCharacter_ThirdPersonWalk.bin",
		"Animations/ThirdPersonCharacter_ThirdPersonRun.bin",
		"Animations/ThirdPersonCharacter_ThirdPersonJump_Start.bin",
		"Animations/ThirdPersonCharacter_ThirdPersonJump_Loop.bin",
		"Animations/ThirdPersonCharacter_ThirdPerson_Jump.bin",
	};

	void LoadSampleSceneData(LScene& Scene);
	
}