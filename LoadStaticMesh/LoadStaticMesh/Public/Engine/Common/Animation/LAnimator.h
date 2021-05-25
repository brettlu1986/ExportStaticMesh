#pragma once

#include "stdafx.h"
#include "LSkeleton.h"
#include "LAnimationSequence.h"
#include "LAnimationStateMachine.h"

class LCharacter;
class LAnimator
{
public: 
	LAnimator();
	~LAnimator();

	void SetSkeleton(LSkeleton* Ske);
	void SetOwner(LCharacter* Owner)
	{
		OwnerCharacter = Owner;
	}

	void AddAnimSequence(E_ANIM_STATE State, LAnimationSequence Seq)
	{
		AnimSequences[State] = Seq;
	}

	void Update(float dt);

	std::vector<XMFLOAT4X4>& GetBoneMapFinalTransforms()
	{
		return BoneMapFinalTransforms;
	}

	void InitStateMachines();
	void ProcessDefaultStateMachineChange(E_ANIM_STATE State);

	E_ANIM_STATE GetCurrentAnimStateType()
	{
		return DefaultStateMachine->GetCurrentAnimStateType();
	}

private: 
	void CreateDefaultStateMachine();
	std::map<E_ANIM_STATE, LAnimationSequence> AnimSequences;

	LSkeleton* Skeleton;
	LCharacter* OwnerCharacter;

	std::vector<XMFLOAT4X4> AllBoneFinalTransforms;
	std::vector<XMFLOAT4X4> BoneMapFinalTransforms; //use to constant buffer

	LAnimationStateMachine* DefaultStateMachine;

	float Speed;
};
