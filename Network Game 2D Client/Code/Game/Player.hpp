#ifndef include_Player
#define include_Player
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Color3b.hpp"
#include "../Engine/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
struct Player
{
	Color3b		m_color;
	Vector2		m_currentPosition;
	Vector2		m_previousPosition;
	Vector2		m_currentVelocity;
	Vector2		m_previousVelocity;
	float		m_orientationDegrees;
	float		m_secondsSinceLastUpdate;
};



#endif // include_Player