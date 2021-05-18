#pragma once

#include "stdafx.h"

using namespace DirectX;

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
	std::vector<XMFLOAT3> ScaleChannelFrames;
	std::vector<XMFLOAT4> QuatChannelFrames;
	std::vector<XMFLOAT3> TranslateChannelFrames;
};
