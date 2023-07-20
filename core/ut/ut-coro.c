/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut-coro.c
 *  \brief  Unit test of coroutines.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#include "include.h"

#ifdef LINK_UT_CORO

/**
 *  \defgroup ut_coro Unit test of coroutines
 *  \ingroup ut
 *  \{
 */

#define CYCLE_COUNT  1024

#define B_SIZE  8
static int buf[ B_SIZE ];
static int pos = 0;

static semaphore_t b_full, b_empty;

static int answer = 0;
static int answer_last = 0;

static int inside_test_3;

static void b_set( int mark )
{
	buf[ pos ] = mark;
	pos = ( pos + 1 ) % B_SIZE;
}

static int b_get( void )
{
	int mark = buf[ pos ];
	pos = ( pos + 1 ) % B_SIZE;

	return mark;
}

static int create_mark( void )
{
	static int mark = 0;

	return mark++;
}

static void check_mark( int mark )
{
	assert_int_eq( answer++, mark );
}

int test_1_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( test_1 )
{
	CORO_LOCAL int i;

	CORO_BEGIN();

	for ( i = 0; i < CYCLE_COUNT; i++ ) {
		SEMAPHORE_ACQUIRE( b_full );

		b_set( create_mark());

		SEMAPHORE_RELEASE( b_empty );
	}
	CORO_END();
}

int test_2_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( test_2 )
{
	CORO_LOCAL int i;

	CORO_BEGIN();

	for ( i = 0; i < CYCLE_COUNT; i++ ) {
		SEMAPHORE_ACQUIRE( b_empty );

		check_mark( b_get());

		SEMAPHORE_RELEASE( b_full );
	}
	CORO_END();
}

int test_3_init( void )
{
	inside_test_3 = 0;

	return ( CO_READY ); /* CO_SKIP */
}

#define INITIAL \
	do { \
		inside_test_3++; \
	} while ( 0 )

#define FINAL \
	do { \
		inside_test_3--; \
	} while ( 0 )

#define FINAL_EXIT \
	do { \
		inside_test_3 = -1; \
	} while ( 0 )

CORO_DEFINE( test_3 )
{
	CORO_LOCAL int i = 1;

	CORO_BEGIN( INITIAL );

	switch ( i ) {

	case 1:
		i = 2;
		CORO_YIELD( FINAL );
		assert_int_eq( 2, i );

		i = 3;
		CORO_YIELD( FINAL );
		assert_int_eq( 3, i );

		i = 4;
		CORO_YIELD( FINAL );
		assert_int_eq( 4, i );

		CORO_WAIT(( ++i < 10 ), FINAL );
		for ( ; i < 20; i++ ) {
			CORO_YIELD( FINAL );
		}
		assert_int_eq( 20, i );

		i = -1;
		CORO_RESTART( FINAL );

	default:
		assert_fail( "unreachable instruction" );
		break;

	case -1:
		i = 1;
		CORO_QUIT( FINAL_EXIT );
	}
	assert_fail( "unreachable instruction" );
	CORO_END( FINAL_EXIT );
}

#undef INITIAL
#undef FINAL
#undef FINAL_EXIT

int test_4_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( test_4 )
{
	CORO_BEGIN();

	do {
		assert_not( inside_test_3 );
		CORO_YIELD();
	} while ( inside_test_3 != -1 );

	CORO_END();
}

int ut_coro_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

void ut_coro_uninit( void )
{
}

CORO_DEFINE( ut_coro )
{
	CORO_LOCAL CORO_CONTEXT( test_1 );
	CORO_LOCAL CORO_CONTEXT( test_2 );
	CORO_LOCAL CORO_CONTEXT( test_3 );
	CORO_LOCAL CORO_CONTEXT( test_4 );

	CORO_BEGIN();

	ut_2_begin();
	CORO_YIELD();

	answer_last += CYCLE_COUNT;

	SEMAPHORE_INIT( b_empty, 0 );
	SEMAPHORE_INIT( b_full, B_SIZE );

	CORO_CONTEXT_INIT( test_1 );
	test_1_init();

	CORO_CONTEXT_INIT( test_2 );
	test_2_init();

	CORO_WAIT_CORO( CORO_CALL( test_1 ) | CORO_CALL( test_2 ));
	assert_int_eq( answer_last, answer );

	CORO_CONTEXT_INIT( test_3 );
	test_3_init();

	CORO_CONTEXT_INIT( test_4 );
	test_4_init();

	CORO_WAIT_CORO( CORO_CALL( test_3 ) | CORO_CALL( test_4 ));

	CORO_END();
}

/** \} */

#endif /* LINK_UT_CORO */
