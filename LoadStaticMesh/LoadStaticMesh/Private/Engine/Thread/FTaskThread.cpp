
#include "stdafx.h"

#include "FTaskThread.h"
#include "FDefine.h"

FThreadTask::FThreadTask(TaskFunc TaskF)
:Task(TaskF)
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

void FTaskThread::AddTask(TaskFunc&& TaskF)
{
	if(IsRun )
	{
		lock_guard<mutex> Lock(Mutex);
		Tasks.push_back(FThreadTask(TaskF));
	}
}