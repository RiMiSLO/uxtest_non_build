/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut.c
 *  \brief  Unit testing.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define UT_IMPLEMENT
#include "include.h"

#ifdef LINK_UT

/**
 *  \addtogroup ut
 *  \{
 */

#define ________ "................................................................................" \
                 "................................................................................"
#define __PASSED__              "P A S S E D"
#define __FAILED__              "F A I L E D"

static uint32_t __executed = 0;
static uint32_t __failed = 0;
static uint32_t __quiet = 0;
static uint32_t __executed_before;
static uint32_t __failed_before;

static uint32_t ut_executed[ 3 ] = { 0 };
static uint32_t ut_failed[ 3 ] = { 0 };

static unsigned long ut_start, ut_stop;

enum {
	ut_thread_IDLE,
	ut_thread_START,
	ut_thread_EXEC,
};
static int ut_thread = ut_thread_IDLE;
static char *ut_unit = NULL;

static __inline uint32_t __get_time( void )
{
	return ( ut_get_time_callback ) ? ut_get_time_callback() : 0;
}

static __inline void __started_callback( int t )
{
	if ( ut_started_callback ) ut_started_callback( t );
}

static __inline void __finished_callback( int t,  bool result )
{
	if ( ut_finished_callback ) ut_finished_callback( t, result );
}

static __inline void __lock( void )
{
	if ( ut_lock_callback ) ut_lock_callback();
}

static __inline void __unlock( void )
{
	if ( ut_unlock_callback ) ut_unlock_callback();
}

bool ut_assert_true( bool test, const char *path, const char *func, uint32_t line )
{
	if ( !test ) ut_fault( "should be true", path, func, line );

	return ( !test );
}

bool ut_assert_false( bool test, const char *path, const char *func, uint32_t line )
{
	if ( test ) ut_fault( "should be false", path, func, line );

	return ( test );
}

bool ut_assert_assigned( int data, const char *path, const char *func, uint32_t line )
{
	if ( data == 0 ) ut_fault( "should be assigned", path, func, line );

	return ( data == 0 );
}

bool ut_assert_int_equal( int expected, int actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( expected == actual );
	if ( !test ) {
		ut_fault_format( "expected %d but %d", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_u32_equal( uint32_t expected, uint32_t actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( expected == actual );
	if ( !test ) {
		ut_fault_format( "expected %" PRIu32 " but %" PRIu32, expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_float_equal( float expected, float actual, float delta, const char *path, const char *func, uint32_t line )
{
	float result = expected - actual;
	bool test;

	if ( result < 0.0 ) result = 0.0 - result;
	test = ( result <= delta );
	if ( !test ) {
		ut_fault_format( "expected %f but %f", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_double_equal( double expected, double actual, double delta, const char *path, const char *func, uint32_t line )
{
	double result = expected - actual;
	bool test;

	if ( result < 0.0 ) result = 0.0 - result;
	test = ( result <= delta );
	if ( !test ) {
		ut_fault_format( "expected %f but %f", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_string_equal( const char *expected, const char *actual, const char *path, const char *func, uint32_t line )
{
	bool test;
	if (( expected == NULL ) && ( actual == NULL )) {
		test = true;
	} else if ( expected == NULL ) {
		test = false;
		expected = "<NULL>";
	} else if ( actual == NULL ) {
		test = false;
		actual = "<NULL>";
	} else {
		test = ( strcmp( expected, actual ) == 0 );
	}
	if ( !test ) {
		ut_fault_format( "expected %s but %s", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_string_ends_with( const char *expected, const char *actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( strcmp( expected, actual + ( strlen( actual ) - strlen( expected ))) == 0 );
	if ( !test ) {
		ut_fault_format( "expected %s to end with %s", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_string_starts_with( const char *expected, const char *actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( strncmp( expected, actual, strlen( expected )) == 0 );
	if ( !test ) {
		ut_fault_format( "expected %s to start with %s", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_string_contains( const char *expected, const char *actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( strstr( actual, expected ) != 0 );
	if ( !test ) {
		ut_fault_format( "expected %s to be in %s", expected, actual, path, func, line );
	}
	return ( !test );
}

bool ut_assert_string_doesnt_contain( const char *expected, const char *actual, const char *path, const char *func, uint32_t line )
{
	bool test = ( strstr( actual, expected ) == 0 );
	if ( !test ) {
		ut_fault_format( "expected %s not to have %s in it", expected, actual, path, func, line );
	}
	return ( !test );
}

static char *__get_name( const char *path )
{
	char *name = strrchr( path, '\\' );

	return ( name ) ? name + 1 : ( char *) path;
}

void __print_header( const char *s, int len, char ch )
{
	int l = strlen( s );
	int d = ( len - ( l + 2 )) / 2;
	int i;

	printf( __CRLF__ );
	for ( i = 0; i < d; i++ ) putchar( ch );
	putchar( ' ' );
	printf( s );
	putchar( ' ' );
	for ( i = d + l + 2; i < len; i++ ) putchar( ch );
}

static void __print_result( bool result )
{
#ifdef PREDICT_USE_FTOA
	char s[ 16 ];

#endif
	printf( __CRLF__ );
	printf( "%.*s", ( int ) console_col_count, ________ );
	printf( __CRLF__ __CRLF__"%*s", ( int )( console_col_count + strlen( __PASSED__ )) / 2, ( result ) ? __PASSED__ : __FAILED__ );
	printf( __CRLF__ __CRLF__"  %d fault(s) in %d test(s)", __failed, __executed );

#ifdef PREDICT_USE_FTOA
	printf( " for %s sec"__CRLF__, ftoa(( float )( ut_stop - ut_start ) / 10, s, 1 ));

#else
	printf( " for %.1f sec"__CRLF__, ( float )( ut_stop - ut_start ) / 10 );

#endif
	printf( "%.*s", ( int ) console_col_count, ________ );
	flush();
}

void ut_print_statistics( void )
{
	printf( __CRLF__"  ut_stage_1() - %d fault(s) in %d test(s)", ut_failed[ 1 ], ut_executed[ 1 ]);
	printf( __CRLF__"  ut_stage_2() - %d fault(s) in %d test(s)", ut_failed[ 2 ], ut_executed[ 2 ]);
	if ( ut_thread != ut_thread_IDLE ) printf( " ( running... )" );
	printf( __CRLF__"  assert()     - %d fault(s) beyond test(s)", ut_failed[ 0 ] + __failed );
}

void ut_fault( const char *error, const char *path, const char *func, uint32_t line )
{
	if ( __quiet ) {
		++__quiet;
		return;
	}
	++__failed;
	__lock();
	if ( ut_unit == NULL ) __print_header( __get_name( path ), console_col_count, '.' );
	printf( __CRLF__"%-22s %-4d %s", func, line, error );
	__unlock();
}

void ut_quiet( void )
{
	__quiet = 1;
}

void ut_loud( void )
{
	__quiet = 0;
}

uint32_t ut_get_quiet( void )
{
	return __quiet;
}

void ut_1_exec( void ( *test )( void ))
{
	test();
	++__executed;
}

void ut_1_unit_begin( const char *path )
{
	ut_unit = __get_name( path );
	__print_header( ut_unit, console_col_count, '.' );
	__failed_before = __failed;
	__executed_before = __executed;
}

void ut_1_unit_end( void )
{
	printf( __CRLF__ __CRLF__"  %d fault(s) in %d test(s)", __failed - __failed_before, __executed - __executed_before );
	ut_unit = NULL;
}

bool ut_stage_1( void ( *test )( void ))
{
	bool result;

	__started_callback( 1 );

	ut_failed[ 0 ] += __failed;
	__failed = 0;
	ut_loud();

	ut_start = __get_time();
	test();
	ut_stop = __get_time();

	ut_assert_loud();
	result = ( __failed == 0 );
	__print_result( result );
	__finished_callback( 1, result );

	ut_executed[ 1 ] += __executed;
	__executed = 0;
	ut_failed[ 1 ] += __failed;
	__failed = 0;

	return ( result );
}

void ut_2_unit_begin( const char *path )
{
	__print_header( __get_name( path ), console_col_count, '.' );
}

void ut_stage_2_start( int count )
{
	__executed = count;
	ut_thread = ut_thread_START;

	__started_callback( 2 );
}

bool ut_stage_2( int ( *test )( void ))
{
	bool result;

	switch ( ut_thread ) {

	case ut_thread_START:

		ut_failed[ 0 ] += __failed;
		__failed = 0;
		ut_loud();

		ut_unit = "thread";
		ut_start = __get_time();
		ut_thread = ut_thread_EXEC;

		test();

		return ( true );

	case ut_thread_EXEC:

		if ( test()) return ( true );

		ut_stop = __get_time();
		ut_thread = ut_thread_IDLE;

		ut_assert_loud();
		result = ( __failed == 0 );
		__print_result( result );
		__finished_callback( 2, result );
		ut_executed[ 2 ] += __executed;
		__executed = 0;
		ut_failed[ 2 ] += __failed;
		__failed = 0;
		ut_unit = NULL;
		/* fallthrough */

	default:
		return ( false );
	}
}

/** \} */

#endif /* LINK_UT */
