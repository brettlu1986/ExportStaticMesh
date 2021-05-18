

#include "LAnimator.h"

LAnimator::LAnimator()
:Skeleton(nullptr)
{

}

LAnimator::~LAnimator()
{
	
}

void LAnimator::Play(std::string AnimName, bool bLoop)
{
	IsPlaying = true;
	CurrentPlay = AnimName;
	AnimSequences[CurrentPlay].SetIsLoop(bLoop);
}

void LAnimator::Stop()
{
	IsPlaying = false;
}

void LAnimator::Update(float dt)
{
	if(!IsPlaying)
		return;

	AnimSequences[CurrentPlay].Update(dt);
}