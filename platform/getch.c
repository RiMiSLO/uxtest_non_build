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

#include <termios.h>

/**
 *  \addtogroup char
 *  \{
 */

static struct termios getch_o_attr;

void getch_init( void )
{
	struct termios n_attr;

	/* Set terminal to raw mode */
	tcgetattr( fileno( stdin ), &getch_o_attr );
	memcpy( &n_attr, &getch_o_attr, sizeof( struct termios ));
	n_attr.c_lflag &= ~( ECHO | ICANON );
	n_attr.c_cc[ VTIME ] = 0;
	n_attr.c_cc[ VMIN  ] = 0;
	tcsetattr( fileno( stdin ), TCSANOW, &n_attr );
}

void getch_uninit( void )
{
	/* Restore original terminal attributes */
	tcsetattr( fileno( stdin ), TCSANOW, &getch_o_attr );
}

int getch( void )
{
	/* Read character from STDIN stream without blocking. Returns EOF (-1) if no character is available. */
	return fgetc( stdin );
}

__inline int getch_1( void )
{
	return getch();
}

int getch_escseq( int chr )
{
	static int seq = 0;

	switch ( chr ) {
	case CHAR_ESC:
		seq = 1;
		return ( ESCSEQ_INSIDE );

	case '[':
		if ( seq == 1 ) {
			seq = 2;
			return ( ESCSEQ_INSIDE );
		}
	}
	if ( seq == 2 ) {
		seq = 0;
		return ( ESCSEQ_OK );
	}
	seq = 0;
	return ( ESCSEQ_1 );
}

/** \} */

#endif /* LINK_CONSOLE */
