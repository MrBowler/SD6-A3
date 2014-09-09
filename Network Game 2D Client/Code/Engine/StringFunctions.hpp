#ifndef include_StringFunctions
#define include_StringFunctions
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <sstream>
#include <algorithm>


//-----------------------------------------------------------------------------------------------
std::string GetStringWithNoWhitespace( const std::string& str );
std::string GetLowercaseString( const std::string& str );
std::string GetUppercaseString( const std::string& str );
std::string ConvertNumberToString( int number );
std::string ConvertNumberToString( size_t number );
std::string ConvertNumberToString( float number );
std::string ConvertNumberToString( double number );
std::string ConvertAddressToString( void* ptr );
std::vector<std::string> GetVectorOfStringsFromSingleString( const std::string& listString, const char separatorChar );
std::string GetShortenedFileName( const std::string& fileName );


#endif // include_StringFunctions