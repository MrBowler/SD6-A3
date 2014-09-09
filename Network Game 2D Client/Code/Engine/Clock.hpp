#ifndef include_Clock
#define include_Clock
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include "Alarm.hpp"


//-----------------------------------------------------------------------------------------------
class Clock
{
public:
	Clock( double maxDeltaTimeStep );
	Clock( Clock* parentClock, double maxDeltaTimeStep );
	~Clock();
	static void AdvanceTime( double deltaSecondsGiven );
	void Pause();
	void Unpause();
	void TogglePauseState();
	void AddAlarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName );
	void AddAlarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName, const NamedProperties& eventParams );
	void ChangeTimeScale( float newTimeScale );
	void ChangeMaxDeltaTimeStep( double newMaxDeltaTimeStep );
	float GetAlarmPercentElapsed( const std::string& alarmEventName );
	float GetAlarmPercentRemaining( const std::string& alarmEventName );
	double GetAlarmSecondsElapsed( const std::string& alarmEventName );
	double GetAlarmSecondsRemaining( const std::string& alarmEventName );
	
private:
	void AddChild( Clock* childClock );
	void RemoveFromParentChildList();
	Alarm* GetAlarmByEventName( const std::string& alarmEventName );
	void AdvanceIndividualClockTime( double deltaSeconds );

	static Clock			s_masterClock;
	Clock*					m_parent;
	Clock*					m_nextSibling;
	Clock*					m_firstChild;
	std::vector< Alarm >	m_alarms;
	bool					m_isPaused;
	float					m_timeScale;
	double					m_currentTime;
	double					m_maxDeltaTimeStep;
	double					m_mostRecentDeltaTimeStep;
};


#endif // include_Clock