#ifndef include_JobManager
#define include_JobManager
#pragma once

//-----------------------------------------------------------------------------------------------
#include <queue>
#include <vector>
#include <windows.h>
#include "Job.hpp"
#include "WorkerThread.hpp"


//-----------------------------------------------------------------------------------------------
void WorkerThreadEntryFunc( void* data );


//-----------------------------------------------------------------------------------------------
class JobManager
{
public:
	JobManager();
	static void CreateNewWorkerThread();
	static void CreateNewWorkerThread( jobType jobTypeToHandle );
	static void AddNewJob( Job* job );
	static Job* GetJobFromTodoList( jobType typeOfJobToGet );
	static Job* GetJobOfAnyTypeFromTodoList();
	static void ReportCompletedJob( Job* job );
	static void ChangeJobPriority( Job* job, priorityRating newPriority );
	static void Update();

	static CRITICAL_SECTION					s_cs;

//private:
	static std::vector< Job* >				s_jobsTodoByType[ NUMBER_OF_JOB_TYPES ];
	static std::vector< Job* >				s_jobsCompleted;
	static std::vector< WorkerThread* >		s_workerThreads;
};


#endif // include_JobManager