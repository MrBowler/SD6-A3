#ifndef include_XMLNode
#define include_XMLNode
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include "pugixml.hpp"
#include "XMLAttribute.hpp"


//-----------------------------------------------------------------------------------------------
class XMLNode
{
public:
	XMLNode() {}
	XMLNode( const pugi::xml_node& node ) : m_node( node ) {}
	std::string GetNodeName() const;
	XMLNode GetParent() const;
	XMLNode GetChild( const std::string childName ) const;
	XMLNode GetFirstChild() const;
	int GetNumberOfChildNodes() const;
	XMLNode GetNextSibling() const;
	XMLAttribute GetAttributeByName( const std::string& attributeName ) const;
	std::vector<XMLAttribute> GetAllAttributes() const;
	std::vector<XMLNode> GetAllChildren() const;
	bool IsNull() const;

private:
	pugi::xml_node	m_node;
};


#endif // include_XMLNode