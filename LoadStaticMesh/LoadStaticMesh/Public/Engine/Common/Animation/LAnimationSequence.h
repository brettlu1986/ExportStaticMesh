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

private:

	UINT SequenceFrameCount;
	//size according to current bone count
	std::vector<LAnimationTrack> SequenceTracks;
};
