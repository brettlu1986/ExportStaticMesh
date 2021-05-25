#pragma once

#include "stdafx.h"
#include "DirectXColors.h"
using namespace DirectX;

namespace SampleAssets
{
	const UINT SamepleCount = 10;

	const std::string CameraBin = "CameraActor.bin";

	const std::string SampleResources[] = 
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

	const std::string SampleResourceTexture[] = 
	{
		"Resource/T_Chair_M.dds",
		"","","","","","","","",""
	};

	const std::string PsoUseTexture = "PsoUseTexture";
	const std::string PsoNoTexture = "PsoNoTexture";

	struct SampleMaterial 
	{
		std::string Name;
		XMFLOAT4 DiffuseAlbedo;
		XMFLOAT3 FresnelR0;
		float Roughness;
	};

	const SampleMaterial SampeMats[] = 
	{
		{ "Chair", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.1f},
		{ "Stair", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.05f},
		{ "Sphere", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.02f},
		{ "Floor2", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.02f},
		{ "Bump", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.2f},
		{ "LeftArm", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.3f, 0.3f, 0.3f), 0.4f},
		{ "Wall6", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.15f},
		{ "Wall7", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.12f},
		{ "Wall8", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.3f},
		{ "Wall9", XMFLOAT4(Colors::DarkGray), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.2f},
	};

	const std::string SceneLightsFile = "SceneLights.bin";

	const UINT SampleSkeletalMeshCount = 3;
	const std::string SkeletalMeshResource[] =
	{
		"ThirdPersonCharacter_SKM.bin",
		"ThirdPersonCharacter2_SKM.bin",
		"ThirdPersonCharacter3_SKM.bin",
	};
	const std::string SkeletonResource = "ThirdPersonCharacter_SK.bin";
	const std::string SkeletonTexture = "Resource/T_Male_Mask.dds";
	
	const std::string SkeletalAnim[] = 
	{	
		"ThirdPersonCharacter_ThirdPersonIdle.bin",
		"ThirdPersonCharacter_ThirdPersonWalk.bin",
		"ThirdPersonCharacter_ThirdPersonRun.bin",
		"ThirdPersonCharacter_ThirdPersonJump_Start.bin",
		"ThirdPersonCharacter_ThirdPersonJump_Loop.bin",
		"ThirdPersonCharacter_ThirdPerson_Jump.bin",
	};
}