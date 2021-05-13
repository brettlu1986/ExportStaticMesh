#pragma once

#include "stdafx.h"

class FSkeletalMesh;
class LAnimator;
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
