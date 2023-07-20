/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut_2.c
 *  \brief  Unit testing of 2nd stage.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#include "include.h"

#ifdef LINK_UT

#define UT_1( name )

/**
 *  \addtogroup ut
 *  \{
 */
/** \cond */

#define UT_2( name ) \
	int ut_##name##_alive; \
	static CORO_CONTEXT( ut_##name ); \
	extern CORO_DEFINE( ut_##name ); \
	extern int ut_##name##_init( void ); \
	extern void ut_##name##_uninit( void );

#include "ut-dict.h"

#undef UT_2

/** \endcond */

int ut_2_init( void )
{
/** \cond */

#define UT_2( name ) \
	CORO_CONTEXT_INIT( ut_##name ); \
	ut_##name##_alive = ut_##name##_init();

#include "ut-dict.h"

#undef UT_2

	return ( CO_READY ); /* CO_SKIP */
}

void ut_2_uninit( void )
{
/** \cond */

#define UT_2( name ) \
	ut_##name##_uninit();

#include "ut-dict.h"

#undef UT_2

/** \endcond */
}

/**
 *  \brief Schedule of test coroutines.
 *  \return true if testing is not complete.
 */
static int ut_2_schedule( void )
{
/** \cond */

#define UT_2( name ) \
	if ( CORO_ALIVE( ut_##name##_alive )) ut_##name##_alive = CORO_CALL( ut_##name );

#include "ut-dict.h"

#undef UT_2

#define UT_2( name ) \
	|| CORO_ALIVE( ut_##name##_alive )

	return ( 0
#include "ut-dict.h"
	);

#undef UT_2

/** \endcond */
}

/** \cond */

#define UT_2( name )  ut_2_##name,

enum {
#include "ut-dict.h"
	ut_2_COUNT
};

#undef UT_2

/** \endcond */

/**
 *  \brief Coroutine of unit testing of 2nd stage.
 */
CORO_DEFINE( ut_2 )
{
#if ( PREDICT_UT_2_PAUSE > 0 )
	CORO_LOCAL struct timeval __t;

#endif
	CORO_BEGIN();

	ut_stage_2_start( ut_2_COUNT );  /* Start unit testing of 2nd stage */
	CORO_YIELD();

#if ( PREDICT_UT_2_PAUSE > 0 )
	__t = clock_t_set( PREDICT_UT_2_PAUSE * TICK_PER_SEC );
	CORO_WAIT( clock_time_after( __t ));  /* Pause before performing tests */

#endif
	/* Test coroutines call loop */
	CORO_WAIT( !ut_stage_2( ut_2_schedule ));

	CORO_END();
}

void ut_2_restart( void )
{
	if ( ut_2_alive == CO_END ) {
		CORO_CONTEXT_INIT( ut_2 );
		ut_2_alive = ut_2_init();
	}
}

/** \} */

#undef UT_1

#endif /* LINK_UT */
