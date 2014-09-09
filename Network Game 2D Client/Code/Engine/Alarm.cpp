#include "Alarm.hpp"
#include "EventSystem.hpp"
#include "MathFunctions.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
Alarm::Alarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName )
	: m_secondsBeforeFire( secondsBeforeFire )
	, m_secondsElapsed( 0.0 )
	, m_isLooping( loopAlarm )
	, m_removeThisAlarm( false )
	, m_eventName( eventToFireName )
{

}


//-----------------------------------------------------------------------------------------------
Alarm::Alarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName, const NamedProperties& eventParams )
	: m_secondsBeforeFire( secondsBeforeFire )
	, m_secondsElapsed( 0.0 )
	, m_isLooping( loopAlarm )
	, m_removeThisAlarm( false )
	, m_eventName( eventToFireName )
	, m_eventParams( eventParams )
{

}


//-----------------------------------------------------------------------------------------------
float Alarm::GetPercentElapsed() const
{
	float percentElapsed = 100.f * static_cast< float >( m_secondsElapsed / m_secondsBeforeFire );
	return ClampFloat( percentElapsed, 0.f, 100.f );
}


//-----------------------------------------------------------------------------------------------
float Alarm::GetPercentRemaining() const
{
	double secondsRemaining = GetSecondsRemaining();
	float percentRemaining = 100.f * static_cast< float >( secondsRemaining / m_secondsBeforeFire );
	return ClampFloat( percentRemaining, 0.f, 100.f );
}


//-----------------------------------------------------------------------------------------------
double Alarm::GetSecondsElapsed() const
{
	return m_secondsElapsed;
}


//-----------------------------------------------------------------------------------------------
double Alarm::GetSecondsRemaining() const
{
	double secondsRemaining = m_secondsBeforeFire - m_secondsElapsed;
	if( secondsRemaining < 0.0 )
		secondsRemaining = 0.0;

	return secondsRemaining;
}


//-----------------------------------------------------------------------------------------------
std::string Alarm::GetEventName() const
{
	return m_eventName;
}


//-----------------------------------------------------------------------------------------------
bool Alarm::CheckIfAlarmShouldBeRemoved() const
{
	return m_removeThisAlarm;
}


//-----------------------------------------------------------------------------------------------
void Alarm::AdvanceTime( double deltaSeconds )
{
	if( m_secondsElapsed > m_secondsBeforeFire )
	{
		EventSystem::FireEvent( m_eventName, m_eventParams );

		if( m_isLooping )
			m_secondsElapsed -= m_secondsBeforeFire;
		else
			m_removeThisAlarm = true;
	}

	m_secondsElapsed += deltaSeconds;
}