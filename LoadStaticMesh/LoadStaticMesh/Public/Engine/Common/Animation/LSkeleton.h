#pragma once

#include "stdafx.h"

using namespace DirectX;

struct LBonePose
{
	XMFLOAT3 Scale;
	XMFLOAT4 Quat;
	XMFLOAT3 Translate;
};

struct LBoneInfo
{
	std::string BoneName;
	UINT ParentIndex;
};

class LSkeleton
{
public: 
	LSkeleton();
	~LSkeleton();

	UINT GetBoneIndex(std::string& Name) 
	{
		return BoneNameToIndex[Name];
	}

private:
	std::map<std::string, UINT> BoneNameToIndex;
	std::vector<LBonePose> RefBonePoses; // root -> last child
	std::vector<LBoneInfo> RefBoneInfos; // root -> last child
};
