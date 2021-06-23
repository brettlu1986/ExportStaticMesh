
#include "pch.h"
#include "LSkeleton.h"

LSkeleton::LSkeleton()
:BoneNum(0)
{

}

LSkeleton::~LSkeleton()
{

}

void LSkeleton::CalculateRefPoseGlobalToLocal()
{
	vector<XMFLOAT4X4> RefPoseToRoot;
	RefPoseToRoot.resize(RefBonePoses.size());

	XMFLOAT4X4 M;
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR S = XMLoadFloat3(&RefBonePoses[0].Scale);
	XMVECTOR Q = XMLoadFloat4(&RefBonePoses[0].Quat);
	XMVECTOR T = XMLoadFloat3(&RefBonePoses[0].Translate);
	XMStoreFloat4x4(&RefPoseToRoot[0], XMMatrixAffineTransformation(S, zero, Q, T));

	for(size_t i = 1 ; i < RefBonePoses.size(); i++)
	{
		S = XMLoadFloat3(&RefBonePoses[i].Scale);
		Q = XMLoadFloat4(&RefBonePoses[i].Quat);
		T = XMLoadFloat3(&RefBonePoses[i].Translate);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));
		XMMATRIX ToParent = XMLoadFloat4x4(&M);

		INT32 ParentIndex = RefBoneInfos[i].ParentIndex;
		XMMATRIX ParentToRoot = XMLoadFloat4x4(&RefPoseToRoot[ParentIndex]);
		XMMATRIX ToRoot = XMMatrixMultiply(ToParent, ParentToRoot);
		XMStoreFloat4x4(&RefPoseToRoot[i], ToRoot);
	}

	RefRootToBoneTransforms.resize(RefPoseToRoot.size());
	for (size_t i = 0; i < RefPoseToRoot.size(); i++)
	{
		XMMATRIX Mat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&RefPoseToRoot[i]));
		XMStoreFloat4x4(&RefRootToBoneTransforms[i], Mat);
	}
}