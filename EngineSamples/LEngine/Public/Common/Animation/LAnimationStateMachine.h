#pragma once
#include "pch.h"
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
	
	void OnCreate(map<E_ANIM_STATE, LAnimationState*>& Stats, E_ANIM_STATE DefaultState);
	void Update(float dt);
	void SetCurrentAnimState(E_ANIM_STATE NewState, LAnimationStateTransition* PreTransition = nullptr);
	void SetTransitionToState(E_ANIM_STATE NewState, float Time);

	vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans();

	E_ANIM_STATE GetCurrentAnimStateType();

	LAnimationState* GetCurrentAnimState()
	{
		return CurrentAnimState;
	}

	bool IsTransition()
	{
		return MachineState == E_MACHINE_STATE::STATE_TRANSITION;
	}

private:
	map<E_ANIM_STATE, LAnimationState*> States;
	LAnimationState* CurrentAnimState;
	E_ANIM_STATE PendingAnimStateType;
	LAnimationStateTransition* Transition;
	E_MACHINE_STATE MachineState;
};
