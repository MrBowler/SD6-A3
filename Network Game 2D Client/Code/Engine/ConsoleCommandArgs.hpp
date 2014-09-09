#ifndef include_ConsoleCommandArgs
#define include_ConsoleCommandArgs
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class ConsoleCommandArgs
{
public:
	ConsoleCommandArgs() {}
	ConsoleCommandArgs( const std::string& argsString );

	std::string					m_entireArgsString;
	std::vector<std::string>	m_argsList;
};


#endif // include_ConsoleCommandArgs