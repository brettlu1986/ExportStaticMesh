#pragma once
#include "stdafx.h"
#include "LAnimationState.h"
#include "LAnimationStateTransition.h"

typedef enum class E_MACHINE_STATE :UINT8
{
	STATE_NONE = 0,
	STATE_UPDATE,
	STATE_TRANSITION,
}E_MACHINE_STATE;

class LAnimationStateMachine
{
public:
	LAnimationStateMachine();
	~LAnimationStateMachine();
	
	void OnCreate(std::map<std::string, LAnimationState*>& Stats, std::string DefaultState);
	void Update(float dt);
	void SetCurrentAnimState(std::string StateName);
	void SetTransitionToState(std::string TargetStateName, float Time);

	std::vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans();

	std::string& GetCurrentAnimStateName()
	{
		return CurrentAnimState->GetName();
	}

	LAnimationState* GetCurrentAnimState()
	{
		return CurrentAnimState;
	}

	bool IsTransition()
	{
		return MachineState == E_MACHINE_STATE::STATE_TRANSITION;
	}

private:
	std::map<std::string, LAnimationState*> States;
	LAnimationState* CurrentAnimState;
	std::string PendingAnimStateName;
	LAnimationStateTransition* Transition;
	E_MACHINE_STATE MachineState;
};
