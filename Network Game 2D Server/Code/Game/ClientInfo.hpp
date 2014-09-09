#ifndef include_ClientInfo
#define include_ClientInfo
#pragma once

//-----------------------------------------------------------------------------------------------
struct ClientInfo
{
	bool operator<( const ClientInfo& info ) const;

	char*			m_ipAddress;
	unsigned short	m_portNumber;
};


//-----------------------------------------------------------------------------------------------
bool ClientInfo::operator<( const ClientInfo& info ) const
{
	if( m_ipAddress < info.m_ipAddress )
		return true;
	else if( m_ipAddress == info.m_ipAddress )
	{
		if( m_portNumber < info.m_portNumber )
			return true;
		else
			return false;
	}

	return false;
}


#endif // include_ClientInfo