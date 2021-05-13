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
	INT32 ParentIndex;
};

class LSkeleton
{
public: 
	LSkeleton();
	~LSkeleton();

	void SetBoneCount(UINT BoneCount)
	{
		BoneNum = BoneCount;
	}

	UINT GetBoneCount() const 
	{
		return BoneNum;
	}

	UINT GetBoneIndex(std::string& Name) 
	{
		return BoneNameToIndex[Name];
	}

	void AddBoneInfo(LBoneInfo Info)
	{
		RefBoneInfos.push_back(Info);
	}

	void AddBonePose(LBonePose Pose)
	{
		RefBonePoses.push_back(Pose);
	}

	void SetNameToIndexValue(std::string Name, INT32 Value)
	{
		BoneNameToIndex[Name] = Value;
	}
private:

	UINT BoneNum;
	std::map<std::string, INT32> BoneNameToIndex;
	std::vector<LBonePose> RefBonePoses; // root -> last child
	std::vector<LBoneInfo> RefBoneInfos; // root -> last child
};
