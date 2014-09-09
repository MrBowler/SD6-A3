#ifndef include_Player
#define include_Player
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Color3b.hpp"
#include "../Engine/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
struct Player
{
	Color3b			m_color;
	Vector2			m_position;
	Vector2			m_velocity;
	float			m_orientationDegrees;
	double			m_lastUpdateTime;
};



#endif // include_Player