#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <WinSock2.h>
#include "Player.hpp"
#include "Color3b.hpp"
#include "CS6Packet.hpp"
#include "ClientInfo.hpp"
#include "../Engine/Time.hpp"
#pragma comment(lib,"ws2_32.lib")

//-----------------------------------------------------------------------------------------------
const unsigned short PORT_NUMBER = 5000;
const int MAP_SIZE_WIDTH = 500;
const int MAP_SIZE_HEIGHT = 500;
const double SECONDS_BEFORE_SEND_UPDATE = 0.0045;
const double SECONDS_BEFORE_RESEND_RELIABLE_PACKETS = 0.25;


//-----------------------------------------------------------------------------------------------
bool g_isQuitting = false;
WSADATA g_wsaData;
SOCKET	g_socket;
double g_secondsSinceLastUpdate;
double g_secondsSinceLastReliableSend;
struct sockaddr_in g_serverAddr;
struct sockaddr_in g_clientAddr;
int g_clientLen = sizeof( g_clientAddr );
unsigned int g_nextPacketNumber = 0;
Vector2 g_flagPosition;
std::map< ClientInfo, Player* > g_players;
std::map< ClientInfo, std::vector< CS6Packet > > g_sentPacketsPerClient;


//-----------------------------------------------------------------------------------------------
void PrintError( const char* errorMessage, bool quitProgram )
{
	std::cout << errorMessage << "\n";
	g_isQuitting &= quitProgram;
	system( "PAUSE" );
}


//-----------------------------------------------------------------------------------------------
std::string ConvertNumberToString( int number )
{
	return static_cast< std::ostringstream* >( &( std::ostringstream() << number ) )->str();
}


//-----------------------------------------------------------------------------------------------
Color3b GetPlayerColorForID( unsigned int playerID )
{
	if( playerID == 0 )
		return Color3b( 255, 0, 0 );
	if( playerID == 1 )
		return Color3b( 0, 255, 0 );
	if( playerID == 2 )
		return Color3b( 0, 0, 255 );
	if( playerID == 3 )
		return Color3b( 255, 255, 0 );
	if( playerID == 4 )
		return Color3b( 255, 0, 255 );
	if( playerID == 5 )
		return Color3b( 0, 255, 255 );
	if( playerID == 6 )
		return Color3b( 255, 165, 0 );
	if( playerID == 7 )
		return Color3b( 128, 0, 128 );

	return Color3b( 255, 255, 255 );
}


//-----------------------------------------------------------------------------------------------
Vector2 GetRandomPosition()
{
	Vector2 returnVec;
	returnVec.x = (float) ( rand() % MAP_SIZE_WIDTH );
	returnVec.y = (float) ( rand() % MAP_SIZE_HEIGHT );

	return returnVec;
}


//-----------------------------------------------------------------------------------------------
void InitializeServer()
{
	if( WSAStartup( 0x202, &g_wsaData ) != 0 )
	{
		PrintError( "Winsock failed to initialize", true );
		return;
	}

	g_socket = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( g_socket == INVALID_SOCKET )
	{
		PrintError( "Socket creation failed", true );
		return;
	}

	g_serverAddr.sin_family = AF_INET;
	g_serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	g_serverAddr.sin_port = htons( PORT_NUMBER );

	g_clientAddr.sin_family = AF_INET;

	u_long mode = 1;
	if( ioctlsocket( g_socket, FIONBIO, &mode ) == SOCKET_ERROR )
	{
		PrintError( "Failed to set socket to non-blocking mode", true );
		return;
	}

	if( bind( g_socket, (struct sockaddr *) &g_serverAddr, sizeof( g_serverAddr ) ) < 0 )
	{
		PrintError( "Failed to bind socket", true );
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void SendPacketToSinglePlayer( const CS6Packet& pkt, const ClientInfo& info, bool requireAck )
{
	g_clientAddr.sin_addr.s_addr = inet_addr( info.m_ipAddress );
	g_clientAddr.sin_port = info.m_portNumber;
	sendto( g_socket, (const char*) &pkt, sizeof( pkt ), 0, (struct sockaddr*) &g_clientAddr, g_clientLen );
	++g_nextPacketNumber;

	if( requireAck )
	{
		std::vector< CS6Packet > sentPackets;
		std::map< ClientInfo, std::vector< CS6Packet > >::iterator vecIter = g_sentPacketsPerClient.find( info );
		if( vecIter != g_sentPacketsPerClient.end() )
			sentPackets = vecIter->second;

		sentPackets.push_back( pkt );
		g_sentPacketsPerClient[ info ] = sentPackets;
	}
}


//-----------------------------------------------------------------------------------------------
void SendPacketToAllPlayers( const CS6Packet& pkt, bool requireAck )
{
	std::map< ClientInfo, Player* >::iterator playerIter;
	for( playerIter = g_players.begin(); playerIter != g_players.end(); ++playerIter )
	{
		SendPacketToSinglePlayer( pkt, playerIter->first, requireAck );
	}
}


//-----------------------------------------------------------------------------------------------
void ResetPlayer( const ClientInfo& info )
{
	std::map< ClientInfo, Player* >::iterator playerIter = g_players.find( info );
	if( playerIter == g_players.end() )
		return;

	Player* player = playerIter->second;
	player->m_position = GetRandomPosition();
	player->m_velocity = Vector2( 0.f, 0.f );
	player->m_orientationDegrees = 0.f;
	player->m_lastUpdateTime = GetCurrentTimeSeconds();

	CS6Packet resetPacket;
	resetPacket.packetType = TYPE_Reset;
	resetPacket.packetNumber = g_nextPacketNumber;
	resetPacket.timestamp = GetCurrentTimeSeconds();
	resetPacket.data.reset.flagXPosition = g_flagPosition.x;
	resetPacket.data.reset.flagYPosition = g_flagPosition.y;
	resetPacket.data.reset.playerXPosition = player->m_position.x;
	resetPacket.data.reset.playerYPosition = player->m_position.y;
	resetPacket.data.reset.playerColorAndID[0] = player->m_color.r;
	resetPacket.data.reset.playerColorAndID[1] = player->m_color.g;
	resetPacket.data.reset.playerColorAndID[2] = player->m_color.b;

	SendPacketToSinglePlayer( resetPacket, info, true );
}


//-----------------------------------------------------------------------------------------------
void AddPlayer( const ClientInfo& info )
{
	std::map< ClientInfo, Player* >::iterator playerIter = g_players.find( info );
	if( playerIter == g_players.end() )
	{
		Player* player = new Player();
		player->m_color = GetPlayerColorForID( g_players.size() );
		g_players[ info ] = player;
		std::cout << "Added client " << info.m_ipAddress << ":" << ConvertNumberToString( info.m_portNumber ) << ". ";
		std::cout << "Set to color <" << ConvertNumberToString( player->m_color.r ) << ", " << ConvertNumberToString( player->m_color.g ) << ", " << ConvertNumberToString( player->m_color.b ) << ">\n";
	}

	ResetPlayer( info );
}


//-----------------------------------------------------------------------------------------------
void SendPlayerRemoval( Player* timeOutPlayer )
{
	/*Packet pkt;
	pkt.m_packetID = PACKET_STATE_REMOVE;
	pkt.m_playerID = timeOutPlayer->m_id;

	std::map< ClientInfo, Player* >::iterator addrIter;
	for( addrIter = g_players.begin(); addrIter != g_players.end(); ++addrIter )
	{
		g_clientAddr.sin_addr.s_addr = inet_addr( addrIter->second->m_ipAddress );
		g_clientAddr.sin_port = addrIter->second->m_portNumber;
		sendto( g_socket, (const char*) &pkt, sizeof( pkt ), 0, (struct sockaddr*) &g_clientAddr, g_clientLen );
	}

	std::cout << "Player " << timeOutPlayer->m_ipAddress << ":" << ConvertNumberToString( timeOutPlayer->m_portNumber ) << " has timed out and was removed\n";*/
}


//-----------------------------------------------------------------------------------------------
void UpdatePlayer( const CS6Packet& pkt, const ClientInfo& info )
{
	std::map< ClientInfo, Player* >::iterator playerIter = g_players.find( info );
	if( playerIter == g_players.end() )
		return;

	Player* player = playerIter->second;
	player->m_position.x = pkt.data.updated.xPosition;
	player->m_position.y = pkt.data.updated.yPosition;
	player->m_velocity.x = pkt.data.updated.xVelocity;
	player->m_velocity.y = pkt.data.updated.yVelocity;
	player->m_orientationDegrees = pkt.data.updated.yawDegrees;
	player->m_lastUpdateTime = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void SendVictory( const CS6Packet& clientVictoryPacket, const ClientInfo& info )
{
	CS6Packet ackPacket;
	ackPacket.packetNumber = g_nextPacketNumber;
	ackPacket.packetType = TYPE_Acknowledge;
	ackPacket.playerColorAndID[0] = clientVictoryPacket.playerColorAndID[0];
	ackPacket.playerColorAndID[1] = clientVictoryPacket.playerColorAndID[1];
	ackPacket.playerColorAndID[2] = clientVictoryPacket.playerColorAndID[2];
	ackPacket.timestamp = GetCurrentTimeSeconds();
	ackPacket.data.acknowledged.packetNumber = clientVictoryPacket.packetNumber;
	ackPacket.data.acknowledged.packetType = TYPE_Victory;

	SendPacketToSinglePlayer( ackPacket, info, false );

	std::cout << "Client " << info.m_ipAddress << ":" << ConvertNumberToString( info.m_portNumber ) << " has captured the flag. Reseting game.\n";

	g_flagPosition = GetRandomPosition();

	CS6Packet serverVictoryPacket;
	serverVictoryPacket.packetNumber = g_nextPacketNumber;
	serverVictoryPacket.packetType = TYPE_Victory;
	serverVictoryPacket.timestamp = GetCurrentTimeSeconds();
	serverVictoryPacket.data.victorious.playerColorAndID[0] = clientVictoryPacket.playerColorAndID[0];
	serverVictoryPacket.data.victorious.playerColorAndID[1] = clientVictoryPacket.playerColorAndID[1];
	serverVictoryPacket.data.victorious.playerColorAndID[2] = clientVictoryPacket.playerColorAndID[2];

	SendPacketToAllPlayers( serverVictoryPacket, true );
}


//-----------------------------------------------------------------------------------------------
void ProcessAckPacket( const CS6Packet& ackPacket, const ClientInfo& info )
{
	if( ackPacket.data.acknowledged.packetType == TYPE_Acknowledge )
	{
		AddPlayer( info );
	}
	else if( ackPacket.data.acknowledged.packetType == TYPE_Victory )
	{
		ResetPlayer( info );
	}

	std::map< ClientInfo, std::vector< CS6Packet > >::iterator vecIter = g_sentPacketsPerClient.find( info );
	if( vecIter == g_sentPacketsPerClient.end() )
		return;

	std::vector< CS6Packet > sentPackets = vecIter->second;
	for( unsigned int packetIndex = 0; packetIndex < sentPackets.size(); ++packetIndex )
	{
		CS6Packet packet = sentPackets[ packetIndex ];
		if( packet.packetNumber == ackPacket.data.acknowledged.packetNumber )
		{
			sentPackets.erase( sentPackets.begin() + packetIndex );
			break;
		}

		SendPacketToSinglePlayer( packet, info, false ); // we don't want to re-add packet to list
	}

	g_sentPacketsPerClient[ info ] = sentPackets;
}


//-----------------------------------------------------------------------------------------------
void GetPackets()
{
	CS6Packet pkt;
	while( recvfrom( g_socket, (char*) &pkt, sizeof( pkt ), 0, (struct sockaddr*) &g_clientAddr, &g_clientLen ) > 0 )
	{
		ClientInfo info;
		info.m_ipAddress = inet_ntoa( g_clientAddr.sin_addr );
		info.m_portNumber = g_clientAddr.sin_port;

		if( pkt.packetType == TYPE_Acknowledge )
		{
			ProcessAckPacket( pkt, info );
		}
		else if( pkt.packetType == TYPE_Update )
		{
			UpdatePlayer( pkt, info );
		}
		else if( pkt.packetType == TYPE_Victory )
		{
			SendVictory( pkt, info );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void RemoveTimedOutPlayers()
{
	std::map< ClientInfo, Player* > tempPlayerMap;
	std::map< ClientInfo, Player* >::iterator playerIter;
	for( playerIter = g_players.begin(); playerIter != g_players.end(); ++playerIter )
	{
		Player* player = playerIter->second;
		double timeSinceLastActivity = GetCurrentTimeSeconds() - player->m_lastUpdateTime;
		if( timeSinceLastActivity >= 5.f )
		{
			SendPlayerRemoval( player );
			delete playerIter->second;
			std::map< ClientInfo, std::vector< CS6Packet > >::iterator vecIter = g_sentPacketsPerClient.find( playerIter->first );
			if( vecIter != g_sentPacketsPerClient.end() )
			{
				g_sentPacketsPerClient.erase( vecIter );
				std::cout << "Client " << vecIter->first.m_ipAddress << ":" << ConvertNumberToString( vecIter->first.m_portNumber ) << " has timed out and is removed.\n";
			}

			continue;
		}

		tempPlayerMap[ playerIter->first ] = playerIter->second;
	}

	g_players = tempPlayerMap;
}


//-----------------------------------------------------------------------------------------------
void ResendAckPackets()
{
	std::map< ClientInfo, std::vector< CS6Packet > >::iterator vecIter;
	for( vecIter = g_sentPacketsPerClient.begin(); vecIter != g_sentPacketsPerClient.end(); ++vecIter )
	{
		std::vector< CS6Packet > sentPackets = vecIter->second;
		for( unsigned int packetIndex = 0; packetIndex < sentPackets.size(); ++packetIndex )
		{
			CS6Packet packet = sentPackets[ packetIndex ];
			if( ( GetCurrentTimeSeconds() - packet.timestamp ) > SECONDS_BEFORE_RESEND_RELIABLE_PACKETS )
			{
				SendPacketToSinglePlayer( packet, vecIter->first, false ); // we don't want to re-add packet to list
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void SendUpdatesToClients()
{
	if( ( GetCurrentTimeSeconds() - g_secondsSinceLastUpdate ) < SECONDS_BEFORE_SEND_UPDATE )
		return;

	std::map< ClientInfo, Player* >::iterator addrIter;
	std::map< ClientInfo, Player* >::iterator playerIter;
	for( playerIter = g_players.begin(); playerIter != g_players.end(); ++playerIter )
	{
		Player* player = playerIter->second;
		CS6Packet updatePacket;
		updatePacket.packetNumber = g_nextPacketNumber;
		updatePacket.packetType = TYPE_Update;
		updatePacket.playerColorAndID[0] = player->m_color.r;
		updatePacket.playerColorAndID[1] = player->m_color.g;
		updatePacket.playerColorAndID[2] = player->m_color.b;
		updatePacket.timestamp = GetCurrentTimeSeconds();
		updatePacket.data.updated.xPosition = player->m_position.x;
		updatePacket.data.updated.yPosition = player->m_position.y;
		updatePacket.data.updated.xVelocity = player->m_velocity.x;
		updatePacket.data.updated.yVelocity = player->m_velocity.y;
		updatePacket.data.updated.yawDegrees = player->m_orientationDegrees;

		SendPacketToAllPlayers( updatePacket, false );
	}

	g_secondsSinceLastUpdate = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void Initialize()
{
	srand( (unsigned int) time( NULL ) );

	InitializeTime();
	InitializeServer();
	g_flagPosition = GetRandomPosition();
	g_secondsSinceLastUpdate = GetCurrentTimeSeconds();
	g_secondsSinceLastReliableSend = GetCurrentTimeSeconds();

	std::cout << "Server is up and running\n";
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	GetPackets();
	RemoveTimedOutPlayers();
	SendUpdatesToClients();
	ResendAckPackets();
}


//-----------------------------------------------------------------------------------------------
int main( int, char* )
{
	Initialize();

	while( !g_isQuitting )
	{
		Update();
	}

	closesocket( g_socket );
	WSACleanup();
	return 0;
}