#include "LAnimationState.h"

LAnimationState::LAnimationState(std::string InName, bool bInLoop, LAnimationSequence* Seq)
:Name(InName)
,bLoop(bInLoop)
,StateAnim(Seq)
{

}

LAnimationState::~LAnimationState()
{

}

void LAnimationState::Update(float dt)
{
	StateAnim->Update(dt);
}

void LAnimationState::Init()
{
	StateAnim->SetIsPlay(true);
	StateAnim->SetIsLoop(bLoop);
	StateAnim->Reset();
}

std::vector<LAnimBonePose>& LAnimationState::GetCurrentAnimPoseToParentTrans()
{
	return StateAnim->GetCurrentAnimPoseToParentTrans();
}

