#pragma once

#include "stdafx.h"

class FResourceView
{
public:
	FResourceView() {};
	virtual ~FResourceView() = default;
};

class FResourceViewCreater
{
public:
	FResourceViewCreater() {};
	virtual ~FResourceViewCreater() = default;

	virtual void OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) = 0;
	virtual void OnDestroy() = 0;
};