/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  app.c
 *  \brief  Application.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define APP_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup app
 *  \{
 */

#ifdef LINK_UT

/**
 *  \brief Get current time (for unit testing).
 */
static uint32_t ut_get_time( void )
{
	return ( clock_time_running() / 10 );
}

/**
 *  \brief Event handler of unit testing stage starting.
 *  \param t Stage of testing.
 */
static void ut_started( int t __attribute__(( unused )))
{
}

/**
 *  \brief Event handler of unit testing stage completion.
 *  \param t Stage of testing.
 *  \param result Testing result.
 */
static void ut_finished( int t, bool result )
{
	if ( !result ) ERROR( ut, t );

#ifdef LINK_CONSOLE
	console_prompt();

#endif
}

#endif

int app_init( int argc, char *argv[])
{
	if ( argc > 1 ) {
		ini_set_file( argv[ 1 ]);
	}
	clock_init();
	ini_load();
    printf(__CRLF__"CONF FILE : %s", ini_path); // see file conf.ini
	coding_init( ini_console_coding, ini_system_coding );

#ifdef LOGGING
	log_open( "a+" );

#endif

#ifdef LINK_UT
	ut_get_time_callback = ut_get_time;
	ut_started_callback = ut_started;
	ut_finished_callback = ut_finished;

#endif

	map_read();
/**
	st_list_write();
**/
	rzu_read();
	if ( mi_count ) mi_comp_read();
	pack_read();

	return ( 1 );
}

void app_uninit( void )
{
#ifdef LOGGING
	log_close();

#endif

	map_free();
}

/** \} */
