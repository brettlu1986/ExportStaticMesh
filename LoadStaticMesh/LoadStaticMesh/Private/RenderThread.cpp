
#include "RenderThread.h"
#include <functional>
#include <iostream>

RenderThread::RenderThread()
{

}

RenderThread::~RenderThread()
{
	ThreadExit();
}

void RenderThread::Start(const CallBack& Run, const CallBack& Exit)
{
	ThreadExit = Exit;
	std::thread thr(Run);
	Thread = std::move(thr);
}

void RenderThread::Detach()
{
	Thread.detach();
}
