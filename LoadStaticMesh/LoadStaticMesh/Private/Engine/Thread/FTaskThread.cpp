

#include "FTaskThread.h"

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


FTaskThread::FTaskThread(const std::string& InName)
:FThread(InName)
{

}

void FTaskThread::Run() 
{
	while(IsRun)
	{
		FThreadTask* Task = nullptr;
		{
			lock_guard<mutex> Lock(Mutex);
			if(!Tasks.empty())
			{
				Task = &Tasks.front();
			}
		}
		if(Task != nullptr)
		{
			Task->DoTask();
			{
				lock_guard<mutex> Lock(Mutex);
				Tasks.pop_front();
			}
		}
	}

	Tasks.clear();
}

void FTaskThread::AddTask(TaskFunc&& TaskF)
{
	if(IsRun)
	{
		lock_guard<mutex> Lock(Mutex);
		Tasks.push_back(FThreadTask(TaskF));
	}
}