#ifndef include_ProfileSection
#define include_ProfileSection
#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include "BitmapFont.hpp"


//-----------------------------------------------------------------------------------------------
const float PROFILE_SECTION_FONT_CELL_HEIGHT = 30.f;
const std::string PROFILE_SECTION_FONT_GLYPH_SHEET_FILE_NAME = "../Data/Fonts/MainFont_EN_00.png";
const std::string PROFILE_SECTION_FONT_META_DATA_FILE_NAME = "../Data/Fonts/MainFont_EN.FontDef.xml";


//-----------------------------------------------------------------------------------------------
struct ProfileSectionInfo
{
	ProfileSectionInfo() : m_totalSeconds( 0.0 ), m_lastProfileSeconds( 0.0 ), m_totalNumProfiles ( 0 ) {}

	double			m_totalSeconds;
	double			m_lastProfileSeconds;
	int				m_totalNumProfiles;
};


//-----------------------------------------------------------------------------------------------
class ProfileSection
{
public:
	ProfileSection( const std::string& profileName );
	virtual void StartProfiling();
	virtual void StopProfiling();
	static void RenderProfileInfo( const Vector2& windowDimensions );

protected:
	ProfileSection() {}

	static std::map< std::string, ProfileSectionInfo* >	s_profileSections;

private:
	ProfileSectionInfo*	m_profileInfo;
	double				m_profileStartTime;
	static BitmapFont	s_font;
};


//-----------------------------------------------------------------------------------------------
class ScopedProfileSection : public ProfileSection
{
public:
	ScopedProfileSection( const std::string& profileName );
	~ScopedProfileSection();
	void StartProfiling() {}
	void StopProfiling() {}

protected:
	ProfileSectionInfo*	m_profileInfo;
	bool				m_startedProfiling;
	double				m_profileStartTime;
};


#endif // include_ProfileSection