#ifndef include_Alarm
#define include_Alarm
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include "NamedProperties.hpp"


//-----------------------------------------------------------------------------------------------
class Alarm
{
public:
	Alarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName );
	Alarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName, const NamedProperties& eventParams );
	float GetPercentElapsed() const;
	float GetPercentRemaining() const;
	double GetSecondsElapsed() const;
	double GetSecondsRemaining() const;
	std::string GetEventName() const;
	bool CheckIfAlarmShouldBeRemoved() const;
	void AdvanceTime( double deltaSeconds );

private:
	std::string		m_eventName;
	NamedProperties	m_eventParams;
	bool			m_isLooping;
	bool			m_removeThisAlarm;
	double			m_secondsElapsed;
	double			m_secondsBeforeFire;
};


#endif // include_Alarm