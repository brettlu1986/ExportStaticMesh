#pragma once

#include <thread>
#include <atomic>
#include <functional>

typedef std::function<void()> CallBack;

class RenderThread
{
public:
	RenderThread();
	~RenderThread();

	void Start(const CallBack& Run, const CallBack& Exit);
	void Detach();

private:
	std::thread Thread;
	CallBack ThreadExit;
};
