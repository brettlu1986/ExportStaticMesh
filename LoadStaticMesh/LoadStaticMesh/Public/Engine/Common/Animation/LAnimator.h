#pragma once

#include "stdafx.h"
#include "LSkeleton.h"
#include "LAnimationSequence.h"

class LAnimator
{
public: 
	LAnimator();
	~LAnimator();

	std::map<std::string, LAnimationSequence> AnimSequences;
	LSkeleton Skeleton;
};
