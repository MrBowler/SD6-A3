#include "XMLNode.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
std::string XMLNode::GetNodeName() const
{
	return m_node.name();
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLNode::GetParent() const
{
	return XMLNode( m_node.parent() );
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLNode::GetChild( const std::string childName ) const
{
	return XMLNode( m_node.child( childName.c_str() ) );
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLNode::GetFirstChild() const
{
	return XMLNode( m_node.first_child() );
}


//-----------------------------------------------------------------------------------------------
int XMLNode::GetNumberOfChildNodes() const
{
	return std::distance( m_node.begin(), m_node.end() );
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLNode::GetNextSibling() const
{
	return XMLNode( m_node.next_sibling() );
}


//-----------------------------------------------------------------------------------------------
XMLAttribute XMLNode::GetAttributeByName( const std::string& attributeName ) const
{
	XMLAttribute attribute( m_node.attribute( attributeName.c_str() ) );
	return attribute;
}


//-----------------------------------------------------------------------------------------------
std::vector<XMLAttribute> XMLNode::GetAllAttributes() const
{
	pugi::xml_object_range<pugi::xml_attribute_iterator> attributeList = m_node.attributes();
	std::vector<XMLAttribute> returnVec;

	for( pugi::xml_attribute_iterator attribIter = attributeList.begin(); attribIter != attributeList.end(); ++attribIter )
	{
		returnVec.push_back( *attribIter );
	}

	return returnVec;
}


//-----------------------------------------------------------------------------------------------
std::vector<XMLNode> XMLNode::GetAllChildren() const
{
	pugi::xml_object_range<pugi::xml_node_iterator> childList = m_node.children();
	std::vector<XMLNode> returnVec;

	for( pugi::xml_node_iterator childIter = childList.begin(); childIter != childList.end(); ++childIter )
	{
		returnVec.push_back( *childIter );
	}

	return returnVec;
}


//-----------------------------------------------------------------------------------------------
bool XMLNode::IsNull() const
{
	return m_node == nullptr;
}