#include "JobManager.hpp"
#include <process.h>
#include "EngineCommon.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
STATIC std::vector< Job* > JobManager::s_jobsTodoByType[];
STATIC std::vector< Job* > JobManager::s_jobsCompleted;
STATIC std::vector< WorkerThread* > JobManager::s_workerThreads;
STATIC CRITICAL_SECTION JobManager::s_cs;


//-----------------------------------------------------------------------------------------------
void WorkerThreadEntryFunc( void* data )
{
	WorkerThread* workerThread = static_cast< WorkerThread* >( data );

	while( !g_isQuitting )
	{
		Job* job = JobManager::GetJobFromTodoList( workerThread->m_jobTypeToHandle );
		if( job )
		{
			job->Execute();
			JobManager::ReportCompletedJob( job );
		}
		else
		{
			Sleep( 0 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
JobManager::JobManager()
{
	InitializeCriticalSection( &s_cs );
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::CreateNewWorkerThread()
{
	WorkerThread* workerThread = new WorkerThread();

	EnterCriticalSection( &s_cs );
	s_workerThreads.push_back( workerThread );
	LeaveCriticalSection( &s_cs );

	_beginthread( WorkerThreadEntryFunc, 0, workerThread );
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::CreateNewWorkerThread( jobType jobTypeToHandle )
{
	WorkerThread* workerThread = new WorkerThread( jobTypeToHandle );

	EnterCriticalSection( &s_cs );
	s_workerThreads.push_back( workerThread );
	LeaveCriticalSection( &s_cs );

	_beginthread( WorkerThreadEntryFunc, 0, workerThread );
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::AddNewJob( Job* job )
{
	EnterCriticalSection( &s_cs );

	std::vector< Job* > jobsTodo = s_jobsTodoByType[ job->m_jobType ];
	jobsTodo.push_back( job );
	s_jobsTodoByType[ job->m_jobType ] = jobsTodo;

	LeaveCriticalSection( &s_cs );
}


//-----------------------------------------------------------------------------------------------
STATIC Job* JobManager::GetJobFromTodoList( jobType typeOfJobToGet )
{
	Job* returnJob = nullptr;
	std::vector< Job* > jobsTodo;

	EnterCriticalSection( &s_cs );
	jobsTodo = s_jobsTodoByType[ typeOfJobToGet ];
	LeaveCriticalSection( &s_cs );

	if( jobsTodo.size() > 0 )
	{
		returnJob = jobsTodo.front();
		unsigned int jobPosition = 0;

		for( unsigned int jobIndex = 0; jobIndex < jobsTodo.size(); ++jobIndex )
		{
			Job* job = jobsTodo[ jobIndex ];
			if( returnJob < job )
			{
				returnJob = job;
				jobPosition = jobIndex;
			}
		}

		jobsTodo.erase( jobsTodo.begin() + jobPosition );

		EnterCriticalSection( &s_cs );
		s_jobsTodoByType[ typeOfJobToGet ] = jobsTodo;
		LeaveCriticalSection( &s_cs );
	}

	return returnJob;
}


//-----------------------------------------------------------------------------------------------
STATIC Job* JobManager::GetJobOfAnyTypeFromTodoList()
{
	Job* returnJob = nullptr;
	unsigned int todoListToEdit = 0;
	unsigned int jobPosition = 0;

	for( unsigned int jobTypeIndex = 0; jobTypeIndex < NUMBER_OF_JOB_TYPES; ++jobTypeIndex )
	{
		std::vector< Job* > jobsTodo;

		EnterCriticalSection( &s_cs );
		jobsTodo = s_jobsTodoByType[ jobTypeIndex ];
		LeaveCriticalSection( &s_cs );

		if( jobsTodo.size() > 0 )
		{
			for( unsigned int jobIndex = 0; jobIndex < jobsTodo.size(); ++jobIndex )
			{
				Job* job = jobsTodo[ jobIndex ];
				if( returnJob == nullptr || returnJob < job )
				{
					returnJob = job;
					todoListToEdit = jobTypeIndex;
					jobPosition = jobIndex;
				}
			}
		}
	}

	EnterCriticalSection( &s_cs );
	std::vector< Job* > jobsTodo = s_jobsTodoByType[ todoListToEdit ];
	if( jobsTodo.size() > 0 )
		jobsTodo.erase( jobsTodo.begin() + jobPosition );
	s_jobsTodoByType[ todoListToEdit ] = jobsTodo;
	LeaveCriticalSection( &s_cs );

	return returnJob;
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::ReportCompletedJob( Job* job )
{
	EnterCriticalSection( &s_cs );
	s_jobsCompleted.push_back( job );
	LeaveCriticalSection( &s_cs );
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::ChangeJobPriority( Job* job, priorityRating newPriority )
{
	std::vector< Job* > jobsTodo;

	EnterCriticalSection( &s_cs );
	jobsTodo = s_jobsTodoByType[ job->m_jobType ];
	LeaveCriticalSection( &s_cs );

	for( unsigned int jobIndex = 0; jobIndex < jobsTodo.size(); ++jobIndex )
	{
		Job* jobToChange = jobsTodo[ jobIndex ];
		if( job == jobToChange )
		{
			jobToChange->m_priority = newPriority;
			break;
		}
	}

	EnterCriticalSection( &s_cs );
	s_jobsTodoByType[ job->m_jobType ] = jobsTodo;
	LeaveCriticalSection( &s_cs );

	
}


//-----------------------------------------------------------------------------------------------
STATIC void JobManager::Update()
{
	if( s_workerThreads.size() == 0 )
	{
		Job* job = GetJobOfAnyTypeFromTodoList();
		if( job )
		{
			job->Execute();
			ReportCompletedJob( job );
		}
	}

	EnterCriticalSection( &s_cs );
	while( s_jobsCompleted.size() > 0 )
	{
		Job* completedJob = s_jobsCompleted.front();
		completedJob->FireCallbackEvent();
		delete completedJob;
		completedJob = nullptr;
		s_jobsCompleted.erase( s_jobsCompleted.begin() );
	}
	LeaveCriticalSection( &s_cs );
}