
#include "LAnimationSequence.h"
#include "LLog.h"

static const XMFLOAT3 NO_S = XMFLOAT3(1.f, 1.f, 1.f);
static const XMFLOAT4 NO_Q = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
static const XMFLOAT3 NO_T = XMFLOAT3(0.f, 0.f, 0.f);

LAnimationSequence::LAnimationSequence()
:TimeElapsed(0.f)
,StartTime(0.f)
,EndTime(0.f)
,IsLoop(false)
,SequenceFrameCount(0)
{
}

LAnimationSequence::~LAnimationSequence()
{

}

void LAnimationSequence::Update(float dt)
{
	if(!bIsPlay)
		return;

	TimeElapsed += dt;

	if(IsLoop && TimeElapsed > EndTime)
	{
		TimeElapsed = 0;
	}
	else 
	{
		if(TimeElapsed > EndTime)
		{
			bIsPlay = false;
		}
	}

	for(size_t i = 0; i < SequenceTracks.size(); i++)
	{
		LAnimationTrack& Track = SequenceTracks[i];

		UINT ScaleSize = static_cast<UINT>(Track.ScaleChannelFrames.size());
		UINT QuatSize = static_cast<UINT>(Track.QuatChannelFrames.size());
		UINT TranslateSize = static_cast<UINT>(Track.TranslateChannelFrames.size());
	//	XMVECTOR Zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		LAnimBonePose Pose;
		if(TimeElapsed <= FrameTimes.front())
		{
			Pose.S = ScaleSize > 0 ? XMLoadFloat3(&Track.ScaleChannelFrames.front()) : XMLoadFloat3(&NO_S);
			Pose.Q = QuatSize > 0 ? XMLoadFloat4(&Track.QuatChannelFrames.front()) : XMLoadFloat4(&NO_Q);
			Pose.T = TranslateSize > 0 ? XMLoadFloat3(&Track.TranslateChannelFrames.front()) : XMLoadFloat3(&NO_T);
			CurrentPoseToParentsTrans[i] = std::move(Pose);
			//XMStoreFloat4x4(&CurrentPoseToParentsTrans[i], XMMatrixAffineTransformation(S, Zero, Q, T));
		}
		else if (TimeElapsed >= FrameTimes.back())
		{
			 Pose.S = ScaleSize > 0 ? XMLoadFloat3(&Track.ScaleChannelFrames.back()) : XMLoadFloat3(&NO_S);
			 Pose.Q = QuatSize > 0 ? XMLoadFloat4(&Track.QuatChannelFrames.back()) : XMLoadFloat4(&NO_Q);
			 Pose.T = TranslateSize > 0 ? XMLoadFloat3(&Track.TranslateChannelFrames.back()) : XMLoadFloat3(&NO_T);
			 CurrentPoseToParentsTrans[i] = std::move(Pose);
			//XMStoreFloat4x4(&CurrentPoseToParentsTrans[i], XMMatrixAffineTransformation(S, Zero, Q, T));
		}
		else 
		{
			float IntPart;
			std::modf(TimeElapsed/ AnimFrameRate, &IntPart);
			UINT j = (UINT)IntPart;
			
			float LerpPercent = (TimeElapsed - FrameTimes[j]) / AnimFrameRate;
			XMVECTOR s0, s1, q0, q1, t0, t1;

			s0 = s1 = XMLoadFloat3(&NO_S);
			q0 = q1 = XMLoadFloat4(&NO_Q);
			t0 = t1 = XMLoadFloat3(&NO_T);

			if (ScaleSize > 0)
			{
				s0 = XMLoadFloat3(ScaleSize == 1 ? &Track.ScaleChannelFrames[0] : &Track.ScaleChannelFrames[j]);
				s1 = XMLoadFloat3(ScaleSize == 1 ? &Track.ScaleChannelFrames[0] : &Track.ScaleChannelFrames[j + 1]);
			}

			if (QuatSize > 0)
			{
				q0 = XMLoadFloat4(QuatSize == 1 ? &Track.QuatChannelFrames[0] : &Track.QuatChannelFrames[j]);
				q1 = XMLoadFloat4(QuatSize == 1 ? &Track.QuatChannelFrames[0] : &Track.QuatChannelFrames[j + 1]);
			}

			if (TranslateSize > 0)
			{
				t0 = XMLoadFloat3(TranslateSize == 1 ? &Track.TranslateChannelFrames[0] : &Track.TranslateChannelFrames[j]);
				t1 = XMLoadFloat3(TranslateSize == 1 ? &Track.TranslateChannelFrames[0] : &Track.TranslateChannelFrames[j + 1]);
			}

			Pose.S = XMVectorLerp(s0, s1, LerpPercent);
			Pose.Q = XMQuaternionSlerp(q0, q1, LerpPercent);
			Pose.T = XMVectorLerp(t0, t1, LerpPercent);
			CurrentPoseToParentsTrans[i] = std::move(Pose);
			//XMStoreFloat4x4(&CurrentPoseToParentsTrans[i], XMMatrixAffineTransformation(S, Zero, Q, T));

		}
	}
}