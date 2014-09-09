#include "World.hpp"
#include "../Engine/Time.hpp"
#include "../Engine/DeveloperConsole.hpp"
#include "../Engine/NewMacroDef.hpp"

//-----------------------------------------------------------------------------------------------
World::World( float worldWidth, float worldHeight )
	: m_size( worldWidth, worldHeight )
	, m_playerTexture( nullptr )
	, m_flagTexture( nullptr )
	, m_isConnectedToServer( false )
	, m_hasFlag( false )
	, m_nextPacketNumber( 0 )
{
	m_mainPlayer = new Player();
	m_mainPlayer->m_color = Color3b( 255, 255, 255 );
	m_mainPlayer->m_currentPosition = Vector2( m_size.x * 0.5f, m_size.y * 0.5f );
	m_mainPlayer->m_previousPosition = Vector2( m_size.x * 0.5f, m_size.y * 0.5f );
	m_mainPlayer->m_currentPosition = Vector2( 0.f, 0.f );
	m_mainPlayer->m_previousVelocity = Vector2( 0.f, 0.f );
	m_mainPlayer->m_orientationDegrees = 0.f;
	m_flagPosition = Vector2( m_size.x * 0.5f, m_size.y * 0.5f );
	m_players.push_back( m_mainPlayer );
}


//-----------------------------------------------------------------------------------------------
void World::Initialize()
{
	InitializeTime();
	InitializeConnection();
	SendJoinGamePacket();
	m_playerTexture = Texture::CreateOrGetTexture( PLAYER_TEXTURE_FILE_PATH );
	m_flagTexture = Texture::CreateOrGetTexture( FLAG_TEXTURE_FILE_PATH );
	m_timeWhenLastInitPacketSent = GetCurrentTimeSeconds();
	m_timeWhenLastUpdatePacketSent = 0.0;
}


//-----------------------------------------------------------------------------------------------
void World::Destruct()
{
	closesocket( m_socket );
	WSACleanup();
}


//-----------------------------------------------------------------------------------------------
void World::ChangeIPAddress( const std::string& ipAddrString )
{
	m_serverAddr.sin_addr.s_addr = inet_addr( ipAddrString.c_str() );
	m_players.clear();
	m_players.push_back( m_mainPlayer );
	m_isConnectedToServer = false;
}


//-----------------------------------------------------------------------------------------------
void World::ChangePortNumber( unsigned short portNumber )
{
	m_serverAddr.sin_port = htons( portNumber );
	m_players.clear();
	m_players.push_back( m_mainPlayer );
	m_isConnectedToServer = false;
}


//-----------------------------------------------------------------------------------------------
void World::Update( float deltaSeconds, const Keyboard& keyboard, const Mouse& mouse )
{
	UpdateFromInput( keyboard, mouse );
	CheckForFlagCapture();
	SendUpdates();
	ReceivePackets();
	InterpolatePositions( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void World::RenderObjects3D()
{
	
}


//-----------------------------------------------------------------------------------------------
void World::RenderObjects2D()
{
	RenderFlag();
	RenderPlayers();
}


//-----------------------------------------------------------------------------------------------
void World::InitializeConnection()
{
	if( WSAStartup( 0x202, &m_wsaData ) != 0 )
	{
		g_isQuitting = true;
		return;
	}

	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( m_socket == INVALID_SOCKET )
	{
		g_isQuitting = true;
		return;
	}

	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = inet_addr( IP_ADDRESS.c_str() );
	m_serverAddr.sin_port = htons( PORT_NUMBER );

	u_long mode = 1;
	if( ioctlsocket( m_socket, FIONBIO, &mode ) == SOCKET_ERROR )
	{
		g_isQuitting = true;
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void World::SendPacket( const CS6Packet& packet, bool requireAck )
{
	sendto( m_socket, (char*) &packet, sizeof( packet ), 0, (struct sockaddr*) &m_serverAddr, sizeof( m_serverAddr ) );
	++m_nextPacketNumber;

	if( requireAck )
	{
		m_sentPackets.push_back( packet );
	}
}


//-----------------------------------------------------------------------------------------------
void World::SendJoinGamePacket()
{
	CS6Packet ackPacket;
	ackPacket.packetNumber = m_nextPacketNumber;
	ackPacket.packetType = TYPE_Acknowledge;
	ackPacket.timestamp = GetCurrentTimeSeconds();
	ackPacket.data.acknowledged.packetType = TYPE_Acknowledge;
	ackPacket.data.acknowledged.packetNumber = m_nextPacketNumber;

	SendPacket( ackPacket, false );
}


//-----------------------------------------------------------------------------------------------
void World::UpdatePlayer( const CS6Packet& updatePacket )
{
	if( updatePacket.playerColorAndID[0] == m_mainPlayer->m_color.r
		&& updatePacket.playerColorAndID[1] == m_mainPlayer->m_color.g
		&& updatePacket.playerColorAndID[2] == m_mainPlayer->m_color.b )
	{
		return;
	}

	for( unsigned int playerIndex = 0; playerIndex < m_players.size(); ++playerIndex )
	{
		Player* player = m_players[ playerIndex ];
		if( updatePacket.playerColorAndID[0] == player->m_color.r
			&& updatePacket.playerColorAndID[1] == player->m_color.g
			&& updatePacket.playerColorAndID[2] == player->m_color.b )
		{
			player->m_currentPosition.x = updatePacket.data.updated.xPosition;
			player->m_currentPosition.y = updatePacket.data.updated.yPosition;
			player->m_previousVelocity = player->m_currentVelocity;
			player->m_currentVelocity.x = updatePacket.data.updated.xVelocity;
			player->m_currentVelocity.y = updatePacket.data.updated.yVelocity;
			player->m_orientationDegrees = updatePacket.data.updated.yawDegrees;
			player->m_secondsSinceLastUpdate = 0.f;
			return;
		}
	}

	Player* player = new Player();
	player->m_color.r = updatePacket.playerColorAndID[0];
	player->m_color.g = updatePacket.playerColorAndID[1];
	player->m_color.b = updatePacket.playerColorAndID[2];
	player->m_previousPosition = player->m_currentPosition;
	player->m_currentPosition.x = updatePacket.data.updated.xPosition;
	player->m_currentPosition.y = updatePacket.data.updated.yPosition;
	player->m_previousPosition = player->m_currentPosition;
	player->m_currentVelocity.x = updatePacket.data.updated.xVelocity;
	player->m_currentVelocity.y = updatePacket.data.updated.yVelocity;
	player->m_previousVelocity = player->m_currentVelocity;
	player->m_previousVelocity = Vector2( 0.f, 0.f );
	player->m_orientationDegrees = updatePacket.data.updated.yawDegrees;
	player->m_secondsSinceLastUpdate = 0.f;

	m_players.push_back( player );
}


//-----------------------------------------------------------------------------------------------
void World::ProcessAckPacket( const CS6Packet& ackPacket )
{
	for( unsigned int packetIndex = 0; packetIndex < m_sentPackets.size(); ++packetIndex )
	{
		CS6Packet packet = m_sentPackets[ packetIndex ];
		if( packet.packetNumber == ackPacket.data.acknowledged.packetNumber )
		{
			m_sentPackets.erase( m_sentPackets.begin() + packetIndex );
			break;
		}

		SendPacket( packet, false ); // we don't want to re-add packet to list
	}
}


//-----------------------------------------------------------------------------------------------
void World::ResendAckPackets()
{
	for( unsigned int packetIndex = 0; packetIndex < m_sentPackets.size(); ++packetIndex )
	{
		CS6Packet packet = m_sentPackets[ packetIndex ];
		if( ( GetCurrentTimeSeconds() - packet.timestamp ) > SECONDS_BEFORE_RESEND_INIT_PACKET )
		{
			SendPacket( packet, false ); // we don't want to re-add packet to list
		}
	}
}


//-----------------------------------------------------------------------------------------------
void World::ResetGame( const CS6Packet& resetPacket )
{
	m_isConnectedToServer = true;
	m_hasFlag = false;

	m_flagPosition.x = resetPacket.data.reset.flagXPosition;
	m_flagPosition.y = resetPacket.data.reset.flagYPosition;
	m_mainPlayer->m_currentPosition.x = resetPacket.data.reset.playerXPosition;
	m_mainPlayer->m_currentPosition.y = resetPacket.data.reset.playerYPosition;
	m_mainPlayer->m_previousPosition = m_mainPlayer->m_currentPosition;
	m_mainPlayer->m_color.r = resetPacket.data.reset.playerColorAndID[0];
	m_mainPlayer->m_color.g = resetPacket.data.reset.playerColorAndID[1];
	m_mainPlayer->m_color.b = resetPacket.data.reset.playerColorAndID[2];

	CS6Packet ackPacket;
	ackPacket.packetNumber = m_nextPacketNumber;
	ackPacket.packetType = TYPE_Acknowledge;
	ackPacket.playerColorAndID[0] = resetPacket.data.reset.playerColorAndID[0];
	ackPacket.playerColorAndID[1] = resetPacket.data.reset.playerColorAndID[1];
	ackPacket.playerColorAndID[2] = resetPacket.data.reset.playerColorAndID[2];
	ackPacket.timestamp = GetCurrentTimeSeconds();
	ackPacket.data.acknowledged.packetNumber = resetPacket.packetNumber;
	ackPacket.data.acknowledged.packetType = TYPE_Reset;

	SendPacket( ackPacket, false );
}


//-----------------------------------------------------------------------------------------------
void World::AcknowledgeVictory( const CS6Packet& victoryPacket )
{
	CS6Packet ackPacket;
	ackPacket.packetNumber = m_nextPacketNumber;
	ackPacket.packetType = TYPE_Acknowledge;
	ackPacket.playerColorAndID[0] = victoryPacket.data.reset.playerColorAndID[0];
	ackPacket.playerColorAndID[1] = victoryPacket.data.reset.playerColorAndID[1];
	ackPacket.playerColorAndID[2] = victoryPacket.data.reset.playerColorAndID[2];
	ackPacket.timestamp = GetCurrentTimeSeconds();
	ackPacket.data.acknowledged.packetNumber = victoryPacket.packetNumber;
	ackPacket.data.acknowledged.packetType = TYPE_Victory;

	SendPacket( ackPacket, false );
}


//-----------------------------------------------------------------------------------------------
void World::SendVictory()
{
	if( m_hasFlag )
		return;

	CS6Packet victoryPacket;
	victoryPacket.packetNumber = m_nextPacketNumber;
	victoryPacket.packetType = TYPE_Victory;
	victoryPacket.playerColorAndID[0] = m_mainPlayer->m_color.r;
	victoryPacket.playerColorAndID[1] = m_mainPlayer->m_color.g;
	victoryPacket.playerColorAndID[2] = m_mainPlayer->m_color.b;
	victoryPacket.timestamp = GetCurrentTimeSeconds();
	victoryPacket.data.victorious.playerColorAndID[0] = m_mainPlayer->m_color.r;
	victoryPacket.data.victorious.playerColorAndID[1] = m_mainPlayer->m_color.g;
	victoryPacket.data.victorious.playerColorAndID[2] = m_mainPlayer->m_color.b;

	m_hasFlag = true;

	SendPacket( victoryPacket, true );
}


//-----------------------------------------------------------------------------------------------
void World::CheckForFlagCapture()
{
	if( !m_isConnectedToServer )
		return;

	Vector2 flagPositionDifference = m_flagPosition - m_mainPlayer->m_currentPosition;
	float distanceToFlag = flagPositionDifference.GetLength();
	if( distanceToFlag <= DISTANCE_FROM_FLAG_FOR_PICKUP_PIXELS )
		SendVictory();
}


//-----------------------------------------------------------------------------------------------
void World::UpdateFromInput( const Keyboard& keyboard, const Mouse& )
{
	if( g_developerConsole.m_drawConsole )
		return;

	bool isMovingEast = keyboard.IsKeyPressedDown( KEY_D );
	bool isMovingNorth = keyboard.IsKeyPressedDown( KEY_W );
	bool isMovingWest = keyboard.IsKeyPressedDown( KEY_A );
	bool isMovingSouth = keyboard.IsKeyPressedDown( KEY_S );

	bool isMovingNorthEast = isMovingNorth && isMovingEast;
	bool isMovingNorthWest = isMovingNorth && isMovingWest;
	bool isMovingSouthWest = isMovingSouth && isMovingWest;
	bool isMovingSouthEast = isMovingSouth && isMovingEast;

	if( isMovingNorthEast )
	{
		m_mainPlayer->m_currentVelocity = Vector2( 1.f, 1.f );
		m_mainPlayer->m_orientationDegrees = 45.f;
	}
	else if( isMovingNorthWest )
	{
		m_mainPlayer->m_currentVelocity = Vector2( -1.f, 1.f );
		m_mainPlayer->m_orientationDegrees = 135.f;
	}
	else if( isMovingSouthWest )
	{
		m_mainPlayer->m_currentVelocity = Vector2( -1.f, -1.f );
		m_mainPlayer->m_orientationDegrees = 225.f;
	}
	else if( isMovingSouthEast )
	{
		m_mainPlayer->m_currentVelocity = Vector2( 1.f, -1.f );
		m_mainPlayer->m_orientationDegrees = 315.f;
	}
	else if( isMovingEast )
	{
		m_mainPlayer->m_currentVelocity = Vector2( 1.f, 0.f );
		m_mainPlayer->m_orientationDegrees = 0.f;
	}
	else if( isMovingNorth )
	{
		m_mainPlayer->m_currentVelocity = Vector2( 0.f, 1.f );
		m_mainPlayer->m_orientationDegrees = 90.f;
	}
	else if( isMovingWest )
	{
		m_mainPlayer->m_currentVelocity = Vector2( -1.f, 0.f );
		m_mainPlayer->m_orientationDegrees = 180.f;
	}
	else if( isMovingSouth )
	{
		m_mainPlayer->m_currentVelocity = Vector2( 0.f, -1.f );
		m_mainPlayer->m_orientationDegrees = 270.f;
	}
	else
	{
		m_mainPlayer->m_currentVelocity = Vector2( 0.f, 0.f );
	}

	m_mainPlayer->m_currentVelocity.Normalize();
	m_mainPlayer->m_currentVelocity *= SPEED_PIXELS_PER_SECOND;
}


//-----------------------------------------------------------------------------------------------
void World::SendUpdates()
{
	if( !m_isConnectedToServer && ( GetCurrentTimeSeconds() - m_timeWhenLastInitPacketSent ) > SECONDS_BEFORE_RESEND_INIT_PACKET )
	{
		SendJoinGamePacket();
		m_timeWhenLastInitPacketSent = GetCurrentTimeSeconds();
		return;
	}

	if( !m_isConnectedToServer )
		return;

	if( ( GetCurrentTimeSeconds() - m_timeWhenLastUpdatePacketSent ) > SECONDS_BEFORE_SEND_UPDATE_PACKET )
	{
		CS6Packet packet;
		packet.packetNumber = m_nextPacketNumber;
		packet.packetType = TYPE_Update;
		packet.playerColorAndID[0] = m_mainPlayer->m_color.r;
		packet.playerColorAndID[1] = m_mainPlayer->m_color.g;
		packet.playerColorAndID[2] = m_mainPlayer->m_color.b;
		packet.timestamp = GetCurrentTimeSeconds();
		packet.data.updated.xPosition = m_mainPlayer->m_currentPosition.x;
		packet.data.updated.yPosition = m_mainPlayer->m_currentPosition.y;
		packet.data.updated.xVelocity = m_mainPlayer->m_currentVelocity.x;
		packet.data.updated.yVelocity = m_mainPlayer->m_currentVelocity.y;
		packet.data.updated.yawDegrees = m_mainPlayer->m_orientationDegrees;

		SendPacket( packet, false );

		m_timeWhenLastUpdatePacketSent = GetCurrentTimeSeconds();
	}
}


//-----------------------------------------------------------------------------------------------
void World::ReceivePackets()
{
	CS6Packet packet;
	struct sockaddr_in clientAddr;
	int clientLen = sizeof( clientAddr );

	while( recvfrom( m_socket, (char*) &packet, sizeof( packet ), 0, (struct sockaddr*) &clientAddr, &clientLen ) > 0 )
	{
		if( packet.packetType == TYPE_Update )
		{
			UpdatePlayer( packet );
		}
		else if( packet.packetType == TYPE_Acknowledge )
		{
			ProcessAckPacket( packet );
		}
		else if( packet.packetType == TYPE_Reset )
		{
			ResetGame( packet );
		}
		else if( packet.packetType == TYPE_Victory )
		{
			AcknowledgeVictory( packet );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void World::InterpolatePositions( float deltaSeconds )
{
	for( unsigned int playerIndex = 0; playerIndex < m_players.size(); ++playerIndex )
	{
		Player* player = m_players[ playerIndex ];
		if( player == nullptr || player == m_mainPlayer )
		{
			player->m_currentPosition += player->m_currentVelocity * deltaSeconds;
		}
		else
		{
			Vector2 blendVector = player->m_currentVelocity + ( player->m_previousVelocity - player->m_currentVelocity ) * player->m_secondsSinceLastUpdate;
			Vector2 currentProj = player->m_currentPosition + player->m_currentVelocity * deltaSeconds;
			Vector2 previousProj = player->m_previousPosition + player->m_previousVelocity * deltaSeconds;
			Vector2 gotoPosition = currentProj + ( previousProj - currentProj ) * player->m_secondsSinceLastUpdate;
			player->m_secondsSinceLastUpdate += deltaSeconds;

			player->m_currentPosition += deltaSeconds * ( gotoPosition - player->m_currentPosition );
		}

		player->m_currentPosition.x = ClampFloat( player->m_currentPosition.x, 0.f, m_size.x );
		player->m_currentPosition.y = ClampFloat( player->m_currentPosition.y, 0.f, m_size.y );
	}
}


//-----------------------------------------------------------------------------------------------
void World::RenderFlag()
{
	OpenGLRenderer::EnableTexture2D();
	OpenGLRenderer::BindTexture2D( m_flagTexture->m_openglTextureID );
	OpenGLRenderer::SetColor3f( 1.f, 1.f, 1.f );

	OpenGLRenderer::BeginRender( QUADS );
	{
		OpenGLRenderer::SetTexCoords2f( 0.f, 1.f );
		OpenGLRenderer::SetVertex2f( m_flagPosition.x - ONE_HALF_POINT_SIZE_PIXELS, m_flagPosition.y - ONE_HALF_POINT_SIZE_PIXELS );

		OpenGLRenderer::SetTexCoords2f( 1.f, 1.f );
		OpenGLRenderer::SetVertex2f( m_flagPosition.x + ONE_HALF_POINT_SIZE_PIXELS, m_flagPosition.y - ONE_HALF_POINT_SIZE_PIXELS );

		OpenGLRenderer::SetTexCoords2f( 1.f, 0.f );
		OpenGLRenderer::SetVertex2f( m_flagPosition.x + ONE_HALF_POINT_SIZE_PIXELS, m_flagPosition.y + ONE_HALF_POINT_SIZE_PIXELS );

		OpenGLRenderer::SetTexCoords2f( 0.f, 0.f );
		OpenGLRenderer::SetVertex2f( m_flagPosition.x - ONE_HALF_POINT_SIZE_PIXELS, m_flagPosition.y + ONE_HALF_POINT_SIZE_PIXELS );
	}
	OpenGLRenderer::EndRender();

	OpenGLRenderer::BindTexture2D( 0 );
	OpenGLRenderer::DisableTexture2D();
}


//-----------------------------------------------------------------------------------------------
void World::RenderPlayers()
{
	OpenGLRenderer::EnableTexture2D();
	OpenGLRenderer::BindTexture2D( m_playerTexture->m_openglTextureID );

	for( unsigned int playerIndex = 0; playerIndex < m_players.size(); ++playerIndex )
	{
		Player* player = m_players[ playerIndex ];
		if( player == nullptr )
			continue;

		Vector2 playerPos = player->m_currentPosition;

		OpenGLRenderer::PushMatrix();

		OpenGLRenderer::SetColor3f( player->m_color.r * ONE_OVER_TWO_HUNDRED_TWENTY_FIVE, player->m_color.g * ONE_OVER_TWO_HUNDRED_TWENTY_FIVE, player->m_color.b * ONE_OVER_TWO_HUNDRED_TWENTY_FIVE );
		OpenGLRenderer::Translatef( playerPos.x, playerPos.y, 0.f );
		OpenGLRenderer::Rotatef( -player->m_orientationDegrees, 0.f, 0.f, 1.f );

		OpenGLRenderer::BeginRender( QUADS );
		{
			OpenGLRenderer::SetTexCoords2f( 0.f, 1.f );
			OpenGLRenderer::SetVertex2f( -ONE_HALF_POINT_SIZE_PIXELS, -ONE_HALF_POINT_SIZE_PIXELS );

			OpenGLRenderer::SetTexCoords2f( 1.f, 1.f );
			OpenGLRenderer::SetVertex2f( ONE_HALF_POINT_SIZE_PIXELS, -ONE_HALF_POINT_SIZE_PIXELS );

			OpenGLRenderer::SetTexCoords2f( 1.f, 0.f );
			OpenGLRenderer::SetVertex2f( ONE_HALF_POINT_SIZE_PIXELS, ONE_HALF_POINT_SIZE_PIXELS );

			OpenGLRenderer::SetTexCoords2f( 0.f, 0.f );
			OpenGLRenderer::SetVertex2f( -ONE_HALF_POINT_SIZE_PIXELS, ONE_HALF_POINT_SIZE_PIXELS );
		}
		OpenGLRenderer::EndRender();

		OpenGLRenderer::PopMatrix();
	}

	OpenGLRenderer::BindTexture2D( 0 );
	OpenGLRenderer::DisableTexture2D();
}