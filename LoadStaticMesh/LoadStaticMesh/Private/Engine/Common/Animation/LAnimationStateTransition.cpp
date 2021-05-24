
#include "LAnimationStateTransition.h"

LAnimationStateTransition::LAnimationStateTransition()
{

}

LAnimationStateTransition::~LAnimationStateTransition()
{

}

void LAnimationStateTransition::OnCreate(LAnimationSequence* From, LAnimationSequence* To, float Time)
{
	TransitionFrom = From;
	TransitionTo = To;
	TransitionTime = Time;
	TransitionElapsed = 0.f;
	bComplete = false;
}

void LAnimationStateTransition::Update(float dt)
{
	if (bComplete)
		return;

	TransitionElapsed += dt;
	if(TransitionElapsed > TransitionTime)
		bComplete = true;


}

