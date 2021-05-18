

#include "LAnimator.h"

LAnimator::LAnimator()
:Skeleton(nullptr)
{

}

LAnimator::~LAnimator()
{
	
}

void LAnimator::SetSkeleton(LSkeleton* Ske)
{
	Skeleton = Ske;
	AllBoneFinalTransforms.resize(Skeleton->GetBoneCount());
	BoneMapFinalTransforms.resize(Skeleton->GetBoneMap().size());
}

void LAnimator::Play(std::string AnimName, bool bLoop)
{
	IsPlaying = true;
	CurrentPlay = AnimName;
	AnimSequences[CurrentPlay].SetIsPlay(true);
	AnimSequences[CurrentPlay].SetIsLoop(bLoop);
	AnimSequences[CurrentPlay].Reset();
}

void LAnimator::Stop()
{
	IsPlaying = false;
	AnimSequences[CurrentPlay].SetIsPlay(false);
}

void LAnimator::Update(float dt)
{
	if(!IsPlaying)
		return;

	AnimSequences[CurrentPlay].Update(dt);

	std::vector<XMFLOAT4X4>& CurrentAnimPoseToParentTrans = AnimSequences[CurrentPlay].GetCurrentAnimPoseToParentTrans();
	UINT BoneCount = Skeleton->GetBoneCount();
	std::vector<XMFLOAT4X4> CurrentAnimPoseToRootTrans(BoneCount);
	//root
	CurrentAnimPoseToRootTrans[0] = CurrentAnimPoseToParentTrans[0];
	for(UINT i = 1; i < BoneCount; i++)
	{
		XMMATRIX ToParent = XMLoadFloat4x4(&CurrentAnimPoseToParentTrans[i]);
		int ParentIndex = Skeleton->GetBoneInfos()[i].ParentIndex;
		XMMATRIX ParentToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[ParentIndex]);

		XMMATRIX ToRoot = XMMatrixMultiply(ToParent, ParentToRoot);
		XMStoreFloat4x4(&CurrentAnimPoseToRootTrans[i], ToRoot);
	}

	const std::vector<XMFLOAT4X4>& RefPoseRootToBoneTrans = Skeleton->GetRefPoseRootToBoneTransform();
	for (UINT i = 0; i < BoneCount; ++i)
	{
		XMMATRIX RefRootToBone = XMLoadFloat4x4(&RefPoseRootToBoneTrans[i]);
		XMMATRIX CurBoneToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(RefRootToBone, CurBoneToRoot);
		XMStoreFloat4x4(&AllBoneFinalTransforms[i], XMMatrixTranspose(finalTransform));
	}

	const std::vector<UINT16>& BoneMap = Skeleton->GetBoneMap();
	for(size_t i = 0; i < BoneMap.size(); i++)
	{
		UINT BoneIndex = BoneMap[i];
		BoneMapFinalTransforms[i] = AllBoneFinalTransforms[BoneIndex];
	}
}