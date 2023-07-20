/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data-str.c
 *  \brief  String converting.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define DATA_STR_IMPLEMENT
#include "include.h"

char *string_byte_h( char *s, byte val )
{
	sprintf( s, "0x%02X", val );
	return ( s );
}

char *string_word_h( char *s, word val )
{
	sprintf( s, "0x%04" PRIXWORD, val );
	return ( s );
}

char *string_dword_h( char *s, dword val )
{
	sprintf( s, "0x%08" PRIXDWORD, val );
	return ( s );
}

char *string_percent( char *s, int val )
{
	if ( val == 0 ) return ( "" );

	if ( val > 100 ) val = 100;
	sprintf( s, ( val < 0 ) ? "%d сек" : "%u%%", val );
	return ( s );
}
