#ifndef include_Job
#define include_Job
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include "NamedProperties.hpp"


//-----------------------------------------------------------------------------------------------
enum priorityRating
{
	LOW_PRIORITY,
	BELOW_AVERAGE_PRIORITY,
	AVERAGE_PRIORITY,
	ABOVE_AVERAGE_PRIORITY,
	HIGH_PRIORITY,
};


//-----------------------------------------------------------------------------------------------
enum jobType
{
	JOB_TYPE_UNDEFINED,
	JOB_TYPE_FILE_IO,
	JOB_TYPE_HASH_ENCRYPTION,
	NUMBER_OF_JOB_TYPES,
};


//-----------------------------------------------------------------------------------------------
class Job
{
public:
	Job();
	Job( priorityRating priority );
	virtual void Execute() {}
	virtual void FireCallbackEvent() {}

	priorityRating	m_priority;
	jobType			m_jobType;
};


//-----------------------------------------------------------------------------------------------
class LoadFileJob : public Job
{
	typedef void ( *func ) ( char*, long );

public:
	LoadFileJob( func callbackFunction, const std::string& fileLocation );
	LoadFileJob( func callbackFunction, const std::string& fileLocation, priorityRating priority );
	void Execute();
	void FireCallbackEvent();

private:
	std::string		m_fileLocation;
	char*			m_byteBuffer;
	long			m_bufferLength;
	func			m_callbackFunction;
};


//-----------------------------------------------------------------------------------------------
class SaveFileJob : public Job
{
	typedef bool ( *func ) ( bool );

public:
	SaveFileJob( func callbackFunction, const std::string& fileLocation, char* buffer );
	SaveFileJob( func callbackFunction, const std::string& fileLocation, char* buffer, priorityRating priority );
	void Execute();
	void FireCallbackEvent();

private:
	std::string		m_fileLocation;
	char*			m_byteBuffer;
	bool			m_savedFile;
	func			m_callbackFunction;
};


//-----------------------------------------------------------------------------------------------
class HashBufferJob : public Job
{
	typedef unsigned int ( *func ) ( unsigned int );

public:
	HashBufferJob( func callbackFunction, char* buffer, long length );
	HashBufferJob( func callbackFunction, char* buffer, long length, priorityRating priority );
	void Execute();
	void FireCallbackEvent();

private:
	char*			m_byteBuffer;
	long			m_bufferLength;
	unsigned int	m_hashValue;
	func			m_callbackFunction;
};


//-----------------------------------------------------------------------------------------------
class ReverseBufferJob : public Job
{
	typedef void ( *func ) (  );

public:
	ReverseBufferJob( func callbackFunction );
	ReverseBufferJob( func callbackFunction, priorityRating priority );
	void Execute();
	void FireCallbackEvent();

private:
	func		m_callbackFunction;
};


#endif // include_Job