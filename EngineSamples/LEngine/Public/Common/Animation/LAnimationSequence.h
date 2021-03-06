#pragma once

#include "pch.h"
#include "LAnimationTrack.h"
#include "LDefine.h"
#include "LSkeleton.h"

class LENGINE_API LAnimationSequence
{
public:
	LAnimationSequence();
	~LAnimationSequence();

	void SetFrameCount(UINT Count)
	{
		SequenceFrameCount = Count;
	}

	void SetIsPlay(bool bPlay)
	{
		bIsPlay = bPlay;
	}

	void Reset()
	{
		StartTime = 0.f;
		EndTime = SequenceFrameCount * AnimFrameRate;
		TimeElapsed = 0.f;
	}

	void AddAnimationTrack(LAnimationTrack AnimTrack)
	{
		SequenceTracks.push_back(AnimTrack);
	}

	void Update(float dt);
	void SetIsLoop(bool bLoop)
	{
		IsLoop = bLoop;
	}

	bool IsAnimationSequenceFinished()
	{
		return bIsPlay == false && TimeElapsed >= EndTime;
	}

	float GetStartTime() { return StartTime;}
	float GetEndTime() { return EndTime; }

	vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans()
	{
		return CurrentPoseToParentsTrans;
	}

	vector<float>& GetFrameTimes()
	{
		return FrameTimes;
	}

	vector<LAnimationTrack>& GetSequenceTracks()
	{
		return SequenceTracks;
	}

	//void SetPreTransitionEndSeq(vector<LAnimBonePose>& TransitionAnimPoseToParent)
	//{
	//	PreTransitionEndSeq.clear();
	//	PreTransitionEndSeq.resize(TransitionAnimPoseToParent.size());
	//	copy(TransitionAnimPoseToParent.begin(), TransitionAnimPoseToParent.end(), PreTransitionEndSeq.begin());
	//	bLerpTransitionEnd = true;
	//}
private:
	
	float TimeElapsed;
	float StartTime; 
	float EndTime;
	bool IsLoop;
	bool bIsPlay;
	
	UINT SequenceFrameCount;
	//size according to current bone count
	vector<LAnimationTrack> SequenceTracks;
	vector<float> FrameTimes;

	vector<LAnimBonePose> CurrentPoseToParentsTrans;

	//bool bLerpTransitionEnd;
	//vector<LAnimBonePose> PreTransitionEndSeq;
};
