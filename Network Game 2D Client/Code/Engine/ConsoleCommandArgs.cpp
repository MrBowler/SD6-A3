#include "ConsoleCommandArgs.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
ConsoleCommandArgs::ConsoleCommandArgs( const std::string& argsString )
	: m_entireArgsString( argsString )
{
	unsigned int charIndex = 0;
	while( charIndex < argsString.size() )
	{
		if( argsString[ charIndex ] == '\"' )
		{
			++charIndex;
			std::string quoteArg = "";

			while( charIndex < argsString.size() && argsString[ charIndex ] != '\"' )
			{
				quoteArg += argsString[ charIndex ];
				++charIndex;
			}

			if( charIndex == argsString.size() && quoteArg[ argsString.size() - 1 ] != '\"' )
				continue;

			m_argsList.push_back( quoteArg );
			++charIndex;
			continue;
		}

		if( argsString[ charIndex ] != ' ' )
		{
			std::string spaceArg = "";
			while( charIndex < argsString.size() && argsString[ charIndex ] != ' ' )
			{
				spaceArg += argsString[ charIndex ];
				++charIndex;
			}

			m_argsList.push_back( spaceArg );
			++charIndex;
			continue;
		}

		++charIndex;
	}
}