/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut-data.c
 *  \brief  Unit test of application state-space.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#include "include.h"

#ifdef LINK_UT_DATA

/**
 *  \defgroup ut_data Unit test of application state-space
 *  \ingroup ut
 *  \{
 */

/* Count of testing cycles */
#define CYCLE_COUNT  (( int ) USHRT_MAX + 1 )

static char __str[ 100 ];

static const int _ut_data_1_watch[] = {
	var__ut_word,
	var__ut_array_3,
	var__ut_float,
	var_COUNT
};

static const int _ut_data_2_watch[] = {
	var__ut_byte,
	var__ut_array_1,
	var__ut_float,
	var__ut_vote,
	var_COUNT
};

static const int _ut_data_array_watch[] = {
	var__ut_byte,
	var__ut_array_1,
	var_COUNT
};

static void test_error( void )
{
	/* Test of error messages */
	ut_quiet_begin();

	data_set_byte( var_COUNT, 0 );                   /* Wrong variable */
	data_set_dword( var__ut_word, 0 );               /* Wrong type */
	data_get_word( var_COUNT );                      /* Wrong variable */
	data_get_float( var__ut_byte );                  /* Wrong type */
	data_get_changed( peek_COUNT, var__ut_array_2 ); /* Wrong thread */

	ut_quiet_end( 5 );                               /* 5 errors */
}

void ut_data( void )
{
	ut_1_begin();
	ut_1_exec( test_error );
	ut_1_end();
}

int _ut_data_1_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( _ut_data_1 )
{
	CORO_LOCAL dword i;

	CORO_BEGIN();

	data_clear_watch( peek__ut_data_1 );
	data_watch( peek__ut_data_1, _ut_data_1_watch );

	data_set_byte( var__ut_byte, 1 );
	data_set_dword( var__ut_array_1, 1 );
	data_set_float( var__ut_float, 0.0 );
	data_reset_all( peek__ut_data_2 );

	/* Main operations */
	for ( i = 0; i < CYCLE_COUNT; i++ ) {
/* 1 */	data_set_byte( var__ut_byte, ( byte ) i );

		CORO_WAIT( data_get_changed( peek__ut_data_1, var__ut_word ));
/* 3 */	data_reset( peek__ut_data_1, var__ut_word );
		assert_u32_eq(( word ) i, data_get_word( var__ut_word ));

		data_set_dword( var__ut_array_1, i << 8 );

		CORO_WAIT( data_get_changed( peek__ut_data_1, var__ut_array_3 ));
/* 5 */	data_reset( peek__ut_data_1, var__ut_array_3 );
		assert_u32_eq( data_get_dword( var__ut_array_1 ), data_get_dword( var__ut_array_3 ));

		data_set_dword( var__ut_array_0, data_get_dword( var__ut_array_2 ));
		data_set_dword( var__ut_array_2, 0 );

		data_set_float( var__ut_float, data_get_float( var__ut_float ) + 0.25 );
		data_reset( peek__ut_data_1, var__ut_float );
		CORO_WAIT( data_get_changed( peek__ut_data_1, var__ut_float ));
	}
	assert_u32_eq( 0x55555555, data_get_dword( var__ut_array_0 ));

	/* Auxiliary operations */
	data_set_all_changed( peek__ut_data_1 );
	assert(( data_get_changed( peek__ut_data_1, var__ut_word )
	&& data_get_changed( peek__ut_data_1, var__ut_array_3 )
	&& data_get_changed( peek__ut_data_1, var__ut_float )
	&& !data_get_changed( peek__ut_data_1, var__ut_array_1 )
	&& !data_get_changed( peek__ut_data_1, var__ut_array_2 ))
	);
	data_reset_all( peek__ut_data_1 );
	assert_not( data_get_changed_any( peek__ut_data_1 ));
	data_set_changed( var__ut_byte );
	assert_not( data_get_changed_any( peek__ut_data_1 ));
	data_set_changed( var__ut_float );
	assert( data_get_changed_any( peek__ut_data_1 ));

	CORO_YIELD();

	data_clear_watch( peek__ut_data_1 );
	assert_not( data_get_changed_any( peek__ut_data_1 ));
	data_watch_array( peek__ut_data_1, _ut_data_array_watch );
	data_set_changed( var__ut_float );
	data_set_changed( var__ut_word );
	data_set_changed( var__ut_array_0 );
	assert_not( data_get_changed_any( peek__ut_data_1 ));
	data_set_changed( var__ut_array_1 );
	assert( data_get_changed( peek__ut_data_1, var__ut_array_1 ));
	data_set_changed( var__ut_array_2 );
	assert( data_get_changed( peek__ut_data_1, var__ut_array_2 ));
	data_set_changed( var__ut_array_3 );
	assert( data_get_changed( peek__ut_data_1, var__ut_array_3 ));
	data_set_changed( var__ut_byte );
	assert( data_get_changed( peek__ut_data_1, var__ut_byte ));

	CORO_YIELD();

	assert_u32_eq( var__ut_array, data_atovar( data_vartoa( var__ut_array_0 )));
	assert_str_eq( "_ut_byte", data_vartoa( data_atovar( "_ut_byte" )));

	i = 0x00ABCDEF;
	data_set( var__ut_array_3, &i );
	assert_str_eq( "0x00ABCDEF", data_get_string( __str, var__ut_array_3 ));
	data_set_string( var__ut_float, "8080.02" );
	assert_str_eq( "8080.02", data_get_string( __str, var__ut_float ));

	assert_str_eq( "word", data_get_type( var__ut_word ));

	CORO_END();
}

int _ut_data_2_init( void )
{
	data_watch( peek__ut_data_2, _ut_data_2_watch );

	return ( CO_READY ); /* CO_SKIP */
}

CORO_DEFINE( _ut_data_2 )
{
	CORO_LOCAL dword i;

	CORO_BEGIN();

	data_set_word( var__ut_word, 1 );
	data_set_dword( var__ut_array_3, 1 );
	data_reset_all( peek__ut_data_1 );
	data_set_dword( var__ut_array_0, 0x55555555 );

	for ( i = 0; i < CYCLE_COUNT; i++ ) {
		CORO_WAIT( data_get_changed( peek__ut_data_2, var__ut_byte ));
/* 2 */	data_reset( peek__ut_data_2, var__ut_byte );
		assert_u32_eq(( byte ) i, data_get_byte( var__ut_byte ));

		data_set_word( var__ut_word, ( word ) i );

		CORO_WAIT( data_get_changed( peek__ut_data_2, var__ut_array_1 ));
/* 4 */	data_reset( peek__ut_data_2, var__ut_array_1 );

		data_set_dword( var__ut_array_3, i << 8 );
		assert_u32_eq( data_get_dword( var__ut_array_3 ), data_get_dword( var__ut_array_1 ));

		data_set_dword( var__ut_array_2, data_get_dword( var__ut_array_0 ));
		data_set_dword( var__ut_array_0, 0 );

		CORO_WAIT( data_get_changed( peek__ut_data_2, var__ut_float ));
/* 6 */	data_set_float( var__ut_float, data_get_float( var__ut_float ) + 0.25 );
		data_reset( peek__ut_data_2, var__ut_float );
	}
	assert_float_eq(( float )( CYCLE_COUNT >> 1 ), data_get_float( var__ut_float ), 0.0 );

	/* Voting operations */
	/* I: -  II: -  III: -  M: -  */
	assert_not( data_get_changed( peek__ut_data_2, var__ut_vote ));
	assert_not( voted_valid( var__ut_vote ));
	/* I: -  II: 0  III: -  M: -  */
	voted_set_byte( var__ut_vote, 2, 0, &vote_byte );
	assert_not( data_get_changed( peek__ut_data_2, var__ut_vote ));
	assert_not( voted_valid( var__ut_vote ));
	/* I: -  II: 0  III: 0  M: 0  */
	voted_set_byte( var__ut_vote, 3, 0, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 0, data_get_byte( var__ut_vote ));
	/* I: -  II: 0  III: 1  M: -  */
	voted_set_byte( var__ut_vote, 3, 1, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert_not( voted_valid( var__ut_vote ));

	CORO_YIELD();

	/* I: 1  II: 0  III: 1  M: 1  */
	voted_set_byte( var__ut_vote, 1, 1, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 1, data_get_byte( var__ut_vote ));
	/* I: 1  II: 0  III: 0  M: 0  */
	voted_set_byte( var__ut_vote, 3, 0, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 0, data_get_byte( var__ut_vote ));
	/* I: 1  II: 2  III: 0  M: -  */
	voted_set_byte( var__ut_vote, 2, 2, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert_not( voted_valid( var__ut_vote ));
	/* I: 3  II: 2  III: 3  M: 3  */
	voted_set_byte( var__ut_vote, 1, 3, &vote_byte );
	voted_set_byte( var__ut_vote, 3, 3, &vote_byte );
	assert( data_get_changed( peek__ut_data_2, var__ut_vote ));
	data_reset( peek__ut_data_2, var__ut_vote );
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 3, data_get_byte( var__ut_vote ));

	CORO_YIELD();

	/* I: 3  II: 3  III: 3  M: 3  */
	voted_set_byte( var__ut_vote, 2, 3, &vote_byte );
	assert_not( data_get_changed( peek__ut_data_2, var__ut_vote ));
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 3, data_get_byte( var__ut_vote ));
	/* I: 4  II: 3  III: 3  M: 3  */
	voted_set_byte( var__ut_vote, 1, 4, &vote_byte );
	assert_not( data_get_changed( peek__ut_data_2, var__ut_vote ));
	assert( voted_valid( var__ut_vote ));
	assert_u32_eq( 3, data_get_byte( var__ut_vote ));
	/* RESET */
	voted_set_byte( var__ut_vote, 3, 2, &vote_byte );
	data_reset( peek__ut_data_2, var__ut_vote );

	CORO_END();
}

int ut_data_init( void )
{
	return ( CO_READY ); /* CO_SKIP */
}

void ut_data_uninit( void )
{
}

CORO_DEFINE( ut_data )
{
	CORO_LOCAL CORO_CONTEXT( _ut_data_1 );
	CORO_LOCAL CORO_CONTEXT( _ut_data_2 );

	CORO_BEGIN();

	ut_2_begin();
	CORO_YIELD();

	CORO_CONTEXT_INIT( _ut_data_1 );
	_ut_data_1_init();

	CORO_CONTEXT_INIT( _ut_data_2 );
	_ut_data_2_init();

	CORO_WAIT_CORO( CORO_CALL( _ut_data_1 ) | CORO_CALL( _ut_data_2 ));

	CORO_END();
}

/** \} */

#endif /* LINK_UT_DATA */
