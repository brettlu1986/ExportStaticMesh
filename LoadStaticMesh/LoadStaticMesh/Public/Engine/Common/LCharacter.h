#pragma once

#include "stdafx.h"

class FSkeletalMesh;
class LAnimator;
class LCharacter
{
public:
	LCharacter();
	~LCharacter();

	FSkeletalMesh* SkeletalMesh;
	LAnimator* Animator;
};
