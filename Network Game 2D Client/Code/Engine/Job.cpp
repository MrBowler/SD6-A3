#include "Job.hpp"
#include "EventSystem.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
Job::Job()
	: m_priority( AVERAGE_PRIORITY )
	, m_jobType( JOB_TYPE_UNDEFINED )
{

}


//-----------------------------------------------------------------------------------------------
Job::Job( priorityRating priority )
	: m_priority( priority )
	, m_jobType( JOB_TYPE_UNDEFINED )
{

}


//-----------------------------------------------------------------------------------------------
LoadFileJob::LoadFileJob( func callbackFunction, const std::string& fileLocation )
	: m_callbackFunction( callbackFunction )
	, m_fileLocation( fileLocation )
{
	m_jobType = JOB_TYPE_FILE_IO;
}


//-----------------------------------------------------------------------------------------------
LoadFileJob::LoadFileJob( func callbackFunction, const std::string& fileLocation, priorityRating priority )
	: m_callbackFunction( callbackFunction )
	, m_fileLocation( fileLocation )
{
	m_priority = priority;
	m_jobType = JOB_TYPE_FILE_IO;
}


//-----------------------------------------------------------------------------------------------
void LoadFileJob::Execute()
{
	FILE* file;
	errno_t fileOpenError = fopen_s( &file, m_fileLocation.c_str(), "rb" );
	if( fileOpenError )
		return;

	fseek( file, 0, SEEK_END );
	long m_bufferLength = ftell( file );
	rewind( file );

	m_byteBuffer = new char[ m_bufferLength ];
	fread( m_byteBuffer, sizeof( char ), m_bufferLength, file );
	fclose( file );
}


//-----------------------------------------------------------------------------------------------
void LoadFileJob::FireCallbackEvent()
{
	m_callbackFunction( m_byteBuffer, m_bufferLength );
}


//-----------------------------------------------------------------------------------------------
SaveFileJob::SaveFileJob( func callbackFunction, const std::string& fileLocation, char* buffer )
	: m_callbackFunction( callbackFunction )
	, m_fileLocation( fileLocation )
	, m_byteBuffer( buffer )
{
	m_jobType = JOB_TYPE_FILE_IO;
}


//-----------------------------------------------------------------------------------------------
SaveFileJob::SaveFileJob( func callbackFunction, const std::string& fileLocation, char* buffer, priorityRating priority )
	: m_callbackFunction( callbackFunction )
	, m_fileLocation( fileLocation )
	, m_byteBuffer( buffer )
{
	m_priority = priority;
	m_jobType = JOB_TYPE_FILE_IO;
}


//-----------------------------------------------------------------------------------------------
void SaveFileJob::Execute()
{
	m_savedFile = false;
	FILE* file;
	errno_t fileOpenError = fopen_s( &file, m_fileLocation.c_str(), "wb" );
	if( fileOpenError )
		return;

	fwrite( m_byteBuffer, sizeof( char ), sizeof( m_byteBuffer ), file );
	fclose( file );
	m_savedFile = true;
}


//-----------------------------------------------------------------------------------------------
void SaveFileJob::FireCallbackEvent()
{
	m_callbackFunction( m_savedFile );
}


//-----------------------------------------------------------------------------------------------
HashBufferJob::HashBufferJob( func callbackFunction, char* buffer, long length )
	: m_callbackFunction( callbackFunction )
	, m_byteBuffer( buffer )
	, m_bufferLength( length )
{
	m_jobType = JOB_TYPE_HASH_ENCRYPTION;
}


//-----------------------------------------------------------------------------------------------
HashBufferJob::HashBufferJob( func callbackFunction, char* buffer, long length, priorityRating priority )
	: m_callbackFunction( callbackFunction )
	, m_byteBuffer( buffer )
	, m_bufferLength( length )
{
	m_priority = priority;
	m_jobType = JOB_TYPE_HASH_ENCRYPTION;
}


//-----------------------------------------------------------------------------------------------
void HashBufferJob::Execute()
{
	m_hashValue = 0;
	for( int byteIndex = 0; byteIndex < m_bufferLength; ++byteIndex )
	{
		m_hashValue &= 0x07ffffff;
		m_hashValue *= 31;
		m_hashValue += m_byteBuffer[ byteIndex ];
	}
}


//-----------------------------------------------------------------------------------------------
void HashBufferJob::FireCallbackEvent()
{
	m_callbackFunction( m_hashValue );
}


//-----------------------------------------------------------------------------------------------
ReverseBufferJob::ReverseBufferJob( func callbackFunction )
	: m_callbackFunction( callbackFunction )
{
	m_jobType = JOB_TYPE_HASH_ENCRYPTION;
}


//-----------------------------------------------------------------------------------------------
ReverseBufferJob::ReverseBufferJob( func callbackFunction, priorityRating priority )
	: m_callbackFunction( callbackFunction )
{
	m_priority = priority;
	m_jobType = JOB_TYPE_HASH_ENCRYPTION;
}


//-----------------------------------------------------------------------------------------------
void ReverseBufferJob::Execute()
{

}


//-----------------------------------------------------------------------------------------------
void ReverseBufferJob::FireCallbackEvent()
{
	m_callbackFunction(  );
}