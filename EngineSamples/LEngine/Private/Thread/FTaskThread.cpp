
#include "pch.h"

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
,TasksIndex(0)
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

		//LLog::Log("---------- \n");
		//LLog::Log("Task Num::%d  TaskIdx::%d \n", Tasks.size(), FrameTaskIndex);
		while(!Tasks.empty())
		{
			Task = &Tasks.front();
			//LLog::Log("TaskName:%s \n", Task->GetTaskName().c_str());
			Task->DoTask();

			Tasks.pop_front();
			Task = nullptr;
		}
		//LLog::Log("---------- \n");

	}

	FrameTaskIndex = (FrameTaskIndex + 1) % FRAME_COUNT;
}

void FTaskThread::AddTask(const char* Name, TaskFunc&& TaskF)
{
	if(IsRun )
	{
		lock_guard<mutex> Lock(Mutex);
		FrameTasks[TasksIndex].push_back(FThreadTask(Name, TaskF));
	}
}