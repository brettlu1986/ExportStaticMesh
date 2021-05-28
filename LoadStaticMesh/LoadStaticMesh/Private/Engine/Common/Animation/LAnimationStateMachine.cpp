#include "LAnimationStateMachine.h"


LAnimationStateMachine::LAnimationStateMachine()
:CurrentAnimState(nullptr)
,Transition(nullptr)
,MachineState(E_MACHINE_STATE::STATE_NONE)
,PendingAnimStateType(E_ANIM_STATE::IDLE)
{

}

LAnimationStateMachine::~LAnimationStateMachine()
{
	for (auto it = States.begin(); it != States.end();) {
		delete it->second;
		it = States.erase(it); 
	}
}

E_ANIM_STATE LAnimationStateMachine::GetCurrentAnimStateType()
{
	if(MachineState == E_MACHINE_STATE::STATE_UPDATE)
		return CurrentAnimState->GetStateType();
	return PendingAnimStateType;
}

void LAnimationStateMachine::OnCreate(map<E_ANIM_STATE, LAnimationState*>& Stats, E_ANIM_STATE DefaultState)
{
	States.insert(Stats.begin(), Stats.end());
	SetCurrentAnimState(DefaultState);
}

void LAnimationStateMachine::SetCurrentAnimState(E_ANIM_STATE NewState, LAnimationStateTransition* PreTransition)
{
	CurrentAnimState = States[NewState];
	CurrentAnimState->Init(PreTransition);
	MachineState = E_MACHINE_STATE::STATE_UPDATE;
}

void LAnimationStateMachine::SetTransitionToState(E_ANIM_STATE NewState, float Time)
{
	if(Transition)
	{
		delete Transition;
		Transition = nullptr;
	}
	PendingAnimStateType = NewState;
	Transition = new LAnimationStateTransition();
	Transition->OnCreate(CurrentAnimState->GetAnimSeq(), States[NewState]->GetAnimSeq(), Time);
	MachineState = E_MACHINE_STATE::STATE_TRANSITION;
}

void LAnimationStateMachine::Update(float dt)
{
	if(MachineState == E_MACHINE_STATE::STATE_UPDATE)
	{
		CurrentAnimState->Update(dt);
	}
	else if (MachineState == E_MACHINE_STATE::STATE_TRANSITION)
	{
		if(Transition)
		{
			Transition->Update(dt);
			if (Transition->IsTransitionComplete())
			{
				SetCurrentAnimState(PendingAnimStateType, Transition);
			}
		}
	}
}

vector<LAnimBonePose>& LAnimationStateMachine::GetCurrentAnimPoseToParentTrans()
{
	if (MachineState == E_MACHINE_STATE::STATE_UPDATE)
	{
		return CurrentAnimState->GetCurrentAnimPoseToParentTrans();
	}
	else if (MachineState == E_MACHINE_STATE::STATE_TRANSITION)
	{
		if(Transition)
			return Transition->GetTransitionAnimPoseToParentTrans();
	}

	assert(!"wrong return scope!");
	return CurrentAnimState->GetCurrentAnimPoseToParentTrans();
}
