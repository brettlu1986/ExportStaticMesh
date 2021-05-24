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

	void AddAnimSequence(std::string AnimName, LAnimationSequence Seq)
	{
		AnimSequences[AnimName] = Seq;
	}

	void Update(float dt);

	std::vector<XMFLOAT4X4>& GetBoneMapFinalTransforms()
	{
		return BoneMapFinalTransforms;
	}

	void InitStateMachines();
	void ProcessDefaultStateMachineChange(std::string name);

	std::string& GetCurrentAnimStateName()
	{
		return DefaultStateMachine->GetCurrentAnimStateName();
	}

private: 
	void CreateDefaultStateMachine();
	std::map<std::string, LAnimationSequence> AnimSequences;
	LSkeleton* Skeleton;
	LCharacter* OwnerCharacter;

	std::vector<XMFLOAT4X4> AllBoneFinalTransforms;
	std::vector<XMFLOAT4X4> BoneMapFinalTransforms; //use to constant buffer

	LAnimationStateMachine* DefaultStateMachine;
};
