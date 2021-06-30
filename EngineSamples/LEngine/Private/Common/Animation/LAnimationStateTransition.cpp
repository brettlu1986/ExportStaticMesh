
#include "pch.h"
#include "LAnimationStateTransition.h"

static const Vec3 NO_S = Vec3(1.f, 1.f, 1.f);
static const XMFLOAT4 NO_Q = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
static const Vec3 NO_T = Vec3(0.f, 0.f, 0.f);

LAnimationStateTransition::LAnimationStateTransition()
:TransitionFrom(nullptr)
,TransitionTo(nullptr)
,TransitionTime(0.f)
,TransitionElapsed(0.f)
,bComplete(false)
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
	TransitionPoseToParentsTrans.resize(TransitionTo->GetSequenceTracks().size());
}

void LAnimationStateTransition::Update(float dt)
{
	if (bComplete)
		return;

	TransitionElapsed += dt;
	if(TransitionElapsed >= TransitionTime)
		bComplete = true;
		

	float LerpPercent = TransitionElapsed / TransitionTime;

	vector<LAnimBonePose>& FromAnimPoseToParentTrans = TransitionFrom->GetCurrentAnimPoseToParentTrans();

	vector<LAnimationTrack>& ToSequenceTracks = TransitionTo->GetSequenceTracks();

	for (size_t i = 0; i < ToSequenceTracks.size(); i++)
	{
		LAnimationTrack& Track = ToSequenceTracks[i];

		UINT ScaleSize = static_cast<UINT>(Track.ScaleChannelFrames.size());
		UINT QuatSize = static_cast<UINT>(Track.QuatChannelFrames.size());
		UINT TranslateSize = static_cast<UINT>(Track.TranslateChannelFrames.size());

		XMVECTOR S1, Q1, T1;
		S1 = ScaleSize > 0 ? XMLoadFloat3(&Track.ScaleChannelFrames.front()) : XMLoadFloat3(&NO_S);
		Q1 = QuatSize > 0 ? XMLoadFloat4(&Track.QuatChannelFrames.front()) : XMLoadFloat4(&NO_Q);
		T1 = TranslateSize > 0 ? XMLoadFloat3(&Track.TranslateChannelFrames.front()) : XMLoadFloat3(&NO_T);

		LAnimBonePose Pose;

		Pose.S = XMVectorLerp(FromAnimPoseToParentTrans[i].S, S1, LerpPercent);
		Pose.Q = XMQuaternionSlerp(FromAnimPoseToParentTrans[i].Q, Q1, LerpPercent);
		Pose.T = XMVectorLerp(FromAnimPoseToParentTrans[i].T, T1, LerpPercent);

		TransitionPoseToParentsTrans[i] = Pose;
	}

}

