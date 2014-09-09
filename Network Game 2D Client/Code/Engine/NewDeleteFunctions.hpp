#ifndef include_NewDeleteFunctions
#define include_NewDeleteFunctions
#pragma once

//-----------------------------------------------------------------------------------------------
void* operator new( size_t size );
void operator delete( void* data );
void* operator new[]( size_t size );
void operator delete[]( void* data );
void* operator new( size_t size, const char* file, unsigned int line );
void operator delete( void* data, const char* file, unsigned int line );
void* operator new[]( size_t size, const char* file, unsigned int line );
void operator delete[]( void* data, const char* file, unsigned int line );


#endif // include_NewDeleteFunctions