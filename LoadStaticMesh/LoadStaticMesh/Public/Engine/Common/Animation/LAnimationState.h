#pragma once
#include "stdafx.h"
#include "LAnimationSequence.h"
#

class LAnimationState
{
public:
	LAnimationState(std::string InName, bool bInLoop, LAnimationSequence* Seq);
	~LAnimationState();

	void Init();
	void Update(float dt);

	std::vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans();

	std::string& GetName() 
	{
		return Name;
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
	std::string Name; 
	LAnimationSequence* StateAnim;
	bool bLoop;
};