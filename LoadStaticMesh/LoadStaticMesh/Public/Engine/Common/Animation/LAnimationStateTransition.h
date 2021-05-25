#pragma once
#include "stdafx.h"
#include "LAnimationSequence.h"

class LAnimationStateTransition
{
public: 
	LAnimationStateTransition();
	~LAnimationStateTransition();

	void Update(float dt);
	void OnCreate(LAnimationSequence* From, LAnimationSequence* To, float Time);

	bool IsTransitionComplete()
	{
		return bComplete;
	}

private: 
	LAnimationSequence* TransitionFrom;
	LAnimationSequence* TransitionTo;
	float TransitionTime;

	float TransitionElapsed;
	bool bComplete;

};