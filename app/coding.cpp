/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  coding.cpp
 *  \brief  Преобразование кодировок.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define CODING_IMPLEMENT
#include "include.h"
#include <QTextCodec>
#include <stdexcept>

static QTextCodec *codec_1251 = NULL;
static QTextCodec *codec_console = NULL;
static QTextCodec *codec_system = NULL;

void coding_init( const char *console, const char *system )
{
	codec_1251 = QTextCodec::codecForName( "cp1251" );
	codec_console = QTextCodec::codecForName( console );
	codec_system = QTextCodec::codecForName( system );
}

QString decode_cp1251( const char *src )
{
	if ( !codec_1251 ) return QString( src );

	return ( codec_1251->toUnicode( src ));
}

QString decode_system( const char *src )
{
	if ( !codec_system ) return QString( src );

	return ( codec_system->toUnicode( src ));
}

static const char *__encode( char *dst, QString *src, uint16_t len, QTextCodec *codec )
{
	if ( !codec ) {
		dst[ 0 ] = '\0';
		return ( dst );
	}

	if ( src->isEmpty()) {
		dst[ 0 ] = '\0';
		return ( dst );
	}

	QByteArray s = codec->fromUnicode( *src );
	if ( s.size()) {
		strncpy( dst, s.data(), len - 1 );
		dst[ len - 1 ] = '\0';
	}
	return ( dst );
}

const char *encode_cp1251( char *dst, QString *src, uint16_t len )
{
	return ( __encode( dst, src, len, codec_1251 ));
}

const char *encode_console( char *dst, QString *src, uint16_t len )
{
	return ( __encode( dst, src, len, codec_console ));
}

const char *encode_system( char *dst, QString *src, uint16_t len )
{
	return ( __encode( dst, src, len, codec_system ));
}

const char *encode_utf8( char *dst, QString *src, uint16_t len )
{
	if ( src->isEmpty()) {
		dst[ 0 ] = '\0';
		return ( dst );
	}

	QByteArray s = src->toUtf8();
	if ( s.size()) {
		strncpy( dst, s.data(), len - 1 );
		dst[ len - 1 ] = '\0';
	}
	return ( dst );
}
