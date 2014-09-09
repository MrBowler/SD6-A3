#ifndef include_EngineCommon
#define include_EngineCommon
#pragma once

//-----------------------------------------------------------------------------------------------
#define STATIC


//-----------------------------------------------------------------------------------------------
typedef unsigned char byte_t;


//-----------------------------------------------------------------------------------------------
const int NUM_VIRTUAL_KEYS = 256;
const int NUM_KEYBOARD_CHARS = 256;


//-----------------------------------------------------------------------------------------------
class BitmapFont;
extern BitmapFont g_font;
extern bool g_isQuitting;


#endif // include_EngineCommon