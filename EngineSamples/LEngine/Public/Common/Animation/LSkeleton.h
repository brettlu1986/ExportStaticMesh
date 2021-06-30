#pragma once

#include "pch.h"


struct LBonePose
{
	Vec3 Scale;
	XMFLOAT4 Quat;
	Vec3 Translate;
};

struct LAnimBonePose
{
	XMVECTOR S;
	XMVECTOR Q;
	XMVECTOR T;
};

struct LBoneInfo
{
	string BoneName;
	INT32 ParentIndex;
};

class LENGINE_API LSkeleton
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

	UINT GetBoneIndex(string& Name) 
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

	void SetNameToIndexValue(string Name, INT32 Value)
	{
		BoneNameToIndex[Name] = Value;
	}

	void CalculateRefPoseGlobalToLocal();

	const vector<XMFLOAT4X4>& GetRefPoseRootToBoneTransform()
	{
		return RefRootToBoneTransforms;
	}
	 
	const vector<LBoneInfo>& GetBoneInfos()
	{
		return RefBoneInfos;
	}

	const vector<LBonePose>& GetRefBonPoses()
	{
		return RefBonePoses;
	}
private:

	UINT BoneNum;
	map<string, INT32> BoneNameToIndex;
	vector<LBonePose> RefBonePoses; // root -> last child
	vector<LBoneInfo> RefBoneInfos; // root -> last child

	vector<XMFLOAT4X4> RefRootToBoneTransforms;
};
