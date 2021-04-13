#pragma once

#include "stdafx.h"
#include "DirectXColors.h"
using namespace DirectX;

namespace SampleAssets
{
	const UINT SamepleCount = 9;

	const std::string CameraBin = "CameraActor.bin";

	const std::string SampleResources[] = 
	{
		"BP_Chair.bin",
		"Linear_Stair_StaticMesh.bin",
		"Floor2.bin",
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
		"","","","","","","","",
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

	const SampleMaterial SampleBrick = { "wall", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.02f, 0.02f, 0.02f), 0.1f};
	const SampleMaterial SampleChair = { "chair", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f };
	const SampleMaterial SampleFloor = { "floor", XMFLOAT4(Colors::LightGray), XMFLOAT3(0.02f, 0.02f, 0.02f), 0.2f };

	const SampleMaterial SampeMats[] = 
	{
		SampleChair, 
		SampleBrick,
		SampleFloor,
		SampleBrick,
		SampleBrick,
		SampleBrick,
		SampleBrick,
		SampleBrick,
		SampleBrick,
	};
	
}