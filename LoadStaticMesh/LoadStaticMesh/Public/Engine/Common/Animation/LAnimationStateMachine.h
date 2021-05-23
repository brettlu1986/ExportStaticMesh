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

	std::vector<XMFLOAT4X4>& GetCurrentAnimPoseToParentTrans();

	std::string& GetCurrentAnimStateName()
	{
		return CurrentAnimState->GetName();
	}

	LAnimationState* GetAnimationState(std::string& Name) 
	{
		return States[Name];
	}

private:
	std::map<std::string, LAnimationState*> States;
	LAnimationState* CurrentAnimState;
	LAnimationStateTransition* Transition;
	E_MACHINE_STATE MachineState;
};
