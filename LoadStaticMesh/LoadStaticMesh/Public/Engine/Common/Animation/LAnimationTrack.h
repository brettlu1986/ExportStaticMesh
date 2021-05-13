#pragma once

#include "stdafx.h"
#include "LAnimationTrackChannel.h"

class LAnimationTrack
{
public: 
	LAnimationTrack();
	~LAnimationTrack();

	//this track with which bone
	UINT TrackToBoneIndex;

	void AddScaleChannelFrame(LAnimationScaleChannel S)
	{
		ScaleChannelFrames.push_back(S);
	}

	void AddQuatChannelFrame(LAnimationQuatChannel Q)
	{
		QuatChannelFrames.push_back(Q);
	}

	void AddTranslateChannelFrame(LAnimationTranslateChannel T)
	{
		TranslateChannelFrames.push_back(T);
	}

private:
	//this size is 1 or frame size, 1 means this bone has no change
	std::vector<struct LAnimationScaleChannel> ScaleChannelFrames;
	std::vector<struct LAnimationQuatChannel> QuatChannelFrames;
	std::vector<struct LAnimationTranslateChannel> TranslateChannelFrames;
};
