
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datn.c
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define DATN_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datn
 *  \{
 */

typedef int __type_t;
typedef int __length_t;
typedef int __changed_t;

/**
 *  \brief Descriptor of state variable.
 */
struct DATN_DESCRY {
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

typedef const struct DATN_DESCRY *datn_descry_t;

static uint8_t __updated[ DATN_DEPTH ];
static char __str[ 100 ];

static __inline void __set_changed( const varn_t item, const depthn_t n );
static __inline void *__datn_var( const varn_t item, const depthn_t n );

/**
 *  \brief State-space.
 */
static DATN_VECTOR datn_vector[ DATN_DEPTH ];

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
	{ __stringify( var ), title, typen_##type, 1, offsetof( DATN_VECTOR, var ), ( void *) string },

#define T( N, len, var, type, title, string ) \
	{ __stringify( var##_##N ), title " (" __stringify( N ) ")", typen_##type, ( len - N ), offsetof( DATN_VECTOR, var[ N ]), ( void *) string },

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

/**
 *  \brief Descriptor of state-space.
 */
static const struct DATN_DESCRY datn_descry[] = {
#include "datn-dict.h"
	{ "none", "none", typen_COUNT, 0, 0, strn_byte },
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

#define DATN_THREAD( thread ) \
	static __changed_t datn_changed_##thread[ varn_COUNT ]; \
	static __changed_t datn_mask_##thread[ varn_COUNT ];

#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread ) datn_changed_##thread,
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of datn changes flags of variables.
 */
static __changed_t *datn_changed[] = {
#include "datn-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread ) datn_mask_##thread,
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of watching masks.
 */
static __changed_t *datn_mask[] = {
#include "datn-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) __stringify( T ),
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

static const char *datn_type_string[] = {
#include "datn-dict.h"
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) typedef char *( *strn_##T##_t )( char *, T, int * );
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

/**
 *  \brief value-to-string functions prototypes.
 */
#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
char *strn_##T( char *s, T value, int *col ) \
{ \
	( void ) col; \
	sprintf( s, format, value ); \
	return ( s ); \
}
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

static __inline void *__datn_var( const varn_t item, const depthn_t n )
{
	return ( void * )(( size_t )( datn_vector + n ) + datn_descry[ item ].offset );
}

static __inline int __datn_filter( const char *name )
{

#ifdef DATN_PROT
	return ((( *ini_datn_prot_filter      == '\0' ) || ( strstr( name, ini_datn_prot_filter      ) != 0 ))
		 && (( *ini_datn_prot_filter_skip == '\0' ) || ( strstr( name, ini_datn_prot_filter_skip ) == 0 ))
	);

#else
	return ( 0 );

#endif
}

static __inline void __datn_print( const char *name, const depthn_t n, const char *value )
{

#ifdef DATN_PROT
	printf( __CRLF__"\t" );
	printf( clock_stamp( NULL, __str ));
	printf( "   %-22s[ %d ]  %s", name, n, value );

#endif
}

#define DATN_CHK_THREAD( thread, R ) do { \
	if ( thread >= peekn_COUNT ) { \
		assert_fail( "invalid thread index (%d)", thread ); \
		return R; \
	} \
} while ( 0 )

#define DATN_CHK_VAR( item, op, R ) do { \
	if ( item >= varn_COUNT ) { \
		assert_fail( "attempt to %s var with invalid index (%d)", op, item ); \
		return R; \
	} \
} while ( 0 )

#define DATN_CHK_TYPE( item, T, op, R ) do { \
	if ( datn_descry[ item ].type != typen_##T ) { \
		assert_fail( "attempt to %s \"%s\" (%d) as %s while it's %s" \
		, op \
		, datn_descry[ item ].name \
		, item \
		, datn_type_string[ typen_##T ] \
		, datn_type_string[ datn_descry[ item ].type ] \
		); \
		return R; \
	} \
} while ( 0 )

#define DATN_CHK_N( n, R ) do { \
	if ( n >= DATN_DEPTH ) { \
		assert_fail( "invalid state-space index (%d)", n ); \
		return R; \
	} \
} while ( 0 )

#define DATN_LOCK() do { \
	if ( datn_lock_callback ) datn_lock_callback(); \
} while ( 0 )

#define DATN_UNLOCK() do { \
	if ( datn_unlock_callback ) datn_unlock_callback(); \
} while ( 0 )

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
T datn_get_##T( const varn_t item, const depthn_t n ) \
{ \
	T result; \
	DATN_CHK_N( n, 0 ); \
	DATN_CHK_VAR( item, "get", 0 ); \
	DATN_CHK_TYPE( item, T, "get", 0 ); \
	DATN_LOCK(); \
	result = *(( T *) __datn_var( item, n )); \
	DATN_UNLOCK(); \
	return ( result ); \
} \
\
char *datn_get_string_##T( char *s, const varn_t item, const depthn_t n ) \
{ \
	char *result; \
	int col; \
	DATN_CHK_N( n, NULL ); \
	DATN_CHK_VAR( item, "get", NULL ); \
	DATN_CHK_TYPE( item, T, "get", NULL ); \
	DATN_LOCK(); \
	result = (( strn_##T##_t ) datn_descry[ item ].string )( s, *(( T *) __datn_var( item, n )), &col ); \
	DATN_UNLOCK(); \
	return ( result ); \
} \
\
void datn_set_##T( const varn_t item, T value, const depthn_t n ) \
{ \
	T *var; \
	int col; \
	DATN_CHK_N( n, ); \
	DATN_CHK_VAR( item, "set", ); \
	DATN_CHK_TYPE( item, T, "set", ); \
	var = ( T *) __datn_var( item, n ); \
	DATN_LOCK(); \
	if ( *var != value ) { \
		*var = value; \
		__set_changed( item, n ); \
		if ( ini_datn_prot ) { \
			const char *name = datn_descry[ item ].name; \
			if ( __datn_filter( name ))	{ \
				__datn_print( name, n, (( strn_##T##_t ) datn_descry[ item ].string )( __str, *var, &col )); \
			} \
		} \
	} \
	DATN_UNLOCK(); \
}
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

char *datn_get_string( char *s, const varn_t item, const depthn_t n )
{
	switch ( datn_descry[ item ].type ) {

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
	case typen_##T: \
		return datn_get_string_##T( s, item, n );

#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

	default:
		return "?";
	}
}

void datn_set_string( const varn_t item, const char *s, const depthn_t n )
{
	char *end;

	switch ( datn_descry[ item ].type ) {

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
	case typen_##T: \
		do { \
			T val = ( T ) strto( s, &end ); \
			if ( *end == '\0' ) { \
				datn_set_##T( item, val, n ); \
			} else { \
				assert_fail( "attempt to set invalid \"%s\" %s value (%s)" \
				, datn_descry[ item ].name \
				, datn_type_string[ typen_##T ] \
				, s \
				); \
			} \
		} while ( 0 ); \
		return;

#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
	}
}

void datn_set( const varn_t item, void *value, const depthn_t n )
{
	switch ( datn_descry[ item ].type ) {

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
	case typen_##T: \
		datn_set_##T( item, *( T *) value, n ); \
		break;

#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
	}
}

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
	if ( datn_vector[ n ].var != datn->var ) { \
		datn_vector[ n ].var = datn->var; \
		__set_changed( varn_##var, n ); \
	}

#define T( N, var, type, title, string ) \
	if ( datn_vector[ n ].var[ N ] != datn->var[ N ]) { \
		datn_vector[ n ].var[ N ] = datn->var[ N ]; \
		__set_changed( varn_##var##_##N, n ); \
	}

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datn_set_vector( const DATN_VECTOR *datn, const depthn_t n )
{
	DATN_CHK_N( n, 0 );

	DATN_LOCK();

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

#include "datn-dict.h"

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif

	DATN_UNLOCK();

	return ( 1 );
}

int datn_set_vector__ut( const DATN_VECTOR *datn, const depthn_t n )
{
	DATN_CHK_N( n, 0 );

	DATN_LOCK();

#include "datn-dict.h"

	DATN_UNLOCK();

	return ( 1 );
}

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
	if ( datn1->var != datn2->var ) return ( 0 );

#define T( N, var, type, title, string ) \
	if ( datn1->var[ N ] != datn2->var[ N ]) return ( 0 );

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datn_cmp_vector( const DATN_VECTOR *datn1, const DATN_VECTOR *datn2 )
{

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

#include "datn-dict.h"

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif

	return ( 1 );
}

int datn_cmp_vector__ut( const DATN_VECTOR *datn1, const DATN_VECTOR *datn2 )
{

#include "datn-dict.h"

	return ( 1 );
}

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */

DATN_VECTOR *datn_get_vector( const depthn_t n )
{
	DATN_CHK_N( n, NULL );

	return ( datn_vector + n );
}

static __inline void __set_changed( const varn_t item, const depthn_t n )
{
	threadn_t i;
	__changed_t mask = 1UL << n;

	for ( i = 0; i < peekn_COUNT; i++ ) {
		if ( datn_mask[ i ][ item ]) datn_changed[ i ][ item ] |= mask;
	}
}

void datn_set_changed( const varn_t item, const depthn_t n )
{
	DATN_CHK_N( n, );
	DATN_CHK_VAR( item, "set change of", );
	DATN_LOCK();
	__set_changed( item, n );
	DATN_UNLOCK();
}

void datn_set_all_changed( const threadn_t thread, const depthn_t n )
{
	varn_t i;
	__changed_t mask;

	DATN_CHK_THREAD( thread, );
	mask = ( n < DATN_DEPTH ) ? 1UL << n : ( __changed_t ) DATN_MASK;
	DATN_LOCK();
	for ( i = 0; i < varn_COUNT; i++ ) {
		if ( datn_mask[ thread ][ i ]) datn_changed[ thread ][ i ] |= mask;
	}
	DATN_UNLOCK();
}

static int __get_changed( const threadn_t thread, const varn_t item, const depthn_t n )
{
	return (( datn_changed[ thread ][ item ] & ( 1UL << n )) != 0 );
}

int datn_get_changed_wo_lock( const threadn_t thread, const varn_t item, const depthn_t n )
{
	int result;

	DATN_CHK_THREAD( thread, 0 );
	DATN_CHK_N( n, 0 );
	DATN_CHK_VAR( item, "get change of", 0 );
	result = __get_changed( thread, item, n );

	return ( result );
}

int datn_get_changed( const threadn_t thread, const varn_t item, const depthn_t n )
{
	int result;

	DATN_CHK_THREAD( thread, 0 );
	DATN_CHK_N( n, 0 );
	DATN_CHK_VAR( item, "get change of", 0 );
	DATN_LOCK();
	result = __get_changed( thread, item, n );
	DATN_UNLOCK();

	return ( result );
}

int datn_get_changed_any( const threadn_t thread, const depthn_t n )
{
	varn_t i;
	__changed_t mask;

	DATN_CHK_THREAD( thread, 0 );
	DATN_CHK_N( n, 0 );
	mask = 1UL << n;
	DATN_LOCK();
	for ( i = 0; i < varn_COUNT; i++ ) {
		if ( datn_changed[ thread ][ i ] & mask ) {
			DATN_UNLOCK();

			return ( 1 );
		}
	}
	DATN_UNLOCK();

	return ( 0 );
}

void datn_watch( const threadn_t thread, const varn_t *vars )
{
	DATN_CHK_THREAD( thread, );
	DATN_LOCK();
	while ( *vars < varn_COUNT ) datn_mask[ thread ][ *vars++ ] = 1;
	DATN_UNLOCK();
}

void datn_watch_array( const threadn_t thread, const varn_t *vars )
{
	DATN_CHK_THREAD( thread, );
	DATN_LOCK();
	while ( *vars < varn_COUNT ) {
		varn_t i;

		for ( i = *vars + datn_descry[ *vars ].len - 1; i >= *vars; i-- ) {
			datn_mask[ thread ][ i ] = 1;
		}
		++vars;
	}
	DATN_UNLOCK();
}

void datn_clear_watch( const thread_t thread )
{
	varn_t i;

	DATN_CHK_THREAD( thread, );
	DATN_LOCK();
	for ( i = 0; i < varn_COUNT; i++ ) {
		datn_mask[ thread ][ i ] = 0;
		datn_changed[ thread ][ i ] = 0;
	}
	DATN_UNLOCK();
}

void datn_reset( const threadn_t thread, const varn_t item, const depthn_t n )
{
	DATN_CHK_THREAD( thread, );
	DATN_CHK_N( n, );
	DATN_CHK_VAR( item, "set change of", );
	DATN_LOCK();
	datn_changed[ thread ][ item ] &= ~( 1UL << n );
	DATN_UNLOCK();
}

void datn_reset_all( const threadn_t thread, const depthn_t n )
{
	varn_t i;
	__changed_t mask;

	DATN_CHK_THREAD( thread, );
	DATN_CHK_N( n, );
	mask = ~( 1UL << n );
	DATN_LOCK();
	for ( i = 0; i < varn_COUNT; i++ ) datn_changed[ thread ][ i ] &= mask;
	DATN_UNLOCK();
}

varn_t datn_atovar( const char *s, const varn_t from, const varn_t to )
{
	varn_t i;

	for ( i = from; i <= to; i++ ) {
		if ( strcmp( datn_descry[ i ].name, s ) == 0 ) return ( i );
	}
	return ( varn_COUNT );
}

char *datn_vartoa( const varn_t item )
{
	DATN_CHK_VAR( item, "get name of", "" );

	return (( char *) datn_descry[ item ].name );
}

char *datn_get_type( const varn_t item )
{
	DATN_CHK_VAR( item, "get type of", "" );

	return (( char *) datn_type_string[ datn_descry[ item ].type ]);
}

char *datn_get_title( const varn_t item )
{
	DATN_CHK_VAR( item, "get title of", "" );

	return (( char *) datn_descry[ item ].title );
}

void datn_lock( void )
{
	DATN_LOCK();
}

void datn_unlock( void )
{
	DATN_UNLOCK();
}

void datn_clear_updated( const depthn_t n )
{
	DATN_CHK_N( n, );

	DATN_LOCK();
	__updated[ n ] = 0;
	DATN_UNLOCK();
}

void datn_set_updated( const depthn_t n )
{
	DATN_CHK_N( n, );

	DATN_LOCK();
	__updated[ n ] = 1;
	DATN_UNLOCK();
}

uint8_t datn_check_updated( const depthn_t n )
{
	uint8_t result;

	DATN_CHK_N( n, 0 );

	DATN_LOCK();
	result = __updated[ n ];
	DATN_UNLOCK();

	return ( result );
}

/** \} */

