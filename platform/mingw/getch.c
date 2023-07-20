/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  getch.c
 *  \brief  Read from debug interface.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define GETCH_IMPLEMENT

#include "include.h"

#ifdef LINK_CONSOLE

/**
 *  \addtogroup char
 *  \{
 */

static struct timeval getch_t = { 0, 0 };

void getch_init( void )
{
}

void getch_uninit( void )
{
}

__inline int getch_1( void )
{
	if ( clock_time_before( getch_t )) return EOF;
	getch_t = clock_t_set( 100 );

	/* Unfortunately, this version of kbhit() has long delay. */
	return ( kbhit() ? _getch() : EOF );
}

int getch_escseq( int chr )
{
	static int seq = 0;

	if (( chr == 0 ) || ( chr == 0xE0 )) {
		seq = 1;
		return ( ESCSEQ_INSIDE );
	}
	if ( seq == 1 ) {
		seq = 0;
		return ( ESCSEQ_OK );
	}
	return ( ESCSEQ_1 );
}

/** \} */

#endif /* LINK_CONSOLE */
