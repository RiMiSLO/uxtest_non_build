
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datu.c
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define DATU_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datu
 *  \{
 */

typedef int __type_t;
typedef int __length_t;
typedef int __changed_t;

/**
 *  \brief Descriptor of state variable.
 */
struct DATU_DESCRY {
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

typedef const struct DATU_DESCRY *datu_descry_t;

static uint8_t __updated[ DATU_DEPTH ];
static char __str[ 100 ];

static __inline void __set_changed( const varu_t item, const depthu_t n );
static __inline void *__datn_var( const varu_t item, const depthu_t n );

/**
 *  \brief State-space.
 */
static DATU_VECTOR datu_vector[ DATU_DEPTH ];

/** \cond */

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
    { __stringify( var ), title, typeu_##type, 1, offsetof( DATU_VECTOR, var ), ( void *) string },

#define T( N, len, var, type, title, string ) \
    { __stringify( var##_##N ), title " (" __stringify( N ) ")", typeu_##type, ( len - N ), offsetof( DATU_VECTOR, var[ N ]), ( void *) string },

#define DATU_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

/**
 *  \brief Descriptor of state-space.
 */
static const struct DATU_DESCRY datu_descry[] = {
#include "datu-dict.h"
    { "none", "none", typeu_COUNT, 0, 0, strn_byte },
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T

#define DATU_THREAD( thread ) \
    static __changed_t datu_changed_##thread[ varu_COUNT ]; \
    static __changed_t datu_mask_##thread[ varu_COUNT ];

#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread ) datu_changed_##thread,
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of datn changes flags of variables.
 */
static __changed_t *datu_changed[] = {
#include "datu-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread ) datu_mask_##thread,
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of watching masks.
 */
static __changed_t *datu_mask[] = {
#include "datu-dict.h"

#ifdef _MSC_VER
	NULL

#endif
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) __stringify( T ),
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

static const char *datu_type_string[] = {
#include "datu-dict.h"
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) typedef char *( *stru_##T##_t )( char *, T, int * );
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

/**
 *  \brief value-to-string functions prototypes.
 */
#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
char *stru_##T( char *s, T value, int *col ) \
{ \
	( void ) col; \
	sprintf( s, format, value ); \
	return ( s ); \
}
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

static __inline void *__datu_var( const varu_t item, const depthu_t n )
{
    return ( void * )(( size_t )( datu_vector + n ) + datu_descry[ item ].offset );
}

static __inline int __datu_filter( const char *name )
{

#ifdef DATU_PROT
    return ((( *ini_datn_prot_filter      == '\0' ) || ( strstr( name, ini_datn_prot_filter      ) != 0 ))
         && (( *ini_datn_prot_filter_skip == '\0' ) || ( strstr( name, ini_datn_prot_filter_skip ) == 0 ))
	);

#else
	return ( 0 );

#endif
}

static __inline void __datu_print( const char *name, const depthu_t n, const char *value )
{

#ifdef DATU_PROT
	printf( __CRLF__"\t" );
	printf( clock_stamp( NULL, __str ));
	printf( "   %-22s[ %d ]  %s", name, n, value );

#endif
}

#define DATU_CHK_THREAD( thread, R ) do { \
    if ( thread >= peeku_COUNT ) { \
		assert_fail( "invalid thread index (%d)", thread ); \
		return R; \
	} \
} while ( 0 )

#define DATU_CHK_VAR( item, op, R ) do { \
    if ( item >= varu_COUNT ) { \
		assert_fail( "attempt to %s var with invalid index (%d)", op, item ); \
		return R; \
	} \
} while ( 0 )

#define DATU_CHK_TYPE( item, T, op, R ) do { \
    if ( datu_descry[ item ].type != typeu_##T ) { \
		assert_fail( "attempt to %s \"%s\" (%d) as %s while it's %s" \
		, op \
        , datu_descry[ item ].name \
		, item \
        , datu_type_string[ typeu_##T ] \
        , datu_type_string[ datu_descry[ item ].type ] \
		); \
		return R; \
	} \
} while ( 0 )

#define DATU_CHK_N( n, R ) do { \
    if ( n >= DATU_DEPTH ) { \
		assert_fail( "invalid state-space index (%d)", n ); \
		return R; \
	} \
} while ( 0 )

#define DATU_LOCK() do { \
    if ( datu_lock_callback ) datu_lock_callback(); \
} while ( 0 )

#define DATU_UNLOCK() do { \
    if ( datu_unlock_callback ) datu_unlock_callback(); \
} while ( 0 )

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
T datu_get_##T( const varu_t item, const depthu_t n ) \
{ \
	T result; \
    DATU_CHK_N( n, 0 ); \
    DATU_CHK_VAR( item, "get", 0 ); \
    DATU_CHK_TYPE( item, T, "get", 0 ); \
    DATU_LOCK(); \
    result = *(( T *) __datu_var( item, n )); \
    DATU_UNLOCK(); \
	return ( result ); \
} \
\
char *datu_get_string_##T( char *s, const varu_t item, const depthu_t n ) \
{ \
	char *result; \
	int col; \
    DATU_CHK_N( n, NULL ); \
    DATU_CHK_VAR( item, "get", NULL ); \
    DATU_CHK_TYPE( item, T, "get", NULL ); \
    DATU_LOCK(); \
    result = (( stru_##T##_t ) datu_descry[ item ].string )( s, *(( T *) __datu_var( item, n )), &col ); \
    DATU_UNLOCK(); \
	return ( result ); \
} \
\
void datu_set_##T( const varn_t item, T value, const depthu_t n ) \
{ \
	T *var; \
	int col; \
    DATU_CHK_N( n, ); \
    DATU_CHK_VAR( item, "set", ); \
    DATU_CHK_TYPE( item, T, "set", ); \
    var = ( T *) __datu_var( item, n ); \
    DATU_LOCK(); \
	if ( *var != value ) { \
		*var = value; \
		__set_changed( item, n ); \
        if ( ini_datn_prot ) { \
            const char *name = datu_descry[ item ].name; \
            if ( __datu_filter( name ))	{ \
                __datu_print( name, n, (( stru_##T##_t ) datu_descry[ item ].string )( __str, *var, &col )); \
			} \
		} \
	} \
    DATU_UNLOCK(); \
}
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

char *datu_get_string( char *s, const varu_t item, const depthu_t n )
{
    switch ( datu_descry[ item ].type ) {

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
    case typeu_##T: \
        return datu_get_string_##T( s, item, n );

#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

	default:
		return "?";
	}
}

void datu_set_string( const varu_t item, const char *s, const depthu_t n )
{
	char *end;

    switch ( datu_descry[ item ].type ) {

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
    case typeu_##T: \
		do { \
			T val = ( T ) strto( s, &end ); \
			if ( *end == '\0' ) { \
                datu_set_##T( item, val, n ); \
			} else { \
				assert_fail( "attempt to set invalid \"%s\" %s value (%s)" \
                , datu_descry[ item ].name \
                , datu_type_string[ typeu_##T ] \
				, s \
				); \
			} \
		} while ( 0 ); \
		return;

#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
	}
}

void datu_set( const varu_t item, void *value, const depthu_t n )
{
    switch ( datu_descry[ item ].type ) {

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
    case typeu_##T: \
        datu_set_##T( item, *( T *) value, n ); \
		break;

#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
	}
}

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
    if ( datu_vector[ n ].var != datu->var ) { \
        datu_vector[ n ].var = datu->var; \
        __set_changed( varu_##var, n ); \
	}

#define T( N, var, type, title, string ) \
    if ( datu_vector[ n ].var[ N ] != datu->var[ N ]) { \
        datu_vector[ n ].var[ N ] = datu->var[ N ]; \
        __set_changed( varu_##var##_##N, n ); \
	}

#define DATU_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datu_set_vector( const DATU_VECTOR *datu, const depthu_t n )
{
    DATU_CHK_N( n, 0 );

    DATU_LOCK();

#ifdef    LINK_UT_DATU
#define __LINK_UT_DATU__
#undef    LINK_UT_DATU
#endif

#include "datu-dict.h"

#ifdef __LINK_UT_DATU__
#define  LINK_UT_DATU
#undef __LINK_UT_DATU__
#endif

    DATU_UNLOCK();

	return ( 1 );
}

int datu_set_vector__ut( const DATU_VECTOR *datu, const depthu_t n )
{
    DATU_CHK_N( n, 0 );

    DATU_LOCK();

#include "datu-dict.h"

    DATU_UNLOCK();

	return ( 1 );
}

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
    if ( datu1->var != datu2->var ) return ( 0 );

#define T( N, var, type, title, string ) \
    if ( datu1->var[ N ] != datu2->var[ N ]) return ( 0 );

#define DATU_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, var, type, title, string )

int datu_cmp_vector( const DATU_VECTOR *datu1, const DATU_VECTOR *datu2 )
{

#ifdef    LINK_UT_DATU
#define __LINK_UT_DATU__
#undef    LINK_UT_DATU
#endif

#include "datu-dict.h"

#ifdef __LINK_UT_DATU__
#define  LINK_UT_DATU
#undef __LINK_UT_DATU__
#endif

	return ( 1 );
}

int datu_cmp_vector__ut( const DATU_VECTOR *datu1, const DATU_VECTOR *datu2 )
{

#include "datu-dict.h"

	return ( 1 );
}

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T

/** \endcond */

DATU_VECTOR *datu_get_vector( const depthu_t n )
{
    DATU_CHK_N( n, NULL );

    return ( datu_vector + n );
}

static __inline void __set_changed( const varu_t item, const depthu_t n )
{
    threadu_t i;
	__changed_t mask = 1UL << n;

    for ( i = 0; i < peeku_COUNT; i++ ) {
        if ( datu_mask[ i ][ item ]) datu_changed[ i ][ item ] |= mask;
	}
}

void datu_set_changed( const varu_t item, const depthu_t n )
{
    DATU_CHK_N( n, );
    DATU_CHK_VAR( item, "set change of", );
    DATU_LOCK();
	__set_changed( item, n );
    DATU_UNLOCK();
}

void datu_set_all_changed( const threadu_t thread, const depthu_t n )
{
    varu_t i;
	__changed_t mask;

    DATU_CHK_THREAD( thread, );
    mask = ( n < DATU_DEPTH ) ? 1UL << n : ( __changed_t ) DATU_MASK;
    DATU_LOCK();
    for ( i = 0; i < varu_COUNT; i++ ) {
        if ( datu_mask[ thread ][ i ]) datu_changed[ thread ][ i ] |= mask;
	}
    DATU_UNLOCK();
}

static int __get_changed( const threadu_t thread, const varu_t item, const depthu_t n )
{
    return (( datu_changed[ thread ][ item ] & ( 1UL << n )) != 0 );
}

int datu_get_changed_wo_lock( const threadu_t thread, const varu_t item, const depthu_t n )
{
	int result;

    DATU_CHK_THREAD( thread, 0 );
    DATU_CHK_N( n, 0 );
    DATU_CHK_VAR( item, "get change of", 0 );
	result = __get_changed( thread, item, n );

	return ( result );
}

int datu_get_changed( const threadu_t thread, const varu_t item, const depthu_t n )
{
	int result;

    DATU_CHK_THREAD( thread, 0 );
    DATU_CHK_N( n, 0 );
    DATU_CHK_VAR( item, "get change of", 0 );
    DATU_LOCK();
	result = __get_changed( thread, item, n );
    DATU_UNLOCK();

	return ( result );
}

int datu_get_changed_any( const threadu_t thread, const depthu_t n )
{
    varu_t i;
	__changed_t mask;

    DATU_CHK_THREAD( thread, 0 );
    DATU_CHK_N( n, 0 );
	mask = 1UL << n;
    DATU_LOCK();
    for ( i = 0; i < varu_COUNT; i++ ) {
        if ( datu_changed[ thread ][ i ] & mask ) {
            DATU_UNLOCK();

			return ( 1 );
		}
	}
    DATU_UNLOCK();

	return ( 0 );
}

void datu_watch( const threadu_t thread, const varu_t *vars )
{
    DATU_CHK_THREAD( thread, );
    DATU_LOCK();
    while ( *vars < varu_COUNT ) datu_mask[ thread ][ *vars++ ] = 1;
    DATU_UNLOCK();
}

void datu_watch_array( const threadu_t thread, const varu_t *vars )
{
    DATU_CHK_THREAD( thread, );
    DATU_LOCK();
    while ( *vars < varu_COUNT ) {
        varu_t i;

        for ( i = *vars + datu_descry[ *vars ].len - 1; i >= *vars; i-- ) {
            datu_mask[ thread ][ i ] = 1;
		}
		++vars;
	}
    DATU_UNLOCK();
}

void datu_clear_watch( const thread_t thread )
{
    varu_t i;

    DATU_CHK_THREAD( thread, );
    DATU_LOCK();
    for ( i = 0; i < varu_COUNT; i++ ) {
        datu_mask[ thread ][ i ] = 0;
        datu_changed[ thread ][ i ] = 0;
	}
    DATU_UNLOCK();
}

void datu_reset( const threadu_t thread, const varu_t item, const depthu_t n )
{
    DATU_CHK_THREAD( thread, );
    DATU_CHK_N( n, );
    DATU_CHK_VAR( item, "set change of", );
    DATU_LOCK();
    datu_changed[ thread ][ item ] &= ~( 1UL << n );
    DATU_UNLOCK();
}

void datu_reset_all( const threadu_t thread, const depthu_t n )
{
    varu_t i;
	__changed_t mask;

    DATU_CHK_THREAD( thread, );
    DATU_CHK_N( n, );
	mask = ~( 1UL << n );
    DATU_LOCK();
    for ( i = 0; i < varu_COUNT; i++ ) datu_changed[ thread ][ i ] &= mask;
    DATU_UNLOCK();
}

varn_t datu_atovar( const char *s, const varu_t from, const varu_t to )
{
    varu_t i;

	for ( i = from; i <= to; i++ ) {
        if ( strcmp( datu_descry[ i ].name, s ) == 0 ) return ( i );
	}
    return ( varu_COUNT );
}

char *datu_vartoa( const varu_t item )
{
    DATU_CHK_VAR( item, "get name of", "" );

    return (( char *) datu_descry[ item ].name );
}

char *datu_get_type( const varu_t item )
{
    DATU_CHK_VAR( item, "get type of", "" );

    return (( char *) datu_type_string[ datu_descry[ item ].type ]);
}

char *datu_get_title( const varu_t item )
{
    DATU_CHK_VAR( item, "get title of", "" );

    return (( char *) datu_descry[ item ].title );
}

void datu_lock( void )
{
    DATU_LOCK();
}

void datu_unlock( void )
{
    DATU_UNLOCK();
}

void datu_clear_updated( const depthu_t n )
{
    DATU_CHK_N( n, );

    DATU_LOCK();
	__updated[ n ] = 0;
    DATU_UNLOCK();
}

void datu_set_updated( const depthu_t n )
{
    DATU_CHK_N( n, );

    DATU_LOCK();
	__updated[ n ] = 1;
    DATU_UNLOCK();
}

uint8_t datu_check_updated( const depthu_t n )
{
	uint8_t result;

    DATU_CHK_N( n, 0 );

    DATU_LOCK();
	result = __updated[ n ];
    DATU_UNLOCK();

	return ( result );
}

/** \} */

