#include "LAnimationStateMachine.h"


LAnimationStateMachine::LAnimationStateMachine()
:CurrentAnimState(nullptr)
,Transition(nullptr)
,MachineState(E_MACHINE_STATE::STATE_NONE)
{

}

LAnimationStateMachine::~LAnimationStateMachine()
{
	for (auto it = States.begin(); it != States.end();) {
		delete it->second;
		it = States.erase(it); 
	}
}

void LAnimationStateMachine::OnCreate(std::map<std::string, LAnimationState*>& Stats, std::string DefaultState)
{
	States.insert(Stats.begin(), Stats.end());
	SetCurrentAnimState(DefaultState);
}

void LAnimationStateMachine::SetCurrentAnimState(std::string StateName)
{
	CurrentAnimState = States[StateName];
	CurrentAnimState->Init();
	MachineState = E_MACHINE_STATE::STATE_UPDATE;
}

void LAnimationStateMachine::SetTransitionToState(std::string TargetStateName, float Time)
{
	if(Transition)
	{
		delete Transition;
		Transition = nullptr;
	}
	PendingAnimStateName = TargetStateName;
	Transition = new LAnimationStateTransition();
	Transition->OnCreate(CurrentAnimState->GetAnimSeq(), States[TargetStateName]->GetAnimSeq(), Time);
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
				SetCurrentAnimState(PendingAnimStateName);
			}
		}
	}
}

std::vector<LAnimBonePose>& LAnimationStateMachine::GetCurrentAnimPoseToParentTrans()
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
