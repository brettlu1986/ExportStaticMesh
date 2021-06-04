#pragma once

#include "stdafx.h"

#include "LTexture.h"
#include "LSkeleton.h"
#include "LShader.h"

class LAssetManager
{
public:
	LAssetManager();
	~LAssetManager();

	static LAssetManager* Get();

	void LoadSkeletal(std::string FileName, string ReferenceName);
	void LoadTexture(std::string FileName, string ReferenceName);
	void LoadShader(LPCWSTR FileName, string ReferenceName);

	LSkeleton* GetSkeletal(const std::string& RefName) {
		return ResourceSkeletons[RefName].get();
	}

	LShader* GetShader(const std::string& RefName)
	{
		return ResourceShaders[RefName].get();
	}

	LTexture* GetTexture(const std::string& RefName)
	{
		return ResourceTexs[RefName].get();
	}

private:
	static LAssetManager* Instance;

	unordered_map<std::string, unique_ptr<LSkeleton>> ResourceSkeletons;
	unordered_map<std::string, unique_ptr<LTexture>> ResourceTexs;
	unordered_map<std::string, unique_ptr<LShader>> ResourceShaders;
};
