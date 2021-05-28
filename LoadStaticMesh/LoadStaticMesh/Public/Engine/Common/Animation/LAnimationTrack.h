#pragma once

#include "stdafx.h"


class LAnimationTrack
{
public: 
	LAnimationTrack();
	~LAnimationTrack();

	//this track with which bone
	UINT TrackToBoneIndex;

	void AddScaleChannelFrame(XMFLOAT3 S)
	{
		ScaleChannelFrames.push_back(S);
	}

	void AddQuatChannelFrame(XMFLOAT4 Q)
	{
		QuatChannelFrames.push_back(Q);
	}

	void AddTranslateChannelFrame(XMFLOAT3 T)
	{
		TranslateChannelFrames.push_back(T);
	}

	//this size is 1 or frame size, 1 means this bone has no change
	vector<XMFLOAT3> ScaleChannelFrames;
	vector<XMFLOAT4> QuatChannelFrames;
	vector<XMFLOAT3> TranslateChannelFrames;
};
