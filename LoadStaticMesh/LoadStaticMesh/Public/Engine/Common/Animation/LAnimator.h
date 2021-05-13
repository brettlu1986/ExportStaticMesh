#pragma once

#include "stdafx.h"
#include "LSkeleton.h"
#include "LAnimationSequence.h"

class LAnimator
{
public: 
	LAnimator();
	~LAnimator();

	void SetSkeleton(LSkeleton* Ske)
	{
		Skeleton = Ske;
	}

	void AddAnimSequence(std::string AnimName, LAnimationSequence Seq)
	{
		AnimSequences[AnimName] = Seq;
	}

private: 

	std::map<std::string, LAnimationSequence> AnimSequences;
	LSkeleton* Skeleton;
};
