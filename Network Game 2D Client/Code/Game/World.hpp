#ifndef include_World
#define include_World
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Player.hpp"
#include "Color3b.hpp"
#include "CS6Packet.hpp"
#include "GameCommon.hpp"
#include "../Engine/Clock.hpp"
#include "../Engine/Mouse.hpp"
#include "../Engine/Camera.hpp"
#include "../Engine/Vertex.hpp"
#include "../Engine/pugixml.hpp"
#include "../Engine/Texture.hpp"
#include "../Engine/Vector2.hpp"
#include "../Engine/Keyboard.hpp"
#include "../Engine/Material.hpp"
#include "../Engine/BitmapFont.hpp"
#include "../Engine/DebugGraphics.hpp"
#include "../Engine/OpenGLRenderer.hpp"
#include "../Engine/NamedProperties.hpp"
#include "../Engine/ConsoleCommandArgs.hpp"
#include "../Engine/XMLParsingFunctions.hpp"
#include "../Engine/ErrorWarningAssertions.hpp"
#pragma comment(lib,"ws2_32.lib")


//-----------------------------------------------------------------------------------------------
const float SPEED_PIXELS_PER_SECOND = 100.f;
const float DISTANCE_FROM_FLAG_FOR_PICKUP_PIXELS = 10.f;
const float POINT_SIZE_PIXELS = 30.f;
const float ONE_HALF_POINT_SIZE_PIXELS = POINT_SIZE_PIXELS * 0.5f;
const double SECONDS_BEFORE_RESEND_INIT_PACKET = 0.1;
const double SECONDS_BEFORE_SEND_UPDATE_PACKET = 0.05;
const unsigned short PORT_NUMBER = 5000;
//const std::string IP_ADDRESS = "129.119.142.83";
const std::string IP_ADDRESS = "127.0.0.1";
const std::string FLAG_TEXTURE_FILE_PATH = "../Data/Images/Flag.png";
const std::string PLAYER_TEXTURE_FILE_PATH = "../Data/Images/Player.png";


//-----------------------------------------------------------------------------------------------
class World
{
public:
	World( float worldWidth, float worldHeight );
	void Initialize();
	void Destruct();
	void ChangeIPAddress( const std::string& ipAddrString );
	void ChangePortNumber( unsigned short portNumber );
	void Update( float deltaSeconds, const Keyboard& keyboard, const Mouse& mouse );
	void RenderObjects3D();
	void RenderObjects2D();

private:
	void InitializeConnection();
	void SendPacket( const CS6Packet& pkt, bool requireAck );
	void SendJoinGamePacket();
	void UpdatePlayer( const CS6Packet& updatePacket );
	void ProcessAckPacket( const CS6Packet& ackPacket );
	void ResendAckPackets();
	void ResetGame( const CS6Packet& resetPacket );
	void AcknowledgeVictory( const CS6Packet& victoryPacket );
	void SendVictory();
	void CheckForFlagCapture();
	void UpdateFromInput( const Keyboard& keyboard, const Mouse& mouse );
	void SendUpdates();
	void ReceivePackets();
	void InterpolatePositions( float deltaSeconds );
	void RenderFlag();
	void RenderPlayers();

	Camera						m_camera;
	Vector2						m_size;
	Texture*					m_playerTexture;
	Texture*					m_flagTexture;
	WSADATA						m_wsaData;
	SOCKET						m_socket;
	struct sockaddr_in			m_serverAddr;
	unsigned int				m_nextPacketNumber;
	bool						m_isConnectedToServer;
	bool						m_hasFlag;
	double						m_timeWhenLastInitPacketSent;
	double						m_timeWhenLastUpdatePacketSent;
	Vector2						m_flagPosition;
	Player*						m_mainPlayer;
	std::vector< Player* >		m_players;
	std::vector< CS6Packet >	m_sentPackets;
};


#endif // include_World