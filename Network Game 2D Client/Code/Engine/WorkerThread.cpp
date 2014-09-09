#include "WorkerThread.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
WorkerThread::WorkerThread()
	: m_status( OPEN )
	, m_jobTypeToHandle( JOB_TYPE_UNDEFINED )
{

}


//-----------------------------------------------------------------------------------------------
WorkerThread::WorkerThread( jobType jobTypesToHandle )
	: m_status( OPEN )
	, m_jobTypeToHandle( jobTypesToHandle )
{

}