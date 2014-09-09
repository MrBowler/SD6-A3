#include "MemoryManager.hpp"
#include <string>
#include <malloc.h>
#include <Windows.h>
#include "StringFunctions.hpp"


//-----------------------------------------------------------------------------------------------
STATIC byte_t* MemoryManager::m_pool;
STATIC size_t MemoryManager::m_poolSizeInBytes;
STATIC size_t MemoryManager::m_numAllocationsRequested;
STATIC size_t MemoryManager::m_totalNumBytesAllocated;
STATIC size_t MemoryManager::m_currentNumBytesAllocated;
STATIC size_t MemoryManager::m_largestAllocation;


//-----------------------------------------------------------------------------------------------
MemoryManager::MemoryManager()
{

}


//-----------------------------------------------------------------------------------------------
STATIC void MemoryManager::Initialize( size_t poolSizeInBytes )
{
	m_numAllocationsRequested = 0;
	m_totalNumBytesAllocated = 0;
	m_currentNumBytesAllocated = 0;
	m_largestAllocation = 0;

	m_poolSizeInBytes = poolSizeInBytes;
	m_pool = static_cast< byte_t* >( malloc( m_poolSizeInBytes ) );
	if( m_pool == nullptr )
	{
		static const std::bad_alloc nomem;
		_RAISE(nomem);
	}

	MetaData* topMeta = (MetaData*) m_pool;
	topMeta->m_isOccupied = false;
	topMeta->m_blockDataSegmentSize = m_poolSizeInBytes - sizeof( MetaData );
}


//-----------------------------------------------------------------------------------------------
STATIC void MemoryManager::Destruct()
{
	free( m_pool );
	m_pool = nullptr;
	m_poolSizeInBytes = 0;
	m_currentNumBytesAllocated = 0;
}


//-----------------------------------------------------------------------------------------------
STATIC void* MemoryManager::AllocateMemory( size_t objectSizeInBytes )
{
	return AllocateMemory( objectSizeInBytes, nullptr, 0 );
}


//-----------------------------------------------------------------------------------------------
STATIC void* MemoryManager::AllocateMemory( size_t objectSizeInBytes, const char* file, unsigned int line )
{
	++m_numAllocationsRequested;
	m_totalNumBytesAllocated += objectSizeInBytes;
	m_currentNumBytesAllocated += objectSizeInBytes;
	if( objectSizeInBytes > m_largestAllocation )
		m_largestAllocation = objectSizeInBytes;

	//size_t numBytesPassed = 0;
	size_t bytesNeeded = objectSizeInBytes + sizeof( MetaData );
	//MetaData* block = (MetaData*) m_pool;

	MetaData* freeBlock = GetSmallestFreeBlock( bytesNeeded );
	if( freeBlock )
	{
		MetaData* newMeta = (MetaData*) ( reinterpret_cast< byte_t* >( freeBlock ) + bytesNeeded );
		newMeta->m_isOccupied = false;
		newMeta->m_blockDataSegmentSize = freeBlock->m_blockDataSegmentSize - bytesNeeded;
		freeBlock->m_isOccupied = true;
		freeBlock->m_blockDataSegmentSize = objectSizeInBytes;
		freeBlock->m_fileName = file;
		freeBlock->m_lineNumber = line;
		return ( reinterpret_cast< byte_t* >( freeBlock ) + sizeof( MetaData ) );
	}

	/*while( numBytesPassed < m_poolSizeInBytes )
	{
		if( !block->m_isOccupied && block->m_blockDataSegmentSize >= bytesNeeded )
		{
			MetaData* newMeta = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesNeeded );
			newMeta->m_isOccupied = false;
			newMeta->m_blockDataSegmentSize = block->m_blockDataSegmentSize - bytesNeeded;
			block->m_isOccupied = true;
			block->m_blockDataSegmentSize = objectSizeInBytes;
			block->m_fileName = file;
			block->m_lineNumber = line;
			return ( reinterpret_cast< byte_t* >( block ) + sizeof( MetaData ) );
		}
		else
		{
			size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
			block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
			numBytesPassed += bytesToNextBlock;
		}
	}*/

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
STATIC void MemoryManager::FreeMemory( void* data )
{
	if( data == nullptr )
		return;

	MetaData* block = (MetaData*) ( reinterpret_cast< byte_t* >( data ) - sizeof( MetaData ) );
	block->m_isOccupied = false;
	block->m_fileName = nullptr;
	block->m_lineNumber = 0;

	m_currentNumBytesAllocated -= block->m_blockDataSegmentSize;

	size_t numBytesPassed = 0;
	MetaData* blockBefore = (MetaData*) m_pool;
	while( blockBefore != block && numBytesPassed < m_poolSizeInBytes )
	{
		size_t bytesToNextBlock = blockBefore->m_blockDataSegmentSize + sizeof( MetaData );
		numBytesPassed += bytesToNextBlock;
		MetaData* nextBlock = (MetaData*) ( reinterpret_cast< byte_t* >( blockBefore ) + bytesToNextBlock );
		if( nextBlock == block )
			break;

		blockBefore = nextBlock;
	}
	numBytesPassed += block->m_blockDataSegmentSize + sizeof( MetaData );

	if( numBytesPassed < m_poolSizeInBytes )
	{
		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		MetaData* blockAfter = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		if( !blockAfter->m_isOccupied )
			block->m_blockDataSegmentSize += sizeof( MetaData ) + blockAfter->m_blockDataSegmentSize;
	}

	if( blockBefore != block && !blockBefore->m_isOccupied )
		blockBefore->m_blockDataSegmentSize += sizeof( MetaData ) + block->m_blockDataSegmentSize;
}


//-----------------------------------------------------------------------------------------------
STATIC bool MemoryManager::IsMemoryManagerAvailable()
{
	return ( m_pool != nullptr );
}


//-----------------------------------------------------------------------------------------------
STATIC void MemoryManager::CheckForMemoryLeaks()
{
	bool memoryLeakFound = false;
	size_t numBytesPassed = 0;
	MetaData* block = (MetaData*) m_pool;

	while( numBytesPassed < m_poolSizeInBytes )
	{
		if( block->m_isOccupied )
		{
			if( !memoryLeakFound )
			{
				OutputDebugStringA( "\nDetected memory leaks!\n" );
				OutputDebugStringA( "Dumping objects ->\n" );
				memoryLeakFound = true;
			}

			std::string debugString = "";
			if( block->m_fileName )
				debugString = std::string( block->m_fileName ) + "(" + ConvertNumberToString( block->m_lineNumber ) + "): ";
			else
				debugString = "<file not given>(0): ";

			debugString += "normal block at " + ConvertAddressToString( block ) + ", " + ConvertNumberToString( block->m_blockDataSegmentSize ) + " bytes long\n";
			OutputDebugStringA( debugString.c_str() );
		}

		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		numBytesPassed += bytesToNextBlock;
	}

	if( !memoryLeakFound )
		OutputDebugStringA( "No memory leaks detected!" );

	OutputDebugStringA( "\n" );
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetNumberOfAllocationRequest()
{
	return m_numAllocationsRequested;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetTotalNumberOfBytesAllocated()
{
	return m_totalNumBytesAllocated;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetCurrentNumberOfBytesAllocated()
{
	return m_currentNumBytesAllocated;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetLargestAllocationSize()
{
	return m_largestAllocation;
}


//-----------------------------------------------------------------------------------------------
STATIC float MemoryManager::GetAverageAllocationSize()
{
	return (float) ( m_totalNumBytesAllocated ) / (float) ( m_numAllocationsRequested );
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetLargestFreeBlockSize()
{
	MetaData* largestBlock = GetLargestFreeBlock();
	if( largestBlock == nullptr )
		return 0;

	return largestBlock->m_blockDataSegmentSize;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetSmallestFreeBlockSize()
{
	MetaData* smallestBlock = GetSmallestFreeBlock();
	if( smallestBlock == nullptr )
		return 0;

	return smallestBlock->m_blockDataSegmentSize;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetLargestFreeBlockSize( size_t maxBlockSize )
{
	MetaData* largestBlock = GetLargestFreeBlock( maxBlockSize );
	if( largestBlock == nullptr )
		return 0;

	return largestBlock->m_blockDataSegmentSize;
}


//-----------------------------------------------------------------------------------------------
STATIC size_t MemoryManager::GetSmallestFreeBlockSize( size_t minBlockSize )
{
	MetaData* smallestBlock = GetSmallestFreeBlock( minBlockSize );
	if( smallestBlock == nullptr )
		return 0;

	return smallestBlock->m_blockDataSegmentSize;
}


//-----------------------------------------------------------------------------------------------
STATIC MetaData* MemoryManager::GetLargestFreeBlock()
{
	size_t numBytesPassed = 0;
	MetaData* block = (MetaData*) m_pool;
	MetaData* largestBlock = block;

	while( numBytesPassed < m_poolSizeInBytes )
	{
		if( largestBlock == nullptr && !block->m_isOccupied )
			largestBlock = block;
		else if( largestBlock != nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize > largestBlock->m_blockDataSegmentSize )
			largestBlock = block;

		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		numBytesPassed += bytesToNextBlock;
	}

	return largestBlock;
}


//-----------------------------------------------------------------------------------------------
STATIC MetaData* MemoryManager::GetSmallestFreeBlock()
{
	size_t numBytesPassed = 0;
	MetaData* block = (MetaData*) m_pool;
	MetaData* smallestBlock = block;

	while( numBytesPassed < m_poolSizeInBytes )
	{
		if( smallestBlock == nullptr && !block->m_isOccupied )
			smallestBlock = block;
		else if( smallestBlock != nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize < smallestBlock->m_blockDataSegmentSize && block->m_blockDataSegmentSize > 0 )
			smallestBlock = block;

		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		numBytesPassed += bytesToNextBlock;
	}

	return smallestBlock;
}


//-----------------------------------------------------------------------------------------------
STATIC MetaData* MemoryManager::GetLargestFreeBlock( size_t maxBlockSize )
{
	size_t numBytesPassed = 0;
	MetaData* block = (MetaData*) m_pool;
	MetaData* largestBlock = block;

	while( numBytesPassed < m_poolSizeInBytes )
	{
		if( largestBlock == nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize < maxBlockSize )
			largestBlock = block;
		else if( largestBlock != nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize > largestBlock->m_blockDataSegmentSize && block->m_blockDataSegmentSize < maxBlockSize )
			largestBlock = block;

		if( largestBlock != nullptr && largestBlock->m_blockDataSegmentSize == maxBlockSize )
			return largestBlock;

		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		numBytesPassed += bytesToNextBlock;
	}

	return largestBlock;
}


//-----------------------------------------------------------------------------------------------
STATIC MetaData* MemoryManager::GetSmallestFreeBlock( size_t minBlockSize )
{
	size_t numBytesPassed = 0;
	MetaData* block = (MetaData*) m_pool;
	MetaData* smallestBlock = nullptr;

	while( numBytesPassed < m_poolSizeInBytes )
	{
		if( smallestBlock == nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize > minBlockSize )
			smallestBlock = block;
		else if( smallestBlock != nullptr && !block->m_isOccupied && block->m_blockDataSegmentSize < smallestBlock->m_blockDataSegmentSize && block->m_blockDataSegmentSize > minBlockSize )
			smallestBlock = block;

		if( smallestBlock != nullptr && smallestBlock->m_blockDataSegmentSize == minBlockSize )
			return smallestBlock;

		size_t bytesToNextBlock = block->m_blockDataSegmentSize + sizeof( MetaData );
		block = (MetaData*) ( reinterpret_cast< byte_t* >( block ) + bytesToNextBlock );
		numBytesPassed += bytesToNextBlock;
	}

	return smallestBlock;
}