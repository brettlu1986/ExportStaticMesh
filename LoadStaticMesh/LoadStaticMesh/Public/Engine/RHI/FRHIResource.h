#pragma once

#include "stdafx.h"

class FGenericFence
{
public: 
	FGenericFence(const std::string& Name)
	:FenceName(Name)
	{}

	virtual void CreateFence() {};
	virtual void SignalCurrentFence() {};
	virtual UINT64 GetCompletedValue() { return 0; };
	virtual bool IsFenceComplete() { return false;};

protected:
	std::string FenceName;
};

