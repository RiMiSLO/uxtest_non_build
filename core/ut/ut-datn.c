/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut-datn.c
 *  \brief  Unit test of array of state vectors.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#include "include.h"

#ifdef LINK_UT_DATN

/**
 *  \defgroup ut_datn Unit test of array of state vectors
 *  \ingroup ut
 *  \{
 */

/* Count of testing cycles */
#define CYCLE_COUNT  ( USHRT_MAX + 1 )

static char __str[ 100 ];

static const int _ut_datn_1_watch[] = {
	varn__ut_word,
	varn__ut_array_3,
	varn__ut_float,
	varn_COUNT
};

static const int _ut_datn_2_watch[] = {
	varn__ut_byte,
	varn__ut_array_1,
	varn__ut_float,
	varn_COUNT
};

static const int _ut_datn_array_watch[] = {
	varn__ut_byte,
	varn__ut_array_1,
	varn_COUNT
};

int _ut_datn_1_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( _ut_datn_1 )
{
	static int i;

	CORO_BEGIN();

	datn_clear_watch( peekn__ut_datn_1 );
	datn_watch( peekn__ut_datn_1, _ut_datn_1_watch );

	/* Test of error messages */
	ut_quiet_begin();

	datn_set_byte( varn_COUNT, 0, 0 );                    /* Wrong variable */
	datn_set_dword( varn__ut_word, 0, 0 );                /* Wrong type */
	datn_get_word( varn_COUNT, 0 );                       /* Wrong variable */
	datn_get_float( varn__ut_byte, 0 );                   /* Wrong type */
	datn_get_changed( peekn_COUNT, varn__ut_array_2, 0 ); /* Wrong thread */
	datn_get_word( varn__ut_word, DATN_DEPTH );           /* Wrong array index */

	ut_quiet_end( 6 );                            /* 6 errors */

	datn_set_byte( varn__ut_byte, 1, 1 );
	datn_set_dword( varn__ut_array_1, 1, 4 );
	datn_set_qword( varn__ut_qword, ( qword ) _MAXDWORD + 1, 4 );
	datn_set_float( varn__ut_float, 0.0, 0 );
	datn_reset_all( peekn__ut_datn_2, 0 );
	datn_reset_all( peekn__ut_datn_2, 2 );
	datn_reset_all( peekn__ut_datn_2, 4 );

	/* Main operations */
	for ( i = 0; i < CYCLE_COUNT; i++ ) {

/* 1 */	datn_set_byte( varn__ut_byte, i, 1 );

		CORO_WAIT(( datn_get_changed( peekn__ut_datn_1, varn__ut_word, 2 )), );
/* 3 */	datn_reset( peekn__ut_datn_1, varn__ut_word, 2 );
		assert_int_eq(( word ) i, datn_get_word( varn__ut_word, 2 ), );

		datn_set_dword( varn__ut_array_1, ( dword ) i << 8, 4 );

		CORO_WAIT(( datn_get_changed( peekn__ut_datn_1, varn__ut_array_3, 4 )), );
/* 5 */	datn_reset( peekn__ut_datn_1, varn__ut_array_3, 4 );
		assert_int_eq( datn_get_dword( varn__ut_array_1, 4 ), datn_get_dword( varn__ut_array_3, 4 ), );

		datn_set_dword( varn__ut_array_0, datn_get_dword( varn__ut_array_2, 4 ), 4 );
		datn_set_dword( varn__ut_array_2, 0, 4 );

		datn_set_float( varn__ut_float, datn_get_float( varn__ut_float, 0 ) + 0.25, 0 );
		datn_reset( peekn__ut_datn_1, varn__ut_float, 0 );
		CORO_WAIT(( datn_get_changed( peekn__ut_datn_1, varn__ut_float, 0 )), );
	}
	assert_int_eq( 0x55555555, datn_get_dword( varn__ut_array_0, 4 ), );

	/* Auxiliary operations */
	datn_set_all_changed( peekn__ut_datn_1, DATN_DEPTH );
	assert(( datn_get_changed( peekn__ut_datn_1, varn__ut_word, 2 )
		  && datn_get_changed( peekn__ut_datn_1, varn__ut_array_3, 4 )
		  && datn_get_changed( peekn__ut_datn_1, varn__ut_float, 0 )
		  && !datn_get_changed( peekn__ut_datn_1, varn__ut_array_1, 4 )
		  && !datn_get_changed( peekn__ut_datn_1, varn__ut_array_2, 4 ))
	, );
	datn_reset_all( peekn__ut_datn_1, 0 );
	datn_reset_all( peekn__ut_datn_1, 1 );
	datn_reset_all( peekn__ut_datn_1, 2 );
	datn_reset_all( peekn__ut_datn_1, 4 );
	assert_not(
	   datn_get_changed_any( peekn__ut_datn_1, 0 )
	|| datn_get_changed_any( peekn__ut_datn_1, 1 )
	|| datn_get_changed_any( peekn__ut_datn_1, 2 )
	|| datn_get_changed_any( peekn__ut_datn_1, 4 )
	, );
	datn_set_changed( varn__ut_byte, 1 );
	assert_not( datn_get_changed_any( peekn__ut_datn_1, 1 ), );
	datn_set_changed( varn__ut_float, 0 );
	assert( datn_get_changed_any( peekn__ut_datn_1, 0 ), );

	datn_clear_watch( peekn__ut_datn_1 );
	assert_not(
	   datn_get_changed_any( peekn__ut_datn_1, 0 )
	|| datn_get_changed_any( peekn__ut_datn_1, 2 )
	|| datn_get_changed_any( peekn__ut_datn_1, 4 )
	, );
	datn_watch_array( peekn__ut_datn_1, _ut_datn_array_watch );
	datn_set_changed( varn__ut_float, 0 );
	datn_set_changed( varn__ut_word, 2 );
	datn_set_changed( varn__ut_array_0, 4 );
	assert_not(
	   datn_get_changed_any( peekn__ut_datn_1, 0 )
	|| datn_get_changed_any( peekn__ut_datn_1, 2 )
	|| datn_get_changed_any( peekn__ut_datn_1, 4 )
	, );
	datn_set_changed( varn__ut_array_1, 4 );
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_array_1, 4 ), );
	datn_set_changed( varn__ut_array_2, 4 );
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_array_2, 4 ), );
	datn_set_changed( varn__ut_array_3, 4 );
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_array_3, 4 ), );
	datn_set_changed( varn__ut_byte, 1 );
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_byte, 1 ), );

	assert_int_eq( varn__ut_array, datn_atovar( datn_vartoa( varn__ut_array_0 ), varn__ut_byte, varn__ut_float ), );
	assert_str_eq( "_ut_byte", datn_vartoa( datn_atovar( "_ut_byte", 0, varn_COUNT - 1 )), );

	_AS_DWORD( i ) = 0xABCDEF;
	datn_set( varn__ut_array_3, &i, 4 );
	assert_str_eq( "0x00ABCDEF", datn_get_string( __str, varn__ut_array_3, 4 ), );
	datn_set_string( varn__ut_float, "8080.02", 0 );
	assert_str_eq( "8080.02", datn_get_string( __str, varn__ut_float, 0 ), );

	assert_str_eq( "word", datn_get_type( varn__ut_word ), );
	assert_str_eq( "_ut_float", datn_get_title( varn__ut_float ), );

    /* datn_set_vector */
	datn_clear_watch( peekn__ut_datn_1 );
	datn_watch_array( peekn__ut_datn_1, _ut_datn_1_watch );
	assert_not( datn_get_changed_any( peekn__ut_datn_1, DATN_DEPTH - 2 ), );
	assert_not( datn_cmp_vector__ut( datn_get_vector( 0 ), datn_get_vector( DATN_DEPTH - 2 )), ); /* No DATN_LOCK! */
	datn_set_vector__ut( datn_get_vector( 0 ), DATN_DEPTH - 2 );
	assert( datn_cmp_vector__ut( datn_get_vector( 0 ), datn_get_vector( DATN_DEPTH - 2 )), ); /* No DATN_LOCK! */
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_float, DATN_DEPTH - 2 ), );

	assert_not( datn_get_changed_any( peekn__ut_datn_1, DATN_DEPTH - 3 ), );
	assert_not( datn_cmp_vector__ut( datn_get_vector( 4 ), datn_get_vector( DATN_DEPTH - 3 )), ); /* No DATN_LOCK! */
	datn_set_vector__ut( datn_get_vector( 4 ), DATN_DEPTH - 3 );
	assert( datn_cmp_vector__ut( datn_get_vector( 4 ), datn_get_vector( DATN_DEPTH - 3 )), ); /* No DATN_LOCK! */
	assert( datn_get_changed( peekn__ut_datn_1, varn__ut_array_3, DATN_DEPTH - 3 ), );
	assert( datn_get_qword( varn__ut_qword, DATN_DEPTH - 3 ) == (( qword ) _MAXDWORD + 1 ), );

	CORO_END();
}

int _ut_datn_2_init( void )
{
	datn_watch( peekn__ut_datn_2, _ut_datn_2_watch );

	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( _ut_datn_2 )
{
	static int i;

	CORO_BEGIN();

	datn_set_word( varn__ut_word, 1, 2 );
	datn_set_dword( varn__ut_array_3, 1, 4 );
	datn_reset_all( peekn__ut_datn_1, 0 );
	datn_reset_all( peekn__ut_datn_1, 1 );
	datn_reset_all( peekn__ut_datn_1, 4 );
	datn_set_dword( varn__ut_array_0, 0x55555555, 4 );

	for ( i = 0; i < CYCLE_COUNT; i++ ) {

		CORO_WAIT(( datn_get_changed( peekn__ut_datn_2, varn__ut_byte, 1 )), );
/* 2 */	datn_reset( peekn__ut_datn_2, varn__ut_byte, 1 );
		assert_int_eq(( byte ) i, datn_get_byte( varn__ut_byte, 1 ), );

		datn_set_word( varn__ut_word, i, 2 );

		CORO_WAIT(( datn_get_changed( peekn__ut_datn_2, varn__ut_array_1, 4 )), );
/* 4 */	datn_reset( peekn__ut_datn_2, varn__ut_array_1, 4 );

		datn_set_dword( varn__ut_array_3, ( dword ) i << 8, 4 );
		assert_int_eq( datn_get_dword( varn__ut_array_3, 4 ), datn_get_dword( varn__ut_array_1, 4 ), );

		datn_set_dword( varn__ut_array_2, datn_get_dword( varn__ut_array_0, 4 ), 4 );
		datn_set_dword( varn__ut_array_0, 0, 4 );

		CORO_WAIT(( datn_get_changed( peekn__ut_datn_2, varn__ut_float, 0 )), );
/* 6 */	datn_set_float( varn__ut_float, datn_get_float( varn__ut_float, 0 ) + 0.25, 0 );
		datn_reset( peekn__ut_datn_2, varn__ut_float, 0 );
	}
	assert_float_eq(( float )( CYCLE_COUNT >> 1 ), datn_get_float( varn__ut_float, 0 ), 0.0, );

	i = 0x000000;
	assert_int_eq( 0x000001, sec2bcd( bcd2sec( i ) + 1 ), );
	i = 0x000059;
	assert_int_eq( 0x000111, sec2bcd( bcd2sec( i ) + 12 ), );
	i = 0x005959;
	assert_int_eq( 0x010101, sec2bcd( bcd2sec( i ) + 62 ), );
	i = 0x995959;
	assert_int_eq( 0x000000, sec2bcd( bcd2sec( i ) + 1 ), );

	CORO_END();
}

int ut_datn_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

void ut_datn_uninit( void )
{
}

CORO_DEFINE( ut_datn )
{
	static CORO_CONTEXT( _ut_datn_1 );
	static CORO_CONTEXT( _ut_datn_2 );

	CORO_BEGIN();

	ut_2_begin();
	CORO_YIELD();

	CORO_CONTEXT_INIT( _ut_datn_1 );
	_ut_datn_1_init();

	CORO_CONTEXT_INIT( _ut_datn_2 );
	_ut_datn_2_init();

	CORO_WAIT_CORO( CORO_CALL( _ut_datn_1 ) | CORO_CALL( _ut_datn_2 ), );

	CORO_END();
}

/** \} */

#endif /* LINK_UT_DATN */
