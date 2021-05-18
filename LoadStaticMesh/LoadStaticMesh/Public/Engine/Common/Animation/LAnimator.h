#pragma once

#include "stdafx.h"
#include "LSkeleton.h"
#include "LAnimationSequence.h"

class LAnimator
{
public: 
	LAnimator();
	~LAnimator();

	void SetSkeleton(LSkeleton* Ske);
	

	void AddAnimSequence(std::string AnimName, LAnimationSequence Seq)
	{
		AnimSequences[AnimName] = Seq;
	}

	void Play(std::string AnimName, bool bLoop);
	void Stop();
	void Update(float dt);

	std::vector<XMFLOAT4X4>& GetBoneMapFinalTransforms()
	{
		return BoneMapFinalTransforms;
	}
private: 

	std::map<std::string, LAnimationSequence> AnimSequences;
	LSkeleton* Skeleton;
	bool IsPlaying;
	std::string CurrentPlay;

	std::vector<XMFLOAT4X4> AllBoneFinalTransforms;
	std::vector<XMFLOAT4X4> BoneMapFinalTransforms; //use to constant buffer
};
