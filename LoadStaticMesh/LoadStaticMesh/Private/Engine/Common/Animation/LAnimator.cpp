

#include "LAnimator.h"
#include "LAnimationState.h"
#include "LEngine.h"
#include "LLog.h"
//#include <functional>

LAnimator::LAnimator()
:Skeleton(nullptr)
,DefaultStateMachine(nullptr)
{

}

LAnimator::~LAnimator()
{
	if(DefaultStateMachine)
	{
		delete DefaultStateMachine;
		DefaultStateMachine = nullptr;
	}
}

void LAnimator::SetSkeleton(LSkeleton* Ske)
{
	Skeleton = Ske;
	AllBoneFinalTransforms.resize(Skeleton->GetBoneCount());
	BoneMapFinalTransforms.resize(Skeleton->GetBoneMap().size());
}

void LAnimator::Update(float dt)
{
	//default state machine update
	{
		DefaultStateMachine->Update(dt);
	}

	// calculate bone map final transforms according to current pose
	{
		std::vector<XMFLOAT4X4>& CurrentAnimPoseToParentTrans = DefaultStateMachine->GetCurrentAnimPoseToParentTrans();
		UINT BoneCount = Skeleton->GetBoneCount();
		std::vector<XMFLOAT4X4> CurrentAnimPoseToRootTrans(BoneCount);
		//root
		CurrentAnimPoseToRootTrans[0] = CurrentAnimPoseToParentTrans[0];
		for (UINT i = 1; i < BoneCount; i++)
		{
			XMMATRIX ToParent = XMLoadFloat4x4(&CurrentAnimPoseToParentTrans[i]);
			int ParentIndex = Skeleton->GetBoneInfos()[i].ParentIndex;
			XMMATRIX ParentToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[ParentIndex]);

			XMMATRIX ToRoot = XMMatrixMultiply(ToParent, ParentToRoot);
			XMStoreFloat4x4(&CurrentAnimPoseToRootTrans[i], ToRoot);
		}

		const std::vector<XMFLOAT4X4>& RefPoseRootToBoneTrans = Skeleton->GetRefPoseRootToBoneTransform();
		for (UINT i = 0; i < BoneCount; ++i)
		{
			XMMATRIX RefRootToBone = XMLoadFloat4x4(&RefPoseRootToBoneTrans[i]);
			XMMATRIX CurBoneToRoot = XMLoadFloat4x4(&CurrentAnimPoseToRootTrans[i]);
			XMMATRIX finalTransform = XMMatrixMultiply(RefRootToBone, CurBoneToRoot);
			XMStoreFloat4x4(&AllBoneFinalTransforms[i], XMMatrixTranspose(finalTransform));
		}

		const std::vector<UINT16>& BoneMap = Skeleton->GetBoneMap();
		for (size_t i = 0; i < BoneMap.size(); i++)
		{
			UINT BoneIndex = BoneMap[i];
			BoneMapFinalTransforms[i] = AllBoneFinalTransforms[BoneIndex];
		}
	}
}

void LAnimator::InitStateMachines()
{
	CreateDefaultStateMachine();
}

void LAnimator::CreateDefaultStateMachine()
{
	DefaultStateMachine = new LAnimationStateMachine();
	std::map<std::string, LAnimationState*> Stats;
	Stats["Idle"] = new LAnimationState("Idle", true, &AnimSequences["Idle"]);
	Stats["Walk"] = new LAnimationState("Walk", true, &AnimSequences["Walk"]);
	Stats["Run"] = new LAnimationState("Run", true, &AnimSequences["Run"]);
	Stats["JumpStart"] = new LAnimationState("JumpStart", false, &AnimSequences["JumpStart"]);
	Stats["JumpLoop"] = new LAnimationState("JumpLoop", false, &AnimSequences["JumpLoop"]);
	Stats["JumpEnd"] = new LAnimationState("JumpEnd", false, &AnimSequences["JumpEnd"]);
	DefaultStateMachine->OnCreate(Stats, "Idle");

	//EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
	//std::function<void(std::string)> Func = std::bind(&LAnimator::ProcessDefaultStateMachineChange, this, std::placeholders::_1);
	//EventDisp.RegisterEvent(new LEvent<std::string>(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, Func));
}

void LAnimator::ProcessDefaultStateMachineChange(std::string name)
{
	if(name == "Walk")
	{
		LLog::Log("change to walk \n");
		//DefaultStateMachine->SetCurrentAnimState()
	}
}