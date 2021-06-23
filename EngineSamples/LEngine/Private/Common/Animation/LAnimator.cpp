
#include "pch.h"
#include "LAnimator.h"
#include "LAnimationState.h"
#include "LEngine.h"
#include "LLog.h"
//#include <functional>
#include "LCharacter.h"
#include "LCharacterMovement.h"
#include "FSkeletalMesh.h"
#include "LSkeleton.h"

LAnimator::LAnimator()
:DefaultStateMachine(nullptr)
,OwnerCharacter(nullptr)
,Speed(0.f)
{

}

LAnimator::~LAnimator()
{
	if(DefaultStateMachine)
	{
		delete DefaultStateMachine;
		DefaultStateMachine = nullptr;
	}
}

void LAnimator::SetOwner(LCharacter* Owner)
{
	OwnerCharacter = Owner;
	LSkeleton* Skeleton = OwnerCharacter->GetSkeletalMesh()->GetSkeleton();
	AllBoneFinalTransforms.resize(Skeleton->GetBoneCount());
	BoneMapFinalTransforms.resize(OwnerCharacter->GetSkeletalMesh()->GetBoneMap().size());
}

void LAnimator::Update(float dt)
{
	// get movement speed change
	{
		float BaseSpeed = OwnerCharacter->GetBaseSpeed();
		Speed = OwnerCharacter->GetMoveSpeed();

		if(Speed == 0.f)
		{
			ProcessDefaultStateMachineChange(E_ANIM_STATE::IDLE);
		}
		else if(Speed == BaseSpeed)
		{
			ProcessDefaultStateMachineChange(E_ANIM_STATE::WALK);
		}
		else if(Speed > BaseSpeed)
		{
			ProcessDefaultStateMachineChange(E_ANIM_STATE::RUN);
		}

		//TODO:add just change

	}
	//default state machine update
	{
		DefaultStateMachine->Update(dt);

		//jump state
		//if(!DefaultStateMachine->IsTransition())
		//{
		//	LAnimationState* AnimState = DefaultStateMachine->GetCurrentAnimState();
		//	if (AnimState->GetStateType() == E_ANIM_STATE::JUMP_START)
		//	{
		//		if(AnimState->IsFinished())
		//		{
		//			DefaultStateMachine->SetTransitionToState(E_ANIM_STATE::JUMP_LOOP, 0.2f);
		//		}
		//	}
		//	else if(AnimState->GetStateType() == E_ANIM_STATE::JUMP_LOOP)
		//	{
		//		if (AnimState->IsFinished())
		//		{
		//			DefaultStateMachine->SetTransitionToState(E_ANIM_STATE::JUMP_END, 0.2f);
		//		}
		//	}
		//	else if(AnimState->GetStateType() == E_ANIM_STATE::JUMP_END)
		//	{
		//		if (AnimState->IsFinished())
		//		{
		//			DefaultStateMachine->SetTransitionToState(E_ANIM_STATE::IDLE, 0.2f);
		//		}
		//	}
		//}
	}

	// calculate bone map final transforms according to current pose
	{
		vector<LAnimBonePose>& CurrentAnimPoseToParentTrans = DefaultStateMachine->GetCurrentAnimPoseToParentTrans();

		LSkeleton* Skeleton = OwnerCharacter->GetSkeletalMesh()->GetSkeleton();
		UINT BoneCount = Skeleton->GetBoneCount();
		vector<XMFLOAT4X4> CurrentAnimPoseToRootTrans(BoneCount);
		//root
		XMVECTOR Zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		//XMStoreFloat4x4(&CurrentAnimPoseToRootTrans[0], XMMatrixAffineTransformation(CurrentAnimPoseToParentTrans[0].S, Zero,
		//	CurrentAnimPoseToParentTrans[0].Q, CurrentAnimPoseToParentTrans[0].T));

		XMVECTOR RefT = XMLoadFloat3(&Skeleton->GetRefBonPoses()[0].Translate);
		XMStoreFloat4x4(&CurrentAnimPoseToRootTrans[0], XMMatrixAffineTransformation(CurrentAnimPoseToParentTrans[0].S, Zero,
				CurrentAnimPoseToParentTrans[0].Q, RefT));
		for (UINT i = 1; i < BoneCount; i++)
		{
			//	XMMATRIX ToParent = XMMatrixAffineTransformation(CurrentAnimPoseToParentTrans[i].S, Zero,
				//	CurrentAnimPoseToParentTrans[i].Q, CurrentAnimPoseToParentTrans[i].T);
			RefT = XMLoadFloat3(&Skeleton->GetRefBonPoses()[i].Translate);
			XMMATRIX ToParent = XMMatrixAffineTransformation(CurrentAnimPoseToParentTrans[i].S, Zero,
				CurrentAnimPoseToParentTrans[i].Q, RefT);
			int ParentIndex = Skeleton->GetBoneInfos()[i].ParentIndex;
			XMMATRIX ParentToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[ParentIndex]);

			XMMATRIX ToRoot = XMMatrixMultiply(ToParent, ParentToRoot);
			XMStoreFloat4x4(&CurrentAnimPoseToRootTrans[i], ToRoot);
		}

		const vector<XMFLOAT4X4>& RefPoseRootToBoneTrans = Skeleton->GetRefPoseRootToBoneTransform();
		for (UINT i = 0; i < BoneCount; ++i)
		{
			XMMATRIX RefRootToBone = XMLoadFloat4x4(&RefPoseRootToBoneTrans[i]);
			XMMATRIX CurBoneToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[i]);
			XMMATRIX finalTransform = XMMatrixMultiply(RefRootToBone, CurBoneToRoot);
			XMStoreFloat4x4(&AllBoneFinalTransforms[i], XMMatrixTranspose(finalTransform));
		}

		const vector<UINT16>& BoneMap = OwnerCharacter->GetSkeletalMesh()->GetBoneMap();
		for (size_t i = 0; i < BoneMap.size(); i++)
		{
			UINT BoneIndex = BoneMap[i];
			BoneMapFinalTransforms[i] = AllBoneFinalTransforms[BoneIndex];
		}


	}
}

void LAnimator::InitStateMachines()
{
	CreateDefaultStateMachine();
}

void LAnimator::CreateDefaultStateMachine()
{
	DefaultStateMachine = new LAnimationStateMachine();
	map<E_ANIM_STATE, LAnimationState*> Stats;
	Stats[E_ANIM_STATE::IDLE] = new LAnimationState(E_ANIM_STATE::IDLE, true, &AnimSequences[E_ANIM_STATE::IDLE]);
	Stats[E_ANIM_STATE::WALK] = new LAnimationState(E_ANIM_STATE::WALK, true, &AnimSequences[E_ANIM_STATE::WALK]);
	Stats[E_ANIM_STATE::RUN] = new LAnimationState(E_ANIM_STATE::RUN, true, &AnimSequences[E_ANIM_STATE::RUN]);
	Stats[E_ANIM_STATE::JUMP_START] = new LAnimationState(E_ANIM_STATE::JUMP_START, false, &AnimSequences[E_ANIM_STATE::JUMP_START]);
	Stats[E_ANIM_STATE::JUMP_LOOP] = new LAnimationState(E_ANIM_STATE::JUMP_LOOP, false, &AnimSequences[E_ANIM_STATE::JUMP_LOOP]);
	Stats[E_ANIM_STATE::JUMP_END] = new LAnimationState(E_ANIM_STATE::JUMP_END, false, &AnimSequences[E_ANIM_STATE::JUMP_END]);
	DefaultStateMachine->OnCreate(Stats, E_ANIM_STATE::IDLE);
}

void LAnimator::ProcessDefaultStateMachineChange(E_ANIM_STATE State)
{
	
	if(State == DefaultStateMachine->GetCurrentAnimStateType())
	{
		return;
	}

	//jump state
	//if(State == E_ANIM_STATE::IDLE && (DefaultStateMachine->GetCurrentAnimStateType() == E_ANIM_STATE::JUMP_START ||
	//	DefaultStateMachine->GetCurrentAnimStateType() == E_ANIM_STATE::JUMP_LOOP ||
	//	DefaultStateMachine->GetCurrentAnimStateType() == E_ANIM_STATE::JUMP_END))
	//{
	//	return;
	//}

	DefaultStateMachine->SetTransitionToState(State, 0.2f);
}