/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ini.cpp
 *  \brief  Configuration options.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define INI_IMPLEMENT
#include "include.h"
#include <QSettings>

static char __at_home_path[ 256 ];
static int __at_home_len;

bool ini_set_file( char *fn )
{
// CORTEX_M //////////////////////////////////////
#if defined( __ARM_ARCH )

	return ( false );

// UNIX //////////////////////////////////////////
// WIN32 /////////////////////////////////////////
// MSDOS /////////////////////////////////////////
#else
	FILE *f;

	f = fopen( fn, "r" );
	if ( f == NULL ) {
		ERROR( file_open, fn, strerror( errno ));
		return ( false );
	}
	fclose( f );
	ini_path = fn;
	printf( __CRLF__"Configure file: %s", ini_path );
	return ( true );

//////////////////////////////////////////////////
#endif
}

void ini_load( void )
{
	QSettings ini( ini_path, QSettings::IniFormat );
	ini.setIniCodec( "UTF-8" );

/** \cond */

#define INI_INT( name, defval ) \
	ini_##name = ini.value( #name, ini_##name ).toInt();
#define INI_BOOL( name, defval ) \
	ini_##name = ini.value( #name, ini_##name ).toBool();
#define INI_STR( name, len, defval ) do { \
	QByteArray val = ini.value( #name, defval ).toString().toLocal8Bit(); \
	if ( val.size()) { \
		ini_##name = ( char *) malloc( val.size() + 1 ); \
		if ( ini_##name ) memcpy( ini_##name, val.data(), val.size()); \
		ini_##name[ val.size()] = 0; \
	} \
} while ( 0 );
#define INI_QSTR( name, defval ) \
	ini_##name = ini.value( #name, defval ).toString();

#include "ini-dict.h"

#undef INI_STR
#undef INI_BOOL
#undef INI_INT

/** \endcond */

	getcwd( __at_home_path, sizeof( __at_home_path ) - 1 );
	__at_home_len = strlen( __at_home_path );
	if ( __at_home_len && ( __at_home_path[ __at_home_len - 1 ] != '/' )) {
		__at_home_path[ __at_home_len ] = '/';
		__at_home_path[ __at_home_len + 1 ] = '\0';
		++__at_home_len;
	}
}

void ini_save( void )
{
}

char *ini_at_home( const char *fn )
{
	__at_home_path[ __at_home_len ] = '\0';

	return ((( fn[ 0 ] == '/' ) || ( fn[ 0 ] == '~' )) ? ( char *) fn : strcat( __at_home_path, fn ));
}
