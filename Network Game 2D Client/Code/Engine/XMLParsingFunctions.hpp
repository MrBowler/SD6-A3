#ifndef include_XMLParsingFunctions
#define include_XMLParsingFunctions
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <windows.h>
#include "XMLNode.hpp"
#include "XMLAttribute.hpp"
#include "XMLDocument.hpp"
#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "IntVector2.hpp"
#include "IntVector3.hpp"
#include "StringFunctions.hpp"


//-----------------------------------------------------------------------------------------------
int GetIntXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, int defaultValueIfNotFound );
unsigned int GetUnsignedIntXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, unsigned int defaultValueIfNotFound );
float GetFloatXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, float defaultValueIfNotFound );
double GetDoubleXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, double defaultValueIfNotFound );
bool GetBoolXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, bool defaultValueIfNotFound );
char GetCharXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, char defaultValueIfNotFound );
std::string GetStringXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const std::string& defaultValueIfNotFound );
Color GetColorXMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const Color& defaultValueIfNotFound );
Vector2 GetVector2XMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const Vector2& defaultValueIfNotFound );
Vector3 GetVector3XMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const Vector3& defaultValueIfNotFound );
Vector4 GetVector4XMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const Vector4& defaultValueIfNotFound );
IntVector2 GetIntVector2XMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const IntVector2& defaultValueIfNotFound );
IntVector3 GetIntVector3XMLAttribute( const XMLDocument& xmlDoc, const std::string& attributeName, const IntVector3& defaultValueIfNotFound );
void ValidateXMLAttributes( const XMLDocument& xmlDoc, const std::string& requiredAttributeNames, const std::string& optionalAttributeNames );
void ValidateXMLChildElements( const XMLDocument& xmlDoc, const std::string& requiredChildElementNames, const std::string& optionalChildElementNames );


#endif // include_XMLParsingFunctions