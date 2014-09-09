#include "NewDeleteFunctions.hpp"
#include <malloc.h>
#include "EngineCommon.hpp"
#include "MemoryManager.hpp"
#include "ErrorWarningAssertions.hpp"


//-----------------------------------------------------------------------------------------------
#undef new


//-----------------------------------------------------------------------------------------------
void* operator new( size_t size )
{
	if( !MemoryManager::IsMemoryManagerAvailable() )
		MemoryManager::Initialize( POOL_MEMORY_IN_BYTES );
		
	void* data = MemoryManager::AllocateMemory( size );
	if( data == nullptr )
	{
		static const std::bad_alloc nomem;
		_RAISE(nomem);
	}

	return data;
}


//-----------------------------------------------------------------------------------------------
void operator delete( void* data )
{
	MemoryManager::FreeMemory( data );
}


//-----------------------------------------------------------------------------------------------
void* operator new[]( size_t size )
{
	return ( operator new( size ) );
}


//-----------------------------------------------------------------------------------------------
void operator delete[]( void* data )
{
	operator delete( data );
}


//-----------------------------------------------------------------------------------------------
void* operator new( size_t size, const char* file, unsigned int line )
{
	if( !MemoryManager::IsMemoryManagerAvailable() )
		MemoryManager::Initialize( POOL_MEMORY_IN_BYTES );

	void* data = MemoryManager::AllocateMemory( size, file, line );
	if( data == nullptr )
	{
		static const std::bad_alloc nomem;
		_RAISE(nomem);
	}

	return data;
}


//-----------------------------------------------------------------------------------------------
void operator delete( void* data, const char*, unsigned int )
{
	operator delete( data );
}


//-----------------------------------------------------------------------------------------------
void* operator new[]( size_t size, const char* file, unsigned int line )
{
	return ( operator new( size, file, line ) );
}


//-----------------------------------------------------------------------------------------------
void operator delete[]( void* data, const char*, unsigned int )
{
	operator delete( data );
}