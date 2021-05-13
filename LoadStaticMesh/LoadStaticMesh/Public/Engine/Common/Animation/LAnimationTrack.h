#pragma once

#include "stdafx.h"
#include "LAnimationTrackFrame.h"

class LAnimationTrack
{
public: 
	LAnimationTrack();
	~LAnimationTrack();

	//this track with which bone
	UINT TrackToBoneIndex;

	//this size is 1 or frame size, 1 means this bone has no change
	std::vector<LAnimationTrackFrame> TrackFrames;
};
