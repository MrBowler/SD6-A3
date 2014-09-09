#include "ProfileSection.hpp"
#include "Time.hpp"
#include "EngineCommon.hpp"
#include "OpenGLRenderer.hpp"
#include "StringFunctions.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BitmapFont ProfileSection::s_font;
STATIC std::map< std::string, ProfileSectionInfo* > ProfileSection::s_profileSections;


//-----------------------------------------------------------------------------------------------
ProfileSection::ProfileSection( const std::string& profileName )
	: m_profileStartTime( 0.0 )
{
	std::map< std::string, ProfileSectionInfo* >::iterator mapIter = s_profileSections.find( profileName );
	if( mapIter != s_profileSections.end() )
		m_profileInfo = mapIter->second;
	else
	{
		m_profileInfo = new ProfileSectionInfo();
		s_profileSections[ profileName ] = m_profileInfo;
	}
}


//-----------------------------------------------------------------------------------------------
void ProfileSection::StartProfiling()
{
	m_profileStartTime = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void ProfileSection::StopProfiling()
{
	if( m_profileStartTime == 0.0 )
		return;

	double profileStopTime = GetCurrentTimeSeconds();
	double profileSeconds = profileStopTime - m_profileStartTime;
	m_profileInfo->m_lastProfileSeconds = profileSeconds;
	m_profileInfo->m_totalSeconds += profileSeconds;
	m_profileInfo->m_totalNumProfiles += 1;

	m_profileStartTime = 0.0;
}


//-----------------------------------------------------------------------------------------------
STATIC void ProfileSection::RenderProfileInfo( const Vector2& windowDimensions )
{
	if( s_font.m_glyphSheet == nullptr )
		s_font = BitmapFont( PROFILE_SECTION_FONT_GLYPH_SHEET_FILE_NAME, PROFILE_SECTION_FONT_META_DATA_FILE_NAME );

	OpenGLRenderer::LoadMatrix();
	OpenGLRenderer::SetColor4f( 0.1f, 0.1f, 0.1f, 0.9f );
	OpenGLRenderer::BeginRender( QUADS );
	{
		OpenGLRenderer::SetVertex2f( 0.f, 0.f );
		OpenGLRenderer::SetVertex2f( windowDimensions.x, 0.f );
		OpenGLRenderer::SetVertex2f( windowDimensions.x, windowDimensions.y );
		OpenGLRenderer::SetVertex2f( 0.f, windowDimensions.y );
	}
	OpenGLRenderer::EndRender();

	int profileCount = 0;
	std::map< std::string, ProfileSectionInfo* >::iterator mapIter;
	for( mapIter = s_profileSections.begin(); mapIter != s_profileSections.end(); ++mapIter )
	{
		ProfileSectionInfo* profileInfo = mapIter->second;
		if( profileInfo == nullptr )
			continue;

		double averageTime = 0.0;
		if( profileInfo->m_totalNumProfiles != 0 )
			averageTime = profileInfo->m_totalSeconds / static_cast< double >( profileInfo->m_totalNumProfiles );
		
		std::string totalTimeText = "Last Profile Seconds: " + ConvertNumberToString( profileInfo->m_lastProfileSeconds );
		std::string avgTimeText = "Average Seconds: " + ConvertNumberToString( averageTime );
		float textPositionY = ( windowDimensions.y - 50.f ) - ( profileCount * 3.f * PROFILE_SECTION_FONT_CELL_HEIGHT );
		
		OpenGLRenderer::RenderText( mapIter->first, s_font, PROFILE_SECTION_FONT_CELL_HEIGHT, Vector2( 30.f, textPositionY ) );
		OpenGLRenderer::RenderText( totalTimeText, s_font, PROFILE_SECTION_FONT_CELL_HEIGHT, Vector2( 50.f, textPositionY - PROFILE_SECTION_FONT_CELL_HEIGHT ) );
		OpenGLRenderer::RenderText( avgTimeText, s_font, PROFILE_SECTION_FONT_CELL_HEIGHT, Vector2( 50.f, textPositionY - ( 2.f * PROFILE_SECTION_FONT_CELL_HEIGHT ) ) );
	
		++profileCount;
	}
}


//-----------------------------------------------------------------------------------------------
ScopedProfileSection::ScopedProfileSection( const std::string& profileName )
{
	m_profileStartTime = GetCurrentTimeSeconds();

	std::map< std::string, ProfileSectionInfo* >::iterator mapIter = s_profileSections.find( profileName );
	if( mapIter != s_profileSections.end() )
		m_profileInfo = mapIter->second;
	else
	{
		m_profileInfo = new ProfileSectionInfo();
		s_profileSections[ profileName ] = m_profileInfo;
	}
}


//-----------------------------------------------------------------------------------------------
ScopedProfileSection::~ScopedProfileSection()
{
	double profileStopTime = GetCurrentTimeSeconds();
	double profileSeconds = profileStopTime - m_profileStartTime;
	m_profileInfo->m_lastProfileSeconds = profileSeconds;
	m_profileInfo->m_totalSeconds += profileSeconds;
	m_profileInfo->m_totalNumProfiles += 1;
}