#pragma once

#include "stdafx.h"
#include "LAnimationTrack.h"
#include "LDefine.h"
#include "LSkeleton.h"

class LAnimationSequence
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

	std::vector<LAnimBonePose>& GetCurrentAnimPoseToParentTrans()
	{
		return CurrentPoseToParentsTrans;
	}

	std::vector<float>& GetFrameTimes()
	{
		return FrameTimes;
	}

	std::vector<LAnimationTrack>& GetSequenceTracks()
	{
		return SequenceTracks;
	}

private:
	
	float TimeElapsed;
	float StartTime; 
	float EndTime;
	bool IsLoop;
	bool bIsPlay;
	UINT SequenceFrameCount;
	//size according to current bone count
	std::vector<LAnimationTrack> SequenceTracks;
	std::vector<float> FrameTimes;

	std::vector<LAnimBonePose> CurrentPoseToParentsTrans;
};
