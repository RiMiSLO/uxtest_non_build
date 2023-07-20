/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  log.c
 *  \brief  Logging.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define LOG_IMPLEMENT
#include "include.h"

#ifdef LOGGING

/**
 *  \addtogroup log
 *  \{
 */

// UNIX //////////////////////////////////////////
#if defined( __linux__ )
#include <sys/types.h>
#include <sys/stat.h>

//////////////////////////////////////////////////
#endif

/** \cond */

#define LOG_WARN( name, format, repeat ) \
	format,
#define LOG_INFO( name, format, repeat )

const char *const WARN_format[] = {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO


#define LOG_WARN( name, format, repeat )
#define LOG_INFO( name, format, repeat ) \
	format,

const char *const INFO_format[] = {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO


#define DATA_ERROR( name, bit, format, repeat ) \
	repeat,

uint8_t ERROR_repeat[] = {
#include "error-dict.h"
};

#undef DATA_ERROR


#define LOG_WARN( name, format, repeat ) \
	repeat,
#define LOG_INFO( name, format, repeat )

uint8_t WARN_repeat[] = {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO


#define LOG_WARN( name, format, repeat )
#define LOG_INFO( name, format, repeat ) \
	repeat,

uint8_t INFO_repeat[] = {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO

/** \endcond */

void log_open( const char *mode )
{
// CORTEX_M //////////////////////////////////////
#if defined( __ARM_ARCH )

// UNIX //////////////////////////////////////////
// WIN32 /////////////////////////////////////////
// MSDOS /////////////////////////////////////////
#else
	char s[ 32 ];
	char sd[ 32 ];
	struct timeval t;
	time_t sec;

	gettimeofday( &t, NULL );
	sec = t.tv_sec;
	strftime( sd, sizeof( sd ), "%d.%m.%Y", localtime( &sec ));

	clock_stamp( &t, s );

	log_file = fopen( ini_at_home( ini_log_path ), mode );
	if ( log_file == NULL ) {
		fprintf( stderr, "\nopen(\"%s\") failed: %s\n", ini_log_path, strerror( errno ));
		log_file = stderr;
	}
	fprintf( log_file, "\n\n%s\tlogging started %s", s, sd );
	fflush( log_file );

//////////////////////////////////////////////////
#endif
}

void log_close( void )
{
// CORTEX_M //////////////////////////////////////
#if defined( __ARM_ARCH )

// UNIX //////////////////////////////////////////
// WIN32 /////////////////////////////////////////
// MSDOS /////////////////////////////////////////
#else
	char s[ 32 ];

	if ( log_file != NULL ) {
		fprintf( log_file, "\n%s\tlogging stopped", clock_stamp( NULL, s ));
		fclose( log_file );

// UNIX //////////////////////////////////////////
#if defined( __linux__ )
		chmod( ini_at_home( ini_log_path )
		, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
		);

//////////////////////////////////////////////////
#endif
		log_file = NULL;
	}

//////////////////////////////////////////////////
#endif
}

void log_clear( void )
{
// CORTEX_M //////////////////////////////////////
#if defined( __ARM_ARCH )

// UNIX //////////////////////////////////////////
// WIN32 /////////////////////////////////////////
// MSDOS /////////////////////////////////////////
#else
	if ( log_lock_callback ) log_lock_callback();

	log_close();
	log_open( "wa" );
	log_printf( "INFO", "log cleaned" );

	if ( log_unlock_callback ) log_unlock_callback();

//////////////////////////////////////////////////
#endif
}

/** \} */

#endif /* LOGGING */
