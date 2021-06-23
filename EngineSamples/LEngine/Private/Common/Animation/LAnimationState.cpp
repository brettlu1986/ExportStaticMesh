#include "pch.h"
#include "LAnimationState.h"

LAnimationState::LAnimationState(E_ANIM_STATE State, bool bInLoop, LAnimationSequence* Seq)
:StateType(State)
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

void LAnimationState::Init(LAnimationStateTransition* PreTransition)
{
	StateAnim->SetIsPlay(true);
	StateAnim->SetIsLoop(bLoop);
	StateAnim->Reset();
	//if(PreTransition != nullptr)
	//	StateAnim->SetPreTransitionEndSeq(PreTransition->GetTransitionAnimPoseToParentTrans());
}

vector<LAnimBonePose>& LAnimationState::GetCurrentAnimPoseToParentTrans()
{
	return StateAnim->GetCurrentAnimPoseToParentTrans();
}

