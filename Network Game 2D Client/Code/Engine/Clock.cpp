#include "Clock.hpp"
#include "EngineCommon.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
STATIC Clock Clock::s_masterClock( 0.0 );


//-----------------------------------------------------------------------------------------------
Clock::Clock( double maxDeltaTimeStep )
	: m_parent( &s_masterClock )
	, m_firstChild( nullptr )
	, m_nextSibling( nullptr )
	, m_isPaused( false )
	, m_timeScale( 1.f )
	, m_currentTime( 0.0 )
	, m_maxDeltaTimeStep( maxDeltaTimeStep )
	, m_mostRecentDeltaTimeStep( 0.0 )
{
	if( this == &s_masterClock )
	{
		m_parent = nullptr;
		return;
	}

	s_masterClock.AddChild( this );
}


//-----------------------------------------------------------------------------------------------
Clock::Clock( Clock* parentClock, double maxDeltaTimeStep )
	: m_parent( parentClock )
	, m_firstChild( nullptr )
	, m_nextSibling( nullptr )
	, m_isPaused( false )
	, m_timeScale( 1.f )
	, m_currentTime( 0.0 )
	, m_maxDeltaTimeStep( maxDeltaTimeStep )
	, m_mostRecentDeltaTimeStep( 0.0 )
{
	if( m_parent == nullptr )
		m_parent = &s_masterClock;

	m_parent->AddChild( this );
}


//-----------------------------------------------------------------------------------------------
Clock::~Clock()
{
	RemoveFromParentChildList();

	if( m_firstChild != nullptr )
	{
		delete m_firstChild;
		m_firstChild = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
STATIC void Clock::AdvanceTime( double deltaSeconds )
{
	s_masterClock.AdvanceIndividualClockTime( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}


//-----------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
}


//-----------------------------------------------------------------------------------------------
void Clock::TogglePauseState()
{
	m_isPaused = !m_isPaused;
}


//-----------------------------------------------------------------------------------------------
void Clock::AddAlarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName )
{
	Alarm alarm( secondsBeforeFire, loopAlarm, eventToFireName );
	m_alarms.push_back( alarm );
}


//-----------------------------------------------------------------------------------------------
void Clock::AddAlarm( double secondsBeforeFire, bool loopAlarm, const std::string& eventToFireName, const NamedProperties& eventParams )
{
	Alarm alarm( secondsBeforeFire, loopAlarm, eventToFireName, eventParams );
	m_alarms.push_back( alarm );
}


//-----------------------------------------------------------------------------------------------
float Clock::GetAlarmPercentElapsed( const std::string& alarmEventName )
{
	Alarm* alarm = GetAlarmByEventName( alarmEventName );
	if( alarm == nullptr )
		return 0.f;

	return alarm->GetPercentElapsed();
}


//-----------------------------------------------------------------------------------------------
float Clock::GetAlarmPercentRemaining( const std::string& alarmEventName )
{
	Alarm* alarm = GetAlarmByEventName( alarmEventName );
	if( alarm == nullptr )
		return 0.f;

	return alarm->GetPercentRemaining();
}


//-----------------------------------------------------------------------------------------------
double Clock::GetAlarmSecondsElapsed( const std::string& alarmEventName )
{
	Alarm* alarm = GetAlarmByEventName( alarmEventName );
	if( alarm == nullptr )
		return 0.0;

	return alarm->GetSecondsElapsed();
}


//-----------------------------------------------------------------------------------------------
double Clock::GetAlarmSecondsRemaining( const std::string& alarmEventName )
{
	Alarm* alarm = GetAlarmByEventName( alarmEventName );
	if( alarm == nullptr )
		return 0.0;

	return alarm->GetSecondsRemaining();
}


//-----------------------------------------------------------------------------------------------
void Clock::ChangeTimeScale( float newTimeScale )
{
	m_timeScale = newTimeScale;
}


//-----------------------------------------------------------------------------------------------
void Clock::ChangeMaxDeltaTimeStep( double newMaxDeltaTimeStep )
{
	m_maxDeltaTimeStep = newMaxDeltaTimeStep;
}


//-----------------------------------------------------------------------------------------------
void Clock::AddChild( Clock* childClock )
{
	if( m_firstChild == nullptr )
	{
		m_firstChild = childClock; 
		return;
	}

	bool childAdded = false;
	Clock* previousChild = nullptr;
	Clock* currentChild = m_firstChild;

	while( !childAdded )
	{
		if( currentChild != nullptr )
		{
			previousChild = currentChild;
			currentChild = currentChild->m_nextSibling;
		}
		else
		{
			previousChild->m_nextSibling = childClock;
			childAdded = true;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Clock::RemoveFromParentChildList()
{
	if( this->m_parent == nullptr )
		return;

	Clock* childClock = this->m_parent->m_firstChild;
	if( childClock == this )
	{
		this->m_parent->m_firstChild = childClock->m_nextSibling;
		return;
	}

	while( childClock != nullptr )
	{
		if( childClock->m_nextSibling == this )
		{
			childClock->m_nextSibling = this->m_nextSibling;
			return;
		}

		childClock = childClock->m_nextSibling;
	}
}


//-----------------------------------------------------------------------------------------------
Alarm* Clock::GetAlarmByEventName( const std::string& alarmEventName )
{
	for( unsigned int alarmIndex = 0; alarmIndex < m_alarms.size(); ++alarmIndex )
	{
		Alarm* alarm = &m_alarms[ alarmIndex ];
		if( alarmEventName == alarm->GetEventName() )
			return alarm;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void Clock::AdvanceIndividualClockTime( double deltaSecondsGiven )
{
	double deltaSecondsToAdvance = deltaSecondsGiven;

	if( m_maxDeltaTimeStep != 0.0 && deltaSecondsToAdvance > m_maxDeltaTimeStep )
		deltaSecondsToAdvance = m_maxDeltaTimeStep;

	if( m_isPaused )
		deltaSecondsToAdvance = 0.0;

	deltaSecondsToAdvance *= static_cast< double >( m_timeScale );

	m_currentTime += deltaSecondsToAdvance;
	m_mostRecentDeltaTimeStep = deltaSecondsToAdvance;

	for( unsigned int alarmIndex = 0; alarmIndex < m_alarms.size(); ++alarmIndex )
	{
		Alarm* alarm = &m_alarms[ alarmIndex ];
		alarm->AdvanceTime( deltaSecondsToAdvance );
		if( alarm->CheckIfAlarmShouldBeRemoved() )
		{
			m_alarms.erase( m_alarms.begin() + alarmIndex );
			--alarmIndex;
		}
	}

	if( m_firstChild != nullptr )
		m_firstChild->AdvanceIndividualClockTime( deltaSecondsToAdvance );
	if( m_nextSibling != nullptr )
		m_nextSibling->AdvanceIndividualClockTime( deltaSecondsGiven );
}