#pragma once

#include "stdafx.h"

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
	
}