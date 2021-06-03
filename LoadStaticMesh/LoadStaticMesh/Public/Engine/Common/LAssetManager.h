#pragma once

#include "stdafx.h"

#include "LTexture.h"
#include "LSkeleton.h"

class LAssetManager
{
public:
	LAssetManager();
	~LAssetManager();


private:
	unordered_map<std::string, unique_ptr<LSkeleton>> ResourceSkeletons;
	unordered_map<std::string, unique_ptr<LTexture>> ResourceTexs;
};
