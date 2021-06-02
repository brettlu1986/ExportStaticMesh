
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
	Tasks.clear();
}

void FTaskThread::DoTasks()
{
	FThreadTask* Task = nullptr;
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
	}
}

void FTaskThread::AddTask(const char* Name, TaskFunc&& TaskF)
{
	if(IsRun )
	{
		lock_guard<mutex> Lock(Mutex);
		Tasks.push_back(FThreadTask(Name, TaskF));
	}
}