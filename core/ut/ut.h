/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut.h
 *  \brief  Unit testing.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef UT_H
#define UT_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( UT_IMPLEMENT )
#include "h-begin.h"

#ifdef LINK_UT

/**
 *  \defgroup ut Unit testing
 *  \ingroup core
 *
 *  Based on Keith Nicholas http://code.google.com/p/seatest/ .
 *  \{
 */

#include <inttypes.h>

/**
 *  \brief Max length of message.
 *  \hideinitializer
 */
#define UT_STR_LIMIT  100

_EXT_C bool ut_assert_true( bool test, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_false( bool test, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_assigned( int data, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_int_equal( int expected, int actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_u32_equal( uint32_t expected, uint32_t actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_float_equal( float expected, float actual, float delta, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_double_equal( double expected, double actual, double delta, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_string_equal( const char *expected, const char *actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_string_ends_with( const char *expected, const char *actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_string_starts_with( const char *expected, const char *actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_string_contains( const char *expected, const char *actual, const char *path, const char *func, uint32_t line );
_EXT_C bool ut_assert_string_doesnt_contain( const char *expected, const char *actual, const char *path, const char *func, uint32_t line );

#define ut_fault_format( msg, expected, actual, ... ) do { \
	char _ut_s[ UT_STR_LIMIT ]; \
	snprintf( _ut_s, UT_STR_LIMIT, msg, expected, actual ); \
	ut_fault( _ut_s, ## __VA_ARGS__ ); \
} while ( 0 )

/**
 *  \brief Test failed.
 *  \param msg Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define assert_fail( msg, ... ) do { \
	char _ut_s[ UT_STR_LIMIT ]; \
	snprintf( _ut_s, UT_STR_LIMIT, msg, ## __VA_ARGS__ ); \
	ut_fault( _ut_s, __FILE__, __FUNCTION__, __LINE__ ); \
} while ( 0 )

#undef assert

/**
 *  \brief Verify expression is true.
 *  \param value Expression.
 *  \param ... Operation if verification failed.
 *  \hideinitializer
 */
#define assert( value, ... ) do { \
	if ( ut_assert_true( value, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_not( value, ... ) do { \
	if ( ut_assert_false( value, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_assigned( value, ... ) do { \
	if ( ut_assert_assigned(( int ) value, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_int_eq( expected, actual, ... ) do { \
	if ( ut_assert_int_equal( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_u32_eq( expected, actual, ... ) do { \
	if ( ut_assert_u32_equal( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_int_cmp( expected, actual, COND, ... ) do { \
	if ( !(( expected ) COND ( actual ))) { \
		ut_fault_format( "expected %d " #COND " but %d", expected, actual, __FILE__, __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_u32_cmp( expected, actual, COND, ... ) do { \
	if ( !(( expected ) COND ( actual ))) { \
		ut_fault_format( "expected %" PRIu32 " " #COND " but %" PRIu32, expected, actual \
		, __FILE__, __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_str_eq( expected, actual, ... ) do { \
	if ( ut_assert_string_equal( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_array_eq( expected, actual, count, ... ) do { \
	int _ut_i; \
	for ( _ut_i = 0; _ut_i < count; _ut_i++ ) { \
		char _ut_s[ UT_STR_LIMIT ]; \
		bool test = ( expected[ _ut_i ] == actual[ _ut_i ]); \
		if ( !test ) { \
			snprintf( _ut_s, UT_STR_LIMIT, "expected %d to be %d at pos %d", actual[ _ut_i ], expected[ _ut_i ], _ut_i ); \
			ut_fault( _ut_s, __FILE__, __FUNCTION__, __LINE__ ); \
			__VA_ARGS__; \
		} \
	} \
} while ( 0 )

#define assert_bit_1( bit, value, ... ) do { \
	bool test = ((( uint32_t ) 1 << ( bit )) & ( value )); \
	if ( !test ) { \
		ut_fault( "expected bit to be set", __FILE__,  __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_bit_0( bit, value, ... ) do { \
	bool test = !((( uint32_t ) 1 << ( bit )) & ( value )); \
	if ( !test ) { \
		ut_fault( "expected bit not to be set", __FILE__,  __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_bit_mask( value, mask, ... ) do { \
	bool test = ((( value ) & ( mask )) == ( mask )); \
	if ( !test ) { \
		ut_fault( "expected mask to be set", __FILE__,  __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_float_eq( expected, actual, delta, ... ) do { \
	if ( ut_assert_float_equal( expected, actual, delta, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_double_eq( expected, actual, delta, ... ) do { \
	if ( ut_assert_double_equal( expected, actual, delta, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_float_cmp( expected, actual, COND, ... ) do { \
	if ( !(( expected ) COND ( actual ))) { \
		ut_fault_format( "expected %f " #COND " but %f", expected, actual, __FILE__, __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

#define assert_double_cmp  assert_float_cmp

#define assert_str_has( expected, actual, ... ) do { \
	if ( ut_assert_string_contains( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_str_hasno( expected, actual, ... ) do { \
	if ( ut_assert_string_doesnt_contain( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_str_start( expected, actual, ... ) do { \
	if ( ut_assert_string_starts_with( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

#define assert_str_end( expected, actual, ... ) do { \
	if ( ut_assert_string_ends_with( expected, actual, __FILE__, __FUNCTION__, __LINE__ )) { __VA_ARGS__; } \
} while ( 0 )

_EXT_C void ut_fault( const char *error, const char *path, const char *func, uint32_t line );
_EXT_C void ut_quiet( void );
_EXT_C void ut_loud( void );
_EXT_C uint32_t ut_get_quiet( void );

_EXT_C void ut_1_unit_begin( const char *path );
_EXT_C void ut_1_unit_end( void );
_EXT_C void ut_2_unit_begin( const char *path );

/**
 *  \brief Verify quiet mode is off.
 *  \hideinitializer
 */
#define ut_assert_loud() do { \
	int quiet = ut_get_quiet(); \
	if ( quiet ) { \
		char _ut_s[ UT_STR_LIMIT ]; \
		snprintf( _ut_s, UT_STR_LIMIT, "QUIET should be disabled ( %d fault(s))", quiet - 1 ); \
		ut_loud(); \
		ut_fault( _ut_s, __FILE__, __FUNCTION__, __LINE__ ); \
	} \
} while ( 0 )

/**
 *  \brief Start verification in quiet mode (without failure messages).
 *  \hideinitializer
 */
#define ut_quiet_begin() do { \
	ut_assert_loud(); \
	ut_quiet(); \
} while ( 0 )

/**
 *  \brief Complete verification in quiet mode.
 *  \param expected Expected count of failures (in quiet mode).
 *  \hideinitializer
 */
#define ut_quiet_end( expected ) do { \
	int quiet = ut_get_quiet(); \
	if ( quiet ) { \
		ut_loud(); \
		ut_assert_int_equal( expected, quiet - 1, __FILE__, __FUNCTION__, __LINE__ ); \
	} \
} while ( 0 )

/**
 *  \brief Beginning of group of unit test of 1st stage.
 *  \hideinitializer
 */
#define ut_1_begin() do { \
	ut_1_unit_begin( __FILE__ ); \
} while ( 0 )

/**
 *  \brief Run unit test of 1st stage.
 *  \param test Unit test.
 */
_EXT_C void ut_1_exec( void ( *test )( void ));

/**
 *  \brief End of group of unit test.
 *  \hideinitializer
 */
#define ut_1_end() do { \
	ut_1_unit_end(); \
} while ( 0 )

/**
 *  \brief 1st stage of unit testing.
 *  \param test Set of tests.
 */
_EXT_C bool ut_stage_1( void ( *test )( void ));

/**
 *  \brief Beginning of group of unit test of 2nd stage (coroutine).
 *  \hideinitializer
 */
#define ut_2_begin() do { \
	ut_2_unit_begin( __FILE__ ); \
} while ( 0 )

/**
 *  \brief Start unit testing of 2nd stage.
 *  \param count Tests count.
 */
_EXT_C void ut_stage_2_start( int count );

/**
 *  \brief Call test coroutines.
 *  \param test Schedule.
 *  \return true if testing is not complete.
 */
_EXT_C bool ut_stage_2( int ( *test )( void ));

/**
 *  \brief Print statistics of unit testing.
 */
_EXT_C void ut_print_statistics( void );

/**
*  \brief Get timer value in centiseconds (callback).
 */
_VAR_C( uint32_t ( *ut_get_time_callback )( void ));

/**
 *  \brief Event handler of testing stage starting (callback).
 *  \param t Stage of testing.
 */
_VAR_C( void ( *ut_started_callback )( int t ));

/**
 *  \brief Event handler of testing stage completion (callback).
 *  \param t Stage of testing.
 *  \param result Testing result.
 */
_VAR_C( void ( *ut_finished_callback )( int t, bool result ));

/**
 *  \brief Lock common data access (callback).
 */
_VAR_C( void ( *ut_lock_callback )( void ));

/**
 *  \brief Unlock common data access (callback).
 */
_VAR_C( void ( *ut_unlock_callback )( void ));

/** \} */

#else  /* LINK_UT */

#include <stddef.h>

#define assert_fail( msg, ... ) NULL
#undef  assert
#define assert( value, ... ) NULL
#define assert_not( value, ... ) NULL
#define assert_assigned( value, ... ) NULL
#define assert_int_eq( expected, actual, ... ) NULL
#define assert_u32_eq( expected, actual, ... ) NULL
#define assert_int_cmp( expected, actual, COND, ... ) NULL
#define assert_u32_cmp( expected, actual, COND, ... ) NULL
#define assert_str_eq( expected, actual, ... ) NULL
#define assert_array_eq( expected, actual, count, ... ) NULL
#define assert_bit_1( bit, value, ... ) NULL
#define assert_bit_0( bit, value, ... ) NULL
#define assert_bit_mask( value, mask, ... ) NULL
#define assert_float_eq( expected, actual, delta, ... ) NULL
#define assert_double_eq( expected, actual, delta, ... ) NULL
#define assert_float_cmp( expected, actual, COND, ... ) NULL
#define assert_double_cmp( expected, actual, COND, ... ) NULL
#define assert_str_has( expected, actual, ... ) NULL
#define assert_str_hasno( expected, actual, ... ) NULL
#define assert_str_start( expected, actual, ... ) NULL
#define assert_str_end( expected, actual, ... ) NULL

#endif /* LINK_UT */

#include "h-end.h"

#endif /* UT_H */
