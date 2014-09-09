#include "XMLDocument.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
XMLDocument::XMLDocument( const std::string xmlFileName )
	: m_fileName( xmlFileName )
{
	pugi::xml_parse_result result = m_document.load_file( xmlFileName.c_str() );
	if( result.status != pugi::status_ok )
		return;

	m_currentNode = XMLNode( m_document.root() );
	m_previousNode = m_currentNode;
}


//-----------------------------------------------------------------------------------------------
std::string XMLDocument::GetFileName() const
{
	return m_fileName;
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLDocument::GetRootNode() const
{
	return m_document.root();
}


//-----------------------------------------------------------------------------------------------
XMLNode XMLDocument::GetCurrentNode() const
{
	return m_currentNode;
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNode( const XMLNode& node )
{
	m_previousNode = m_currentNode;
	m_currentNode = node;
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToRoot()
{
	m_previousNode = m_currentNode;
	m_currentNode = m_document.root();
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToPreviousNode()
{
	XMLNode tempNode = m_previousNode;
	m_previousNode = m_currentNode;
	m_currentNode = tempNode;
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToParent()
{
	m_previousNode = m_currentNode;
	m_currentNode = m_currentNode.GetParent();
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToChild( const std::string& childName )
{
	m_previousNode = m_currentNode;
	m_currentNode = m_currentNode.GetChild( childName );
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToFirstChild()
{
	m_previousNode = m_currentNode;
	m_currentNode = m_currentNode.GetFirstChild();
}


//-----------------------------------------------------------------------------------------------
void XMLDocument::SetCurrentNodeToNextSibling()
{
	m_previousNode = m_currentNode;
	m_currentNode = m_currentNode.GetNextSibling();
}