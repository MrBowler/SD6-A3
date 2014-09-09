#include "NamedProperties.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
NamedProperties::~NamedProperties()
{
	std::map< std::string, NamedPropertiesBase* >::iterator mapIter;
	for( mapIter = m_properties.begin(); mapIter != m_properties.end(); ++mapIter )
	{
		delete mapIter->second;
	}

	m_properties.clear();
}


//-----------------------------------------------------------------------------------------------
void NamedProperties::SetProperty( const std::string& propertyName, const char* typedDataValue )
{
	SetProperty( propertyName, std::string( typedDataValue ) );
}