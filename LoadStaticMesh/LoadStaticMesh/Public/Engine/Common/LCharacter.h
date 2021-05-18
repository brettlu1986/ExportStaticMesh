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

	LAnimator* GetAnimator()
	{
		return AnimatorIns;
	}

	void PlayAnimation(std::string AnimName, bool bLoop)
	{
		AnimatorIns->Play(AnimName, bLoop);
	}

	void Update(float dt);
private: 

	FSkeletalMesh* SkeletalMesh;
	LAnimator* AnimatorIns;
};
