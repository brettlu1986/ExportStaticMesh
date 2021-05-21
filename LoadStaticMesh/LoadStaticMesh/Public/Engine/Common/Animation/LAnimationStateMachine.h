#pragma once
#include "stdafx.h"
#include "LAnimationState.h"

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

	std::vector<XMFLOAT4X4>& GetCurrentAnimPoseToParentTrans();
private:
	std::map<std::string, LAnimationState*> States;
	LAnimationState* CurrentAnimState;
	E_MACHINE_STATE MachineState;
};
