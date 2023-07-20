/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  journal.cpp
 *  \brief  Журнал результатов тестирования.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define JOURNAL_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup journal
 *  \{
 */

// UNIX //////////////////////////////////////////
#if defined( __linux__ )
#include <sys/types.h>
#include <sys/stat.h>

//////////////////////////////////////////////////
#endif

#define MESSAGE_LIMIT			128

#define STR_OPEN_JOURNAL        "Журнал открыт. Текущая дата: %s"
#define STR_CLOSE_JOURNAL       "Журнал закрыт."
#define STR_CLEAR_JOURNAL       "Журнал очищен."

static FILE *journal_file = NULL;
static FILE *journal_filemcp = NULL;


static char journal_name[ 256 ];
static char journal_time[ 64 ];
static char journal_date[ 64 ];

static char journal_namemcp[ 256 ];


const char *journal_freeze_time()
{
	clock_stamp( NULL, journal_time );

	return ( journal_time );
}

const char *journal_freeze_date()
{
	struct timeval t;
	time_t sec;

	gettimeofday( &t, NULL );
	sec = t.tv_sec;
	strftime( journal_date, sizeof( journal_date ), "%d.%m.%Y", localtime( &sec ));

	return ( journal_date );
}

const char *journal_get_time()
{
	return ( journal_time );
}

void journal_open( const char *name, const char *mode )
{
	strncpy( journal_name, name, sizeof( journal_name ) - 1 );
	journal_name[ sizeof( journal_name ) - 1 ] = '\0';

	journal_freeze_date();
	journal_freeze_time();

	journal_file = fopen( journal_name, mode );
	if ( journal_file == NULL ) {
		ERROR( file_open, journal_name, strerror( errno ));
		journal_file = stderr;
	}
	fprintf( journal_file, "\n%s\t" STR_OPEN_JOURNAL "\n", journal_time, journal_date );
	fflush( journal_file );
}




void journal_close( void )
{
	if ( journal_file != NULL ) {
		journal_freeze_time();
		fprintf( journal_file, "%s\t" STR_CLOSE_JOURNAL "\n", journal_time );
		fclose( journal_file );
// UNIX //////////////////////////////////////////
#if defined( __linux__ )
		chmod( journal_name
		, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
		);

//////////////////////////////////////////////////
#endif
		journal_file = NULL;
	}
}

void journal_clear( const char *name )
{
	pthread_mutex_lock( &journal_mutex );

	journal_close();
	journal_open( name, "wa" );

	pthread_mutex_unlock( &journal_mutex );

	journal_print( "", STR_CLEAR_JOURNAL );
}

void journal_print( const char *st, const char *s, bool istime )
{
	pthread_mutex_lock( &journal_mutex );

	fprintf( journal_file, ( istime ) ? journal_time : "\t" );
	fprintf( journal_file, "\t%s\t%s\n", st, s );
	fflush( journal_file );

	pthread_mutex_unlock( &journal_mutex );
}

void journal_printf( const char *st, const char *format, ... )
{
	va_list va;
	char s[ MESSAGE_LIMIT ];

	va_start( va, format );
	vsprintf( s, format, va );
	va_end( va );

	journal_print( st, s );
}


void journal_openmcp( const char *name, const char *mode )
{
    strncpy( journal_namemcp, name, sizeof( journal_namemcp ) - 1 );
    journal_namemcp[ sizeof( journal_namemcp ) - 1 ] = '\0';


    journal_filemcp = fopen( journal_namemcp, mode );
    if ( journal_filemcp == NULL ) {
        printf("ERROR MCP FILE JOURNAL READ");
        journal_filemcp = stderr;
    }
    fprintf( journal_filemcp, "start mcp");
    fflush( journal_filemcp );
}

void journal_closemcp( void )
{
    if ( journal_filemcp != NULL ) {
        fclose( journal_filemcp );
// UNIX //////////////////////////////////////////
#if defined( __linux__ )
        chmod( journal_namemcp
        , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
        );

//////////////////////////////////////////////////
#endif
        journal_filemcp = NULL;
    }
}

void journal_printmcp( const char *st, const char *s, bool istime )
{
    pthread_mutex_lock( &journal_mutex );

    fprintf( journal_filemcp, "%x ", s );
    fflush( journal_filemcp );

    pthread_mutex_unlock( &journal_mutex );
}

void journal_printmcp_enter(  )
{
    pthread_mutex_lock( &journal_mutex );

    fprintf( journal_filemcp, "\n\n\n\n\n");
    fflush( journal_filemcp );

    pthread_mutex_unlock( &journal_mutex );
}
void journal_printmcp_qstring( const char *st, QString str, bool istime ){

    pthread_mutex_lock( &journal_mutex );

    const char *s=str.toUtf8();
    fprintf( journal_filemcp, "%s \n", s );
    fflush( journal_filemcp );


    pthread_mutex_unlock( &journal_mutex );
}



/** \} */
