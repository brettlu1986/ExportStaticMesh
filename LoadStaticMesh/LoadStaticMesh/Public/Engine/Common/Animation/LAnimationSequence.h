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

private:

	UINT SequenceFrameCount;
	//size according to current bone count
	std::vector<LAnimationTrack> SequenceTracks;
};
