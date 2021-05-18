#pragma once

#include "stdafx.h"
#include "LAnimator.h"

class FSkeletalMesh;
class LCharacter
{
public:
	LCharacter();
	~LCharacter();

	void Destroy();

	void SetSkeletalMesh(FSkeletalMesh* Mesh)
	{
		SkeletalMesh = Mesh;
	}

	void SetAnimator(LAnimator* Animator)
	{
		AnimatorIns = Animator;
	}

	FSkeletalMesh* GetSkeletalMesh()
	{
		return SkeletalMesh;
	}

	void PlayAnimation(std::string AnimName, bool bLoop)
	{
		AnimatorIns->Play(AnimName, bLoop);
	}

private: 

	FSkeletalMesh* SkeletalMesh;
	LAnimator* AnimatorIns;
};
