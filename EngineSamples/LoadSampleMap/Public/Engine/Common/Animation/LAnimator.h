#pragma once

#include "stdafx.h"

#include "LAnimationSequence.h"
#include "LAnimationStateMachine.h"

class LCharacter;
class LAnimator
{
public: 
	LAnimator();
	~LAnimator();

	void SetOwner(LCharacter* Owner);

	void AddAnimSequence(E_ANIM_STATE State, LAnimationSequence Seq)
	{
		AnimSequences[State] = Seq;
	}

	void Update(float dt);

	vector<XMFLOAT4X4>& GetBoneMapFinalTransforms()
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
	map<E_ANIM_STATE, LAnimationSequence> AnimSequences;

	LCharacter* OwnerCharacter;

	vector<XMFLOAT4X4> AllBoneFinalTransforms;
	vector<XMFLOAT4X4> BoneMapFinalTransforms; //use to constant buffer

	LAnimationStateMachine* DefaultStateMachine;

	float Speed;
};
