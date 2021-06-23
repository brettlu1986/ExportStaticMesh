#pragma once

#include "stdafx.h"
#include "FThread.h"
#include <deque>
#include <functional>
#include "FDefine.h"

using TaskFunc = function<void()>;
class FThreadTask
{
public: 
	FThreadTask(const char* InTaskName, TaskFunc TaskF);

	void DoTask();

	std::string& GetTaskName() { return TaskName;}
private:
	TaskFunc Task;
	std::string TaskName;
};

class FTaskThread : public FThread
{
public:
	FTaskThread(const string& InName);

	virtual void Run() override;
	virtual void AddTask(const char* TaskName, TaskFunc&& TaskF);
protected:

	void DoTasks();
	void ClearTask();

	//deque<FThreadTask> Tasks;
	UINT FrameTaskIndex;
	UINT TasksIndex;
	deque<FThreadTask> FrameTasks[FRAME_COUNT];
	
};