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

	FSkeletalMesh* SkeletalMesh;
	LAnimator* Animator;
};
