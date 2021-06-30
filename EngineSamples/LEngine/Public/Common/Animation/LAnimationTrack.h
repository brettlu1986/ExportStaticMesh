#pragma once

#include "pch.h"


class LENGINE_API LAnimationTrack
{
public: 
	LAnimationTrack();
	~LAnimationTrack();

	//this track with which bone
	UINT TrackToBoneIndex;

	void AddScaleChannelFrame(Vec3 S)
	{
		ScaleChannelFrames.push_back(S);
	}

	void AddQuatChannelFrame(XMFLOAT4 Q)
	{
		QuatChannelFrames.push_back(Q);
	}

	void AddTranslateChannelFrame(Vec3 T)
	{
		TranslateChannelFrames.push_back(T);
	}

	//this size is 1 or frame size, 1 means this bone has no change
	vector<Vec3> ScaleChannelFrames;
	vector<XMFLOAT4> QuatChannelFrames;
	vector<Vec3> TranslateChannelFrames;
};
