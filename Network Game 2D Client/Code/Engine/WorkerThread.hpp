#ifndef include_WorkerThread
#define include_WorkerThread
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Job.hpp"


//-----------------------------------------------------------------------------------------------
enum threadStatus
{
	OPEN,
	WORKING,
	FINISHED_JOB,
};


//-----------------------------------------------------------------------------------------------
class WorkerThread
{
public:
	WorkerThread();
	WorkerThread( jobType jobTypesToHandle );

	threadStatus	m_status;
	jobType			m_jobTypeToHandle;
};


#endif // include_WorkerThread