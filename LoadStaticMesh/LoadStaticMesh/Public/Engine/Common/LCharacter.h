#pragma once

#include "stdafx.h"
#include "LAnimator.h"

class FSkeletalMesh;
class LCharacter
{
public:
	LCharacter();
	~LCharacter();

	void SetSkeletalMesh(FSkeletalMesh* Mesh)
	{
		SkeletalMesh= Mesh;
	}

	void SetAnimator(LAnimator* Animator)
	{
		AnimatorIns = Animator;
	}

private: 

	FSkeletalMesh* SkeletalMesh;
	LAnimator* AnimatorIns;
};
