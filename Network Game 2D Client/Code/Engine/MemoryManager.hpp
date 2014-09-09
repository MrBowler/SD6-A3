#ifndef include_MemoryManager
#define include_MemoryManager
#pragma once

//-----------------------------------------------------------------------------------------------
#include "EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
const size_t POOL_MEMORY_IN_BYTES = 1024 * 1024 * 512;


//-----------------------------------------------------------------------------------------------
struct MetaData
{
	size_t			m_blockDataSegmentSize;
	bool			m_isOccupied;
	const char*		m_fileName;
	unsigned int	m_lineNumber;
};


//-----------------------------------------------------------------------------------------------
class MemoryManager
{
public:
	MemoryManager();
	static void Initialize( size_t poolSizeInBytes );
	static void Destruct();
	static void* AllocateMemory( size_t objectSizeInBytes );
	static void* AllocateMemory( size_t objectSizeInBytes, const char* file, unsigned int line );
	static void FreeMemory( void* data );
	static bool IsMemoryManagerAvailable();
	static void CheckForMemoryLeaks();
	static size_t GetNumberOfAllocationRequest();
	static size_t GetTotalNumberOfBytesAllocated();
	static size_t GetCurrentNumberOfBytesAllocated();
	static size_t GetLargestAllocationSize();
	static float GetAverageAllocationSize();
	static size_t GetLargestFreeBlockSize();
	static size_t GetSmallestFreeBlockSize();
	static size_t GetLargestFreeBlockSize( size_t maxBlockSize );
	static size_t GetSmallestFreeBlockSize( size_t minBlockSize );

private:
	static MetaData* GetLargestFreeBlock();
	static MetaData* GetSmallestFreeBlock();
	static MetaData* GetLargestFreeBlock( size_t maxBlockSize );
	static MetaData* GetSmallestFreeBlock( size_t minBlockSize );

	static byte_t*	m_pool;
	static size_t	m_poolSizeInBytes;
	static size_t	m_numAllocationsRequested;
	static size_t	m_totalNumBytesAllocated;
	static size_t	m_currentNumBytesAllocated;
	static size_t	m_largestAllocation;
};


#endif // include_MemoryManager