#ifndef include_XMLDocument
#define include_XMLDocument
#pragma once

//-----------------------------------------------------------------------------------------------
#include "pugixml.hpp"
#include "XMLNode.hpp"


//-----------------------------------------------------------------------------------------------
class XMLDocument
{
public:
	XMLDocument() {}
	XMLDocument( const std::string xmlFileName );
	std::string GetFileName() const;
	XMLNode GetRootNode() const;
	XMLNode GetCurrentNode() const;
	void SetCurrentNode( const XMLNode& node );
	void SetCurrentNodeToRoot();
	void SetCurrentNodeToPreviousNode();
	void SetCurrentNodeToParent();
	void SetCurrentNodeToChild( const std::string& childName );
	void SetCurrentNodeToFirstChild();
	void SetCurrentNodeToNextSibling();

private:
	std::string			m_fileName;
	pugi::xml_document	m_document;
	XMLNode				m_currentNode;
	XMLNode				m_previousNode;
};


#endif // include_XMLDocument