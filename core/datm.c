
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datm.c
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define DATM_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datm
 *  \{
 */

typedef int __type_t;
typedef int __length_t;
typedef int __changed_t;

/**
 *  \brief Descriptor of state variable.
 */
struct DATM_DESCRY {
	/**  Variable name */
	const char *name;
	/**  Variable title */
	const char *title;
	/**  Type */
	__type_t type;
	/**  Array length */
	__length_t len;
	/**  Variable offset in state-space */
	size_t offset;
	/**  value-to-string function */
	void *string;
};

typedef const struct DATM_DESCRY *datm_descry_t;

static uint8_t __updated[ DATM_DEPTH ];
static char __str[ 100 ];

static __inline void __set_changed( const varm_t item, const depthm_t n );
static __inline void *__datm_var( const varm_t item, const depthm_t n );

/**
 *  \brief State-space.
 */
static DATM_VECTOR datm_vector[ DATM_DEPTH ];

/** \cond */

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
	{ __stringify( var ), title, typem_##type, 1, offsetof( DATM_VECTOR, var ), ( void *) string },

#define T( N, len, var, type, title, string ) \
	{ __stringify( var##_##N ), title " (" __stringify( N ) ")", typem_##type, ( len - N ), offsetof( DATM_VECTOR, var[ N ]), ( void *) string },

#define DATM_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

/**
 *  \brief Descriptor of state-space.
 */
static const struct DATM_DESCRY datm_descry[] = {
#include "datm-dict.h"
	{ "none", "none", typem_COUNT, 0, 0, strm_byte },
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

#define DATM_THREAD( thread ) \
	static __changed_t datm_changed_##thread[ varm_COUNT ]; \
	static __changed_t datm_mask_##thread[ varm_COUNT ];

#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread ) datm_changed_##thread,
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of datm changes flags of variables.
 */
static __changed_t *datm_changed[] = {
#include "datm-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread ) datm_mask_##thread,
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of watching masks.
 */
static __changed_t *datm_mask[] = {
#include "datm-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) __stringify( T ),
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

static const char *datm_type_string[] = {
#include "datm-dict.h"
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) typedef char *( *strm_##T##_t )( char *, T, int * );
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

/**
 *  \brief value-to-string functions prototypes.
 */
#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
char *strm_##T( char *s, T value, int *col ) \
{ \
	( void ) col; \
	sprintf( s, format, value ); \
	return ( s ); \
}
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

static __inline void *__datm_var( const varm_t item, const depthm_t n )
{
	return ( void * )(( size_t )( datm_vector + n ) + datm_descry[ item ].offset );
}

static __inline int __datm_filter( const char *name )
{

#ifdef DATM_PROT
	return ((( *ini_datm_prot_filter      == '\0' ) || ( strstr( name, ini_datm_prot_filter      ) != 0 ))
		 && (( *ini_datm_prot_filter_skip == '\0' ) || ( strstr( name, ini_datm_prot_filter_skip ) == 0 ))
	);

#else
	return ( 0 );

#endif
}

static __inline void __datm_print( const char *name, const depthm_t n, const char *value )
{

#ifdef DATM_PROT
	printf( __CRLF__"\t" );
	printf( clock_stamp( NULL, __str ));
	printf( "   %-22s[ %d ]  %s", name, n, value );

#endif
}

#define DATM_CHK_THREAD( thread, R ) do { \
	if ( thread >= peekm_COUNT ) { \
		assert_fail( "invalid thread index (%d)", thread ); \
		return R; \
	} \
} while ( 0 )

#define DATM_CHK_VAR( item, op, R ) do { \
	if ( item >= varm_COUNT ) { \
		assert_fail( "attempt to %s var with invalid index (%d)", op, item ); \
		return R; \
	} \
} while ( 0 )

#define DATM_CHK_TYPE( item, T, op, R ) do { \
	if ( datm_descry[ item ].type != typem_##T ) { \
		assert_fail( "attempt to %s \"%s\" (%d) as %s while it's %s" \
		, op \
		, datm_descry[ item ].name \
		, item \
		, datm_type_string[ typem_##T ] \
		, datm_type_string[ datm_descry[ item ].type ] \
		); \
		return R; \
	} \
} while ( 0 )

#define DATM_CHK_N( n, R ) do { \
	if ( n >= DATM_DEPTH ) { \
		assert_fail( "invalid state-space index (%d)", n ); \
		return R; \
	} \
} while ( 0 )

#define DATM_LOCK() do { \
	if ( datm_lock_callback ) datm_lock_callback(); \
} while ( 0 )

#define DATM_UNLOCK() do { \
	if ( datm_unlock_callback ) datm_unlock_callback(); \
} while ( 0 )

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
T datm_get_##T( const varm_t item, const depthm_t n ) \
{ \
	T result; \
	DATM_CHK_N( n, 0 ); \
	DATM_CHK_VAR( item, "get", 0 ); \
	DATM_CHK_TYPE( item, T, "get", 0 ); \
	DATM_LOCK(); \
	result = *(( T *) __datm_var( item, n )); \
	DATM_UNLOCK(); \
	return ( result ); \
} \
\
char *datm_get_string_##T( char *s, const varm_t item, const depthm_t n ) \
{ \
	char *result; \
	int col; \
	DATM_CHK_N( n, NULL ); \
	DATM_CHK_VAR( item, "get", NULL ); \
	DATM_CHK_TYPE( item, T, "get", NULL ); \
	DATM_LOCK(); \
	result = (( strm_##T##_t ) datm_descry[ item ].string )( s, *(( T *) __datm_var( item, n )), &col ); \
	DATM_UNLOCK(); \
	return ( result ); \
} \
\
void datm_set_##T( const varm_t item, T value, const depthm_t n ) \
{ \
	T *var; \
	int col; \
	DATM_CHK_N( n, ); \
	DATM_CHK_VAR( item, "set", ); \
	DATM_CHK_TYPE( item, T, "set", ); \
	var = ( T *) __datm_var( item, n ); \
	DATM_LOCK(); \
	if ( *var != value ) { \
		*var = value; \
		__set_changed( item, n ); \
		if ( ini_datm_prot ) { \
			const char *name = datm_descry[ item ].name; \
			if ( __datm_filter( name ))	{ \
				__datm_print( name, n, (( strm_##T##_t ) datm_descry[ item ].string )( __str, *var, &col )); \
			} \
		} \
	} \
	DATM_UNLOCK(); \
}
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

char *datm_get_string( char *s, const varm_t item, const depthm_t n )
{
	switch ( datm_descry[ item ].type ) {

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
	case typem_##T: \
		return datm_get_string_##T( s, item, n );

#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

	default:
		return "?";
	}
}

void datm_set_string( const varm_t item, const char *s, const depthm_t n )
{
	char *end;

	switch ( datm_descry[ item ].type ) {

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
	case typem_##T: \
		do { \
			T val = ( T ) strto( s, &end ); \
			if ( *end == '\0' ) { \
				datm_set_##T( item, val, n ); \
			} else { \
				assert_fail( "attempt to set invalid \"%s\" %s value (%s)" \
				, datm_descry[ item ].name \
				, datm_type_string[ typem_##T ] \
				, s \
				); \
			} \
		} while ( 0 ); \
		return;

#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
	}
}

void datm_set( const varm_t item, void *value, const depthm_t n )
{
	switch ( datm_descry[ item ].type ) {

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
	case typem_##T: \
		datm_set_##T( item, *( T *) value, n ); \
		break;

#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
	}
}

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
	if ( datm_vector[ n ].var != datm->var ) { \
		datm_vector[ n ].var = datm->var; \
		__set_changed( varm_##var, n ); \
	}

#define T( N, var, type, title, string ) \
	if ( datm_vector[ n ].var[ N ] != datm->var[ N ]) { \
		datm_vector[ n ].var[ N ] = datm->var[ N ]; \
		__set_changed( varm_##var##_##N, n ); \
	}

#define DATM_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datm_set_vector( const DATM_VECTOR *datm, const depthm_t n )
{
	DATM_CHK_N( n, 0 );

	DATM_LOCK();

#ifdef    LINK_UT_DATM
#define __LINK_UT_DATM__
#undef    LINK_UT_DATM
#endif

#include "datm-dict.h"

#ifdef __LINK_UT_DATM__
#define  LINK_UT_DATM
#undef __LINK_UT_DATM__
#endif

	DATM_UNLOCK();

	return ( 1 );
}

int datm_set_vector__ut( const DATM_VECTOR *datm, const depthm_t n )
{
	DATM_CHK_N( n, 0 );

	DATM_LOCK();

#include "datm-dict.h"

	DATM_UNLOCK();

	return ( 1 );
}

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
	if ( datm1->var != datm2->var ) return ( 0 );

#define T( N, var, type, title, string ) \
	if ( datm1->var[ N ] != datm2->var[ N ]) return ( 0 );

#define DATM_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datm_cmp_vector( const DATM_VECTOR *datm1, const DATM_VECTOR *datm2 )
{

#ifdef    LINK_UT_DATM
#define __LINK_UT_DATM__
#undef    LINK_UT_DATM
#endif

#include "datm-dict.h"

#ifdef __LINK_UT_DATM__
#define  LINK_UT_DATM
#undef __LINK_UT_DATM__
#endif

	return ( 1 );
}

int datm_cmp_vector__ut( const DATM_VECTOR *datm1, const DATM_VECTOR *datm2 )
{

#include "datm-dict.h"

	return ( 1 );
}

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

/** \endcond */

DATM_VECTOR *datm_get_vector( const depthm_t n )
{
	DATM_CHK_N( n, NULL );

	return ( datm_vector + n );
}

static __inline void __set_changed( const varm_t item, const depthm_t n )
{
	threadm_t i;
	__changed_t mask = 1UL << n;

	for ( i = 0; i < peekm_COUNT; i++ ) {
		if ( datm_mask[ i ][ item ]) datm_changed[ i ][ item ] |= mask;
	}
}

void datm_set_changed( const varm_t item, const depthm_t n )
{
	DATM_CHK_N( n, );
	DATM_CHK_VAR( item, "set change of", );
	DATM_LOCK();
	__set_changed( item, n );
	DATM_UNLOCK();
}

void datm_set_all_changed( const threadm_t thread, const depthm_t n )
{
	varm_t i;
	__changed_t mask;

	DATM_CHK_THREAD( thread, );
	mask = ( n < DATM_DEPTH ) ? 1UL << n : ( __changed_t ) DATM_MASK;
	DATM_LOCK();
	for ( i = 0; i < varm_COUNT; i++ ) {
		if ( datm_mask[ thread ][ i ]) datm_changed[ thread ][ i ] |= mask;
	}
	DATM_UNLOCK();
}

static int __get_changed( const threadm_t thread, const varm_t item, const depthm_t n )
{
	return (( datm_changed[ thread ][ item ] & ( 1UL << n )) != 0 );
}

int datm_get_changed_wo_lock( const threadm_t thread, const varm_t item, const depthm_t n )
{
	int result;

	DATM_CHK_THREAD( thread, 0 );
	DATM_CHK_N( n, 0 );
	DATM_CHK_VAR( item, "get change of", 0 );
	result = __get_changed( thread, item, n );

	return ( result );
}

int datm_get_changed( const threadm_t thread, const varm_t item, const depthm_t n )
{
	int result;

	DATM_CHK_THREAD( thread, 0 );
	DATM_CHK_N( n, 0 );
	DATM_CHK_VAR( item, "get change of", 0 );
	DATM_LOCK();
	result = __get_changed( thread, item, n );
	DATM_UNLOCK();

	return ( result );
}

int datm_get_changed_any( const threadm_t thread, const depthm_t n )
{
	varm_t i;
	__changed_t mask;

	DATM_CHK_THREAD( thread, 0 );
	DATM_CHK_N( n, 0 );
	mask = 1UL << n;
	DATM_LOCK();
	for ( i = 0; i < varm_COUNT; i++ ) {
		if ( datm_changed[ thread ][ i ] & mask ) {
			DATM_UNLOCK();

			return ( 1 );
		}
	}
	DATM_UNLOCK();

	return ( 0 );
}

void datm_watch( const threadm_t thread, const varm_t *vars )
{
	DATM_CHK_THREAD( thread, );
	DATM_LOCK();
	while ( *vars < varm_COUNT ) datm_mask[ thread ][ *vars++ ] = 1;
	DATM_UNLOCK();
}

void datm_watch_array( const threadm_t thread, const varm_t *vars )
{
	DATM_CHK_THREAD( thread, );
	DATM_LOCK();
	while ( *vars < varm_COUNT ) {
		varm_t i;

		for ( i = *vars + datm_descry[ *vars ].len - 1; i >= *vars; i-- ) {
			datm_mask[ thread ][ i ] = 1;
		}
		++vars;
	}
	DATM_UNLOCK();
}

void datm_clear_watch( const thread_t thread )
{
	varm_t i;

	DATM_CHK_THREAD( thread, );
	DATM_LOCK();
	for ( i = 0; i < varm_COUNT; i++ ) {
		datm_mask[ thread ][ i ] = 0;
		datm_changed[ thread ][ i ] = 0;
	}
	DATM_UNLOCK();
}

void datm_reset( const threadm_t thread, const varm_t item, const depthm_t n )
{
	DATM_CHK_THREAD( thread, );
	DATM_CHK_N( n, );
	DATM_CHK_VAR( item, "set change of", );
	DATM_LOCK();
	datm_changed[ thread ][ item ] &= ~( 1UL << n );
	DATM_UNLOCK();
}

void datm_reset_all( const threadm_t thread, const depthm_t n )
{
	varm_t i;
	__changed_t mask;

	DATM_CHK_THREAD( thread, );
	DATM_CHK_N( n, );
	mask = ~( 1UL << n );
	DATM_LOCK();
	for ( i = 0; i < varm_COUNT; i++ ) datm_changed[ thread ][ i ] &= mask;
	DATM_UNLOCK();
}

varm_t datm_atovar( const char *s, const varm_t from, const varm_t to )
{
	varm_t i;

	for ( i = from; i <= to; i++ ) {
		if ( strcmp( datm_descry[ i ].name, s ) == 0 ) return ( i );
	}
	return ( varm_COUNT );
}

char *datm_vartoa( const varm_t item )
{
	DATM_CHK_VAR( item, "get name of", "" );

	return (( char *) datm_descry[ item ].name );
}

char *datm_get_type( const varm_t item )
{
	DATM_CHK_VAR( item, "get type of", "" );

	return (( char *) datm_type_string[ datm_descry[ item ].type ]);
}

char *datm_get_title( const varm_t item )
{
	DATM_CHK_VAR( item, "get title of", "" );

	return (( char *) datm_descry[ item ].title );
}

void datm_lock( void )
{
	DATM_LOCK();
}

void datm_unlock( void )
{
	DATM_UNLOCK();
}

void datm_clear_updated( const depthm_t n )
{
	DATM_CHK_N( n, );

	DATM_LOCK();
	__updated[ n ] = 0;
	DATM_UNLOCK();
}

void datm_set_updated( const depthm_t n )
{
	DATM_CHK_N( n, );

	DATM_LOCK();
	__updated[ n ] = 1;
	DATM_UNLOCK();
}

uint8_t datm_check_updated( const depthm_t n )
{
	uint8_t result;

	DATM_CHK_N( n, 0 );

	DATM_LOCK();
	result = __updated[ n ];
	DATM_UNLOCK();

	return ( result );
}

/** \} */

