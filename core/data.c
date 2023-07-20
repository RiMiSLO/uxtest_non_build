/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data.c
 *  \brief  Application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define DATA_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup data
 *  \{
 */

typedef int __type_t;
typedef int __length_t;
typedef int __changed_t;

/**
 *  \brief Descriptor of state variable.
 */
struct DATA_DESCRY {
	/**  Variable name */
	const char *name;
	/**  Type */
	__type_t type;
	/**  Array length */
	__length_t len;
	/**  Data pointer */
	void *data;
	/**  value-to-string function */
	void *string;
};

typedef const struct DATA_DESCRY *data_descry_t;

static char __str[ 100 ] __attribute__(( unused ));

static __inline void __set_changed( const var_t item, const __changed_t value );

/** \cond */

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string ) type var;
#define DATA_ARRAY( len, var, type, string ) type var[ len ];

/**
 *  \brief Structure of state-space.
 */
struct DATA_SPACE {
#include "data-dict.h"
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


/**
 *  \brief State-space.
 */
static struct DATA_SPACE data_space;


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string ) \
	{ __stringify( var ), type_##type, 1, &data_space.var, ( void *) string },

#define T( N, len, var, type, string ) \
	{ __stringify( var##_##N ), type_##type, ( len - N ), data_space.var + N, ( void *) string },

#define DATA_ARRAY( len, var, type, string ) \
	UNFOLD( len, T, T, len, var, type, string )

/**
 *  \brief Descriptor of state-space.
 */
static const struct DATA_DESCRY data_descry[] = {
#include "data-dict.h"
	{ "none", type_COUNT, 0, NULL, string_byte },
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY
#undef T


#define DATA_THREAD( thread ) \
	static __changed_t data_changed_##thread[ var_COUNT ]; \
	static __changed_t data_mask_##thread[ var_COUNT ];

#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread ) data_changed_##thread,
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

/**
 *  \brief Matrix of data changes flags of variables.
 */
static __changed_t *data_changed[] = {
#include "data-dict.h"
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread ) data_mask_##thread,
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

/**
 *  \brief Matrix of watching masks.
 */
static __changed_t *data_mask[] = {
#include "data-dict.h"
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) __stringify( T ),
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

static const char *data_type_string[] = {
#include "data-dict.h"
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) typedef char *( *string_##T##_t )( char *, T );
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

/**
 *  \brief value-to-string functions prototypes.
 */
#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
char *string_##T( char *s, T value ) \
{ \
	sprintf( s, format, value ); \
	return ( s ); \
}
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_CHK_THREAD( thread, R ) do { \
	if ( thread >= peek_COUNT ) { \
		assert_fail( "invalid thread inx (%d)", thread ); \
		return R; \
	} \
} while ( 0 )

#define DATA_CHK_VAR( item, op, R ) do { \
	if ( item >= var_COUNT ) { \
		assert_fail( "attempt to %s var with invalid inx (%d)", op, item ); \
		return R; \
	} \
} while ( 0 )

#define DATA_CHK_TYPE( item, T, op, R ) do { \
	if ( data_descry[ item ].type != type_##T ) { \
		assert_fail( "attempt to %s \"%s\" (%d) as %s while it's %s" \
		, op \
		, data_descry[ item ].name \
		, item \
		, data_type_string[ type_##T ] \
		, data_type_string[ data_descry[ item ].type ] \
		); \
		return R; \
	} \
} while ( 0 )

#define DATA_LOCK() do { \
	if ( data_lock_callback ) data_lock_callback(); \
} while ( 0 )

#define DATA_UNLOCK() do { \
	if ( data_unlock_callback ) data_unlock_callback(); \
} while ( 0 )

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
T data_get_##T( const var_t item ) \
{ \
	T result; \
	DATA_CHK_VAR( item, "get", 0 ); \
	DATA_CHK_TYPE( item, T, "get", 0 ); \
	DATA_LOCK(); \
	result = *(( T *) data_descry[ item ].data ); \
	DATA_UNLOCK(); \
	return ( result ); \
} \
\
char *data_get_string_##T( char *s, const var_t item ) \
{ \
	char *result; \
	DATA_CHK_VAR( item, "get", NULL ); \
	DATA_CHK_TYPE( item, T, "get", NULL ); \
	DATA_LOCK(); \
	result = (( string_##T##_t ) data_descry[ item ].string )( s, *(( T *) data_descry[ item ].data )); \
	DATA_UNLOCK(); \
	return ( result ); \
} \
\
void data_set_##T( const var_t item, T value ) \
{ \
	DATA_CHK_VAR( item, "set", ); \
	DATA_CHK_TYPE( item, T, "set", ); \
	data_descry_t d = data_descry + item; \
	T *var = ( T *) d->data; \
	DATA_LOCK(); \
	if ( *var != value ) { \
		*var = value; \
		__set_changed( item, 1 ); \
		if ( ini_data_prot ) { \
			if ((( *ini_data_prot_filter == '\0' ) \
			  || ( strstr( d->name, ini_data_prot_filter ) != 0 )) \
			 && (( *ini_data_prot_filter_skip == '\0' ) \
			  || ( strstr( d->name, ini_data_prot_filter_skip ) == 0 ))) { \
				printf( __CRLF__"\t" ); \
				printf( clock_stamp( NULL, __str )); \
				printf( "   %-22s  %s" \
				, d->name \
				, (( string_##T##_t ) d->string )( __str, *var ) \
				); \
			} \
		} \
	} \
	DATA_UNLOCK(); \
}
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


char *data_get_string( char *s, const var_t item )
{
	switch ( data_descry[ item ].type ) {

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	case type_##T: \
		return data_get_string_##T( s, item );

#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY

	default:
		return "?";
	}
}

void data_set_string( const var_t item, const char *s )
{
	char *end;

	switch ( data_descry[ item ].type ) {

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	case type_##T: \
		do { \
			T val = ( T ) strto( s, &end ); \
			if ( *end == '\0' ) { \
				data_set_##T( item, val ); \
			} else { \
				assert_fail( "attempt to set invalid \"%s\" %s value (%s)" \
				, data_descry[ item ].name \
				, data_type_string[ type_##T ] \
				, s \
				); \
			} \
		} while ( 0 ); \
		return;

#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY
	}
}

void data_set( const var_t item, void *value )
{
	switch ( data_descry[ item ].type ) {

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	case type_##T: \
		data_set_##T( item, *( T *) value ); \
		break;

#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY
	}
}

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
void voted_set_##T( const var_t item, byte h, T value, bool ( *vote )( T *, bool, T, bool, T, bool, T )) \
{ \
	T result = data_get_##T( item ); \
\
	if (( h < 1 ) || ( h > 3 )) { \
		assert_fail( "attempt to set invalid \"%s\" channel (%d)" \
		, data_descry[ item ].name \
		, h \
		); \
		return; \
	} \
	if ( data_get_byte( item + h + 4 ) == 0 ) { \
		  data_set_byte( item + h + 4, 1 ); \
	} else { \
		if ( value == data_get_##T( item + h )) \
			/* Nothing changed */ \
			return; \
	} \
	data_set_##T( item + h, value ); \
\
	if ( vote( &result \
	, data_get_byte( item + 5 ), data_get_##T( item + 1 ) \
	, data_get_byte( item + 6 ), data_get_##T( item + 2 ) \
	, data_get_byte( item + 7 ), data_get_##T( item + 3 )) \
	) { \
		data_set_##T( item, result ); \
		if ( !data_get_byte( item + 4 )) { \
			  data_set_byte( item + 4, 1 ); \
			  data_set_changed( item ); \
		} \
	} else { \
		/* Invalid voted */ \
		if ( data_get_byte( item + 4 )) { \
			 data_set_byte( item + 4, 0 ); \
			 data_set_changed( item ); \
		} \
	} \
} \
\
void joint_set_##T( const var_t item, T value ) \
{ \
	data_set_byte( item + 4,     1 ); \
	data_set_byte( item + 4 + 1, 1 ); \
	data_set_byte( item + 4 + 2, 1 ); \
	data_set_byte( item + 4 + 3, 1 ); \
	data_set_##T( item, value ); \
	data_set_##T( item + 1, value ); \
	data_set_##T( item + 2, value ); \
	data_set_##T( item + 3, value ); \
}

#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY

/** \endcond */

static __inline void __set_changed( const var_t item, const __changed_t value )
{
	thread_t i;

	for ( i = 0; i < peek_COUNT; i++ ) {
		if ( data_mask[ i ][ item ]) data_changed[ i ][ item ] = value;
	}
}

void data_set_changed( const var_t item )
{
	DATA_CHK_VAR( item, "set change of", );
	DATA_LOCK();
	__set_changed( item, 1 );
	DATA_UNLOCK();
}

void data_reset_changed( const var_t item )
{
	DATA_CHK_VAR( item, "reset change of", );
	DATA_LOCK();
	__set_changed( item, 0 );
	DATA_UNLOCK();
}

void data_set_all_changed( const thread_t thread )
{
	var_t i;

	DATA_CHK_THREAD( thread, );
	DATA_LOCK();
	for ( i = 0; i < var_COUNT; i++ ) {
		if ( data_mask[ thread ][ i ]) data_changed[ thread ][ i ] = 1;
	}
	DATA_UNLOCK();
}

int data_get_changed( const thread_t thread, const var_t item )
{
	int result;

	DATA_CHK_THREAD( thread, 0 );
	DATA_CHK_VAR( item, "get change of", 0 );
	DATA_LOCK();
	result = (( int ) data_changed[ thread ][ item ]);
	DATA_UNLOCK();

	return ( result );
}

int data_get_changed_any( const thread_t thread )
{
	var_t i;

	DATA_CHK_THREAD( thread, 0 );
	DATA_LOCK();
	for ( i = 0; i < var_COUNT; i++ ) {
		if ( data_changed[ thread ][ i ]) {
			DATA_UNLOCK();

			return ( 1 );
		}
	}
	DATA_UNLOCK();

	return ( 0 );
}

void data_watch( const thread_t thread, const var_t *vars )
{
	DATA_CHK_THREAD( thread, );
	DATA_LOCK();
	while ( *vars < var_COUNT ) data_mask[ thread ][ *vars++ ] = 1;
	DATA_UNLOCK();
}

void data_watch_array( const thread_t thread, const var_t *vars )
{
	DATA_CHK_THREAD( thread, );
	DATA_LOCK();
	while ( *vars < var_COUNT ) {
		var_t i;

		for ( i = *vars + data_descry[ *vars ].len - 1; i >= *vars; i-- ) {
			data_mask[ thread ][ i ] = 1;
		}
		++vars;
	}
	DATA_UNLOCK();
}

void data_clear_watch( const thread_t thread )
{
	var_t i;

	DATA_CHK_THREAD( thread, );
	DATA_LOCK();
	for ( i = 0; i < var_COUNT; i++ ) {
		data_mask[ thread ][ i ] = 0;
		data_changed[ thread ][ i ] = 0;
	}
	DATA_UNLOCK();
}

void data_reset( const thread_t thread, const var_t item )
{
	DATA_CHK_THREAD( thread, );
	DATA_CHK_VAR( item, "set change of", );
	DATA_LOCK();
	data_changed[ thread ][ item ] = 0;
	DATA_UNLOCK();
}

void data_reset_all( const thread_t thread )
{
	var_t i;

	DATA_CHK_THREAD( thread, );
	DATA_LOCK();
	for ( i = 0; i < var_COUNT; i++ ) data_changed[ thread ][ i ] = 0;
	DATA_UNLOCK();
}

bool voted_valid( const var_t item )
{
	return ( data_get_byte( item + 4 ));
}

var_t data_atovar( const char *s )
{
	var_t i;

	for ( i = 0; i < var_COUNT; i++ ) {
		if ( strcmp( data_descry[ i ].name, s ) == 0 ) return ( i );
	}
	return ( var_COUNT );
}

char *data_vartoa( const var_t item )
{
	DATA_CHK_VAR( item, "get name of", "" );

	return (( char *) data_descry[ item ].name );
}

char *data_get_type( const var_t item )
{
	DATA_CHK_VAR( item, "get type of", "" );

	return (( char *) data_type_string[ data_descry[ item ].type ]);
}

/** \} */

int popcount( int i )
{
	i = i - (( i >> 1 ) & 0x55555555 );
	i = ( i & 0x33333333 ) + (( i >> 2 ) & 0x33333333 );

	return ((( i + ( i >> 4 )) & 0x0F0F0F0F ) * 0x01010101 ) >> 24;
}
