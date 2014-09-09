#ifndef include_Color3b
#define include_Color3b
#pragma once

//-----------------------------------------------------------------------------------------------
struct Color3b
{
	Color3b() : r( 0 ), g( 0 ), b( 0 ) {}
	Color3b( unsigned char initialR, unsigned char initialG, unsigned char initialB ) : r( initialR ), g( initialG ), b( initialB ) {}

	unsigned char r;
	unsigned char g;
	unsigned char b;
};


#endif // include_Color3b