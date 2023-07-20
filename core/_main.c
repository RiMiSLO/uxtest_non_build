/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  main.c
 *  \brief  predict - an embedded application framework.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define CO_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup core
 *  \{
 */

/**
 *  \brief Initialize all coroutines.
 */
static void coro_init( void )
{
/** \cond */

#define CORO( name ) \
	CORO_CONTEXT_INIT( name ); \
	name##_alive = name##_init();

#include "co-dict.h"

#undef CORO

/** \endcond */
}

/**
 *  \brief Uninitialize all coroutines.
 */
static void coro_uninit( void )
{
/** \cond */

#define CORO( name ) \
	name##_uninit();

#include "co-dict.h"

#undef CORO

/** \endcond */
}

/**
 *  \brief Call coroutines according to dictionary.
 */
static int coro_schedule( void )
{
/** \cond */

#define CORO( name ) \
	if ( CORO_ALIVE( name##_alive )) name##_alive = CORO_CALL( name );

#include "co-dict.h"

#undef CORO

#define CORO( name ) \
	|| CORO_ALIVE( name##_alive )

	return ( PREDICT_ALIVE_CORO_COMPLETED
#include "co-dict.h"
	);

#undef CORO

/** \endcond */
}

#ifdef LINK_UT

/**
 *  \brief 1st stage of unit testing (before coroutines call loop).
 */
static void ut_1( void )
{
/** \cond */

#define UT_1( test ) \
	ut_##test();
#define UT_2( test )

#include "ut-dict.h"

#undef UT_2
#undef UT_1

/** \endcond */
}

#endif /* LINK_UT */

/**
 *  \brief Application life-cycle.
 */
int _main( int argc, char *argv[])
{
	( void ) argc;
	( void ) argv;

	app_alive = 1; /* app_init( argc, argv ) */
	coro_init();

#ifdef LINK_UT
	ut_stage_1( ut_1 );

#endif
	/* Coroutines call loop */
	while ( app_alive ) {
		if ( coro_schedule() == 0 ) break;
		usleep( 2 ); // !? +12s to ut...
	}
	coro_uninit();
	/* app_uninit(); */

	return ( app_return );
}

/** \} */
