
#include "stdafx.h"

#include "FTaskThread.h"
#include "FDefine.h"
#include "LGameTimer.h"
#include "LLog.h"

FThreadTask::FThreadTask(const char* InTaskName, TaskFunc TaskF)
:Task(TaskF)
,TaskName(InTaskName)
{}

void FThreadTask::DoTask()
{
	if(Task)
	{
		Task();
	}
}


FTaskThread::FTaskThread(const string& InName)
:FThread(InName)
,FrameTaskIndex(0)
{

}

void FTaskThread::Run() 
{
	while(IsRun)
	{
		DoTasks();
	}
	ClearTask();
}

void FTaskThread::ClearTask()
{
	for (auto& Tasks : FrameTasks)
	{
		Tasks.clear();
	}
	//Tasks.clear();
}

void FTaskThread::DoTasks()
{
	FThreadTask* Task = nullptr;
	{
		lock_guard<mutex> Lock(Mutex);
		auto& Tasks = FrameTasks[FrameTaskIndex];
		//LLog::Log("Task Num::%d  \n", Tasks.size());
		while(!Tasks.empty())
		{
			Task = &Tasks.front();
			Task->DoTask();

			Tasks.pop_front();
			Task = nullptr;
		}
	}

	FrameTaskIndex = (FrameTaskIndex + 1) % FRAME_COUNT;
	/*FThreadTask* Task = nullptr;
	{
		lock_guard<mutex> Lock(Mutex);
		if (!Tasks.empty())
		{
			Task = &Tasks.front();
		}
	}
	if (Task != nullptr)
	{
		Task->DoTask();
		{
			lock_guard<mutex> Lock(Mutex);
			Tasks.pop_front();
		}
	}*/
}

void FTaskThread::AddTask(const char* Name, TaskFunc&& TaskF)
{
	if(IsRun )
	{
		lock_guard<mutex> Lock(Mutex);
		FrameTasks[FrameTaskIndex].push_back(FThreadTask(Name, TaskF));
		//Tasks.push_back(FThreadTask(Name, TaskF));
	}
}