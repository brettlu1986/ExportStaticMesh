#pragma once
#include "stdafx.h"
#include "LAnimationSequence.h"
#

class LAnimationState
{
public:
	LAnimationState(E_ANIM_STATE State, bool bInLoop, LAnimationSequence* Seq);
	~LAnimationState();

	void Init();
	void Update(float dt);

	std::vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans();

	E_ANIM_STATE GetStateType()
	{
		return StateType;
	}

	LAnimationSequence* GetAnimSeq()
	{
		return StateAnim;
	}

	bool IsFinished()
	{
		return !bLoop && StateAnim->IsAnimationSequenceFinished();
	}
private:
	E_ANIM_STATE StateType;
	LAnimationSequence* StateAnim;
	bool bLoop;
};