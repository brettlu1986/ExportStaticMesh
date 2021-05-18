#pragma once

#include "stdafx.h"
#include "LAnimationTrack.h"

class LAnimationSequence
{
public:
	LAnimationSequence();
	~LAnimationSequence();

	void SetFrameCount(UINT Count)
	{
		SequenceFrameCount = Count;
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
private:
	
	float TimeElapsed;
	float StartTime; 
	float EndTime;
	bool IsLoop;
	UINT SequenceFrameCount;
	//size according to current bone count
	std::vector<LAnimationTrack> SequenceTracks;
};
