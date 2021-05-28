#pragma once

#include "stdafx.h"
#include "FThread.h"
#include <deque>
#include <functional>

using TaskFunc = function<void()>;
class FThreadTask
{
public: 
	FThreadTask(TaskFunc TaskF);

	void DoTask();
private:
	TaskFunc Task;
};

class FTaskThread : public FThread
{
public:
	FTaskThread(const string& InName);

	virtual void Run() override;
	virtual void AddTask(TaskFunc&& TaskF);
protected:

	void DoTasks();
	void ClearTask();
	deque<FThreadTask> Tasks;
	
};