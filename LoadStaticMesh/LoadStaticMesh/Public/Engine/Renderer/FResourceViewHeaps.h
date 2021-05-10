#pragma once

#include "stdafx.h"

class FResourceView
{
public:
	FResourceView() {};
	virtual ~FResourceView() = default;
};

class FResourceViewHeaps
{
public:
	FResourceViewHeaps() {};
	virtual ~FResourceViewHeaps() = default;

	virtual void OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) = 0;
	virtual void OnDestroy() = 0;
};