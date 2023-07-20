
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datmcp.c
 *  \brief  Array of application state-space representation.
 *  \author  Nikita Lagutin
 *  \copyright  See the LICENSE file.
 */

#define DATMCP_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datmcp
 *  \{
 */

typedef int __type_t;
typedef int __length_t;
typedef int __changed_t;

/**
 *  \brief Descriptor of state variable.
 */
struct datmcp_descry {
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

typedef const struct datmcp_descry *datmcp_descry_t;

static uint8_t __updated[ DATMCP_DEPTH ];
static char __str[ 100 ];

static __inline void __set_changed( const varmcp_t item, const depthmcp_t n );
static __inline void *__datmcp_var( const varmcp_t item, const depthmcp_t n );

/**
 *  \brief State-space.
 */
static DATMCP_VECTOR datmcp_vector[ DATMCP_DEPTH ];

/** \cond */

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
    { __stringify( var ), title, typemcp_##type, 1, offsetof( DATMCP_VECTOR, var ), ( void *) string },

#define T( N, len, var, type, title, string ) \
    { __stringify( var##_##N ), title " (" __stringify( N ) ")", typemcp_##type, ( len - N ), offsetof( DATMCP_VECTOR, var[ N ]), ( void *) string },

#define DATMCP_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, len, var, type, title, string )

/**
 *  \brief Descriptor of state-space.
 */
static const struct datmcp_descry datmcp_descry[] = {
#include "datmcp-dict.h"
    { "none", "none", typemcp_COUNT, 0, 0, strmcp_byte },
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
#undef T

#define DATMCP_THREAD( thread ) \
    static __changed_t datmcp_changed_##thread[ varmcp_COUNT ]; \
    static __changed_t datmcp_mask_##thread[ varmcp_COUNT ];

#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread ) datmcp_changed_##thread,
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of DATMCP changes flags of variables.
 */
static __changed_t *datmcp_changed[] = {
#include "datmcp-dict.h"

#ifdef _MSC_VER
    NULL

#endif
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread ) datmcp_mask_##thread,
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

/**
 *  \brief Matrix of watching masks.
 */
static __changed_t *datmcp_mask[] = {
#include "datmcp-dict.h"

#ifdef _MSC_VER
    NULL

#endif
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) __stringify( T ),
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

static const char *datmcp_type_string[] = {
#include "datmcp-dict.h"
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) typedef char *( *strmcp_##T##_t )( char *, T, int * );
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

/**
 *  \brief value-to-string functions prototypes.
 */
#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
char *strmcp_##T( char *s, T value, int *col ) \
{ \
    ( void ) col; \
    sprintf( s, format, value ); \
    return ( s ); \
}
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

static __inline void *__datmcp_var( const varmcp_t item, const depthmcp_t n )
{
    return ( void * )(( size_t )( datmcp_vector + n ) + datmcp_descry[ item ].offset );
}

static __inline int __datmcp_filter( const char *name )
{

#ifdef DATMCP_PROT
    return ((( *ini_datmcp_prot_filter      == '\0' ) || ( strstr( name, ini_datmcp_prot_filter      ) != 0 ))
         && (( *ini_datmcp_prot_filter_skip == '\0' ) || ( strstr( name, ini_datmcp_prot_filter_skip ) == 0 ))
    );

#else
    return ( 0 );

#endif
}

static __inline void __datmcp_print( const char *name, const depthmcp_t n, const char *value )
{

#ifdef DATMCP_PROT
    printf( __CRLF__"\t" );
    printf( clock_stamp( NULL, __str ));
    printf( "   %-22s[ %d ]  %s", name, n, value );

#endif
}

#define DATMCP_CHK_THREAD( thread, R ) do { \
    if ( thread >= peekmcp_COUNT ) { \
        assert_fail( "invalid thread index (%d)", thread ); \
        return R; \
    } \
} while ( 0 )

#define DATMCP_CHK_VAR( item, op, R ) do { \
    if ( item >= varmcp_COUNT ) { \
        assert_fail( "attempt to %s var with invalid index (%d)", op, item ); \
        return R; \
    } \
} while ( 0 )

#define DATMCP_CHK_TYPE( item, T, op, R ) do { \
    if ( datmcp_descry[ item ].type != typemcp_##T ) { \
        assert_fail( "attempt to %s \"%s\" (%d) as %s while it's %s" \
        , op \
        , datmcp_descry[ item ].name \
        , item \
        , datmcp_type_string[ typemcp_##T ] \
        , datmcp_type_string[ datmcp_descry[ item ].type ] \
        ); \
        return R; \
    } \
} while ( 0 )

#define DATMCP_CHK_N( n, R ) do { \
    if ( n >= DATMCP_DEPTH ) { \
        assert_fail( "invalid state-space index (%d)", n ); \
        return R; \
    } \
} while ( 0 )

#define DATMCP_LOCK() do { \
    if ( datmcp_lock_callback ) datmcp_lock_callback(); \
} while ( 0 )

#define DATMCP_UNLOCK() do { \
    if ( datmcp_unlock_callback ) datmcp_unlock_callback(); \
} while ( 0 )

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
T datmcp_get_##T( const varmcp_t item, const depthmcp_t n ) \
{ \
    T result; \
    DATMCP_CHK_N( n, 0 ); \
    DATMCP_CHK_VAR( item, "get", 0 ); \
    DATMCP_CHK_TYPE( item, T, "get", 0 ); \
    DATMCP_LOCK(); \
    result = *(( T *) __datmcp_var( item, n )); \
    DATMCP_UNLOCK(); \
    return ( result ); \
} \
\
char *datmcp_get_string_##T( char *s, const varmcp_t item, const depthmcp_t n ) \
{ \
    char *result; \
    int col; \
    DATMCP_CHK_N( n, NULL ); \
    DATMCP_CHK_VAR( item, "get", NULL ); \
    DATMCP_CHK_TYPE( item, T, "get", NULL ); \
    DATMCP_LOCK(); \
    result = (( strmcp_##T##_t ) datmcp_descry[ item ].string )( s, *(( T *) __datmcp_var( item, n )), &col ); \
    DATMCP_UNLOCK(); \
    return ( result ); \
} \
\
void datmcp_set_##T( const varmcp_t item, T value, const depthmcp_t n ) \
{ \
    T *var; \
    int col; \
    DATMCP_CHK_N( n, ); \
    DATMCP_CHK_VAR( item, "set", ); \
    DATMCP_CHK_TYPE( item, T, "set", ); \
    var = ( T *) __datmcp_var( item, n ); \
    DATMCP_LOCK(); \
    if ( *var != value ) { \
        *var = value; \
        __set_changed( item, n ); \
        if ( ini_datmcp_prot ) { \
            const char *name = datmcp_descry[ item ].name; \
            if ( __datmcp_filter( name ))	{ \
                __datmcp_print( name, n, (( strmcp_##T##_t ) datmcp_descry[ item ].string )( __str, *var, &col )); \
            } \
        } \
    } \
    DATMCP_UNLOCK(); \
}
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

char *datmcp_get_string( char *s, const varmcp_t item, const depthmcp_t n )
{
    switch ( datmcp_descry[ item ].type ) {

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
    case typemcp_##T: \
        return datmcp_get_string_##T( s, item, n );

#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

    default:
        return "?";
    }
}

void datmcp_set_string( const varmcp_t item, const char *s, const depthmcp_t n )
{
    char *end;

    switch ( datmcp_descry[ item ].type ) {

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
    case typemcp_##T: \
        do { \
            T val = ( T ) strto( s, &end ); \
            if ( *end == '\0' ) { \
                datmcp_set_##T( item, val, n ); \
            } else { \
                assert_fail( "attempt to set invalid \"%s\" %s value (%s)" \
                , datmcp_descry[ item ].name \
                , datmcp_type_string[ typem_##T ] \
                , s \
                ); \
            } \
        } while ( 0 ); \
        return;

#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
    }
}

void datmcp_set( const varmcp_t item, void *value, const depthmcp_t n )
{
    switch ( datmcp_descry[ item ].type ) {

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
    case typemcp_##T: \
        datmcp_set_##T( item, *( T *) value, n ); \
        break;

#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
    }
}

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
    if ( datmcp_vector[ n ].var != datmcp->var ) { \
        datmcp_vector[ n ].var = datmcp->var; \
        __set_changed( varmcp_##var, n ); \
    }

#define T( N, var, type, title, string ) \
    if ( datmcp_vector[ n ].var[ N ] != datmcp->var[ N ]) { \
        datmcp_vector[ n ].var[ N ] = datmcp->var[ N ]; \
        __set_changed( varmcp_##var##_##N, n ); \
    }

#define DATMCP_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, var, type, title, string )

int datmcp_set_vector( const DATMCP_VECTOR *datmcp, const depthmcp_t n )
{
    DATMCP_CHK_N( n, 0 );

    DATMCP_LOCK();

#ifdef    LINK_UT_DATMCP
#define __LINK_UT_DATMCP__
#undef    LINK_UT_DATMCP
#endif

#include "datmcp-dict.h"

#ifdef __LINK_UT_DATMCP__
#define  LINK_UT_DATMCP
#undef __LINK_UT_DATMCP__
#endif

    DATMCP_UNLOCK();

    return ( 1 );
}

int datmcp_set_vector__ut( const DATMCP_VECTOR *datmcp, const depthmcp_t n )
{
    DATMCP_CHK_N( n, 0 );

    DATMCP_LOCK();

#include "datmcp-dict.h"

    DATMCP_UNLOCK();

    return ( 1 );
}

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
#undef T

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
    if ( datmcp1->var != datmcp2->var ) return ( 0 );

#define T( N, var, type, title, string ) \
    if ( datmcp1->var[ N ] != datmcp2->var[ N ]) return ( 0 );

#define DATMCP_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, var, type, title, string )

int datmcp_cmp_vector( const DATMCP_VECTOR *datmcp1, const DATMCP_VECTOR *datmcp2 )
{

#ifdef    LINK_UT_DATMCP
#define __LINK_UT_DATMCP__
#undef    LINK_UT_DATMCP
#endif

#include "datmcp-dict.h"

#ifdef __LINK_UT_DATMCP__
#define  LINK_UT_DATMCP
#undef __LINK_UT_DATMCP__
#endif

    return ( 1 );
}

int datmcp_cmp_vector__ut( const DATMCP_VECTOR *datmcp1, const DATMCP_VECTOR *datmcp2 )
{

#include "datmcp-dict.h"

    return ( 1 );
}

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
#undef T

/** \endcond */

DATMCP_VECTOR *datmcp_get_vector( const depthmcp_t n )
{
    DATMCP_CHK_N( n, NULL );

    return ( datmcp_vector + n );
}

static __inline void __set_changed( const varmcp_t item, const depthmcp_t n )
{
    threadmcp_t i;
    __changed_t mask = 1UL << n;

    for ( i = 0; i < peekmcp_COUNT; i++ ) {
        if ( datmcp_mask[ i ][ item ]) datmcp_changed[ i ][ item ] |= mask;
    }
}

void datmcp_set_changed( const varmcp_t item, const depthmcp_t n )
{
    DATMCP_CHK_N( n, );
    DATMCP_CHK_VAR( item, "set change of", );
    DATMCP_LOCK();
    __set_changed( item, n );
    DATMCP_UNLOCK();
}

void datmcp_set_all_changed( const threadmcp_t thread, const depthmcp_t n )
{
    varmcp_t i;
    __changed_t mask;

    DATMCP_CHK_THREAD( thread, );
    mask = ( n < DATMCP_DEPTH ) ? 1UL << n : ( __changed_t ) datmcp_mask;
    DATMCP_LOCK();
    for ( i = 0; i < varmcp_COUNT; i++ ) {
        if ( datmcp_mask[ thread ][ i ]) datmcp_changed[ thread ][ i ] |= mask;
    }
    DATMCP_UNLOCK();
}

static int __get_changed( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n )
{
    return (( datmcp_changed[ thread ][ item ] & ( 1UL << n )) != 0 );
}

int datmcp_get_changed_wo_lock( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n )
{
    int result;

    DATMCP_CHK_THREAD( thread, 0 );
    DATMCP_CHK_N( n, 0 );
    DATMCP_CHK_VAR( item, "get change of", 0 );
    result = __get_changed( thread, item, n );

    return ( result );
}

int datmcp_get_changed( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n )
{
    int result;

    DATMCP_CHK_THREAD( thread, 0 );
    DATMCP_CHK_N( n, 0 );
    DATMCP_CHK_VAR( item, "get change of", 0 );
    DATMCP_LOCK();
    result = __get_changed( thread, item, n );
    DATMCP_UNLOCK();

    return ( result );
}

int datmcp_get_changed_any( const threadmcp_t thread, const depthmcp_t n )
{
    varmcp_t i;
    __changed_t mask;

    DATMCP_CHK_THREAD( thread, 0 );
    DATMCP_CHK_N( n, 0 );
    mask = 1UL << n;
    DATMCP_LOCK();
    for ( i = 0; i < varmcp_COUNT; i++ ) {
        if ( datmcp_changed[ thread ][ i ] & mask ) {
            DATMCP_UNLOCK();

            return ( 1 );
        }
    }
    DATMCP_UNLOCK();

    return ( 0 );
}

void datmcp_watch( const threadmcp_t thread, const varmcp_t *vars )
{
    DATMCP_CHK_THREAD( thread, );
    DATMCP_LOCK();
    while ( *vars < varmcp_COUNT ) datmcp_mask[ thread ][ *vars++ ] = 1;
    DATMCP_UNLOCK();
}

void datmcp_watch_array( const threadmcp_t thread, const varmcp_t *vars )
{
    DATMCP_CHK_THREAD( thread, );
    DATMCP_LOCK();
    while ( *vars < varmcp_COUNT ) {
        varmcp_t i;

        for ( i = *vars + datmcp_descry[ *vars ].len - 1; i >= *vars; i-- ) {
            datmcp_mask[ thread ][ i ] = 1;
        }
        ++vars;
    }
    DATMCP_UNLOCK();
}

void datmcp_clear_watch( const thread_t thread )
{
    varmcp_t i;

    DATMCP_CHK_THREAD( thread, );
    DATMCP_LOCK();
    for ( i = 0; i < varmcp_COUNT; i++ ) {
        datmcp_mask[ thread ][ i ] = 0;
        datmcp_changed[ thread ][ i ] = 0;
    }
    DATMCP_UNLOCK();
}

void datmcp_reset( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n )
{
    DATMCP_CHK_THREAD( thread, );
    DATMCP_CHK_N( n, );
    DATMCP_CHK_VAR( item, "set change of", );
    DATMCP_LOCK();
    datmcp_changed[ thread ][ item ] &= ~( 1UL << n );
    DATMCP_UNLOCK();
}

void datmcp_reset_all( const threadmcp_t thread, const depthmcp_t n )
{
    varmcp_t i;
    __changed_t mask;

    DATMCP_CHK_THREAD( thread, );
    DATMCP_CHK_N( n, );
    mask = ~( 1UL << n );
    DATMCP_LOCK();
    for ( i = 0; i < varmcp_COUNT; i++ ) datmcp_changed[ thread ][ i ] &= mask;
    DATMCP_UNLOCK();
}

varmcp_t datmcp_atovar( const char *s, const varmcp_t from, const varmcp_t to )
{
    varmcp_t i;

    for ( i = from; i <= to; i++ ) {
        if ( strcmp( datmcp_descry[ i ].name, s ) == 0 ) return ( i );
    }
    return ( varmcp_COUNT );
}

char *datmcp_vartoa( const varmcp_t item )
{
    DATMCP_CHK_VAR( item, "get name of", "" );

    return (( char *) datmcp_descry[ item ].name );
}

char *datmcp_get_type( const varmcp_t item )
{
    DATMCP_CHK_VAR( item, "get type of", "" );

    return (( char *) datmcp_type_string[ datmcp_descry[ item ].type ]);
}

char *datmcp_get_title( const varmcp_t item )
{
    DATMCP_CHK_VAR( item, "get title of", "" );

    return (( char *) datmcp_descry[ item ].title );
}

void datmcp_lock( void )
{
    DATMCP_LOCK();
}

void datmcp_unlock( void )
{
    DATMCP_UNLOCK();
}

void datmcp_clear_updated( const depthmcp_t n )
{
    DATMCP_CHK_N( n, );

    DATMCP_LOCK();
    __updated[ n ] = 0;
    DATMCP_UNLOCK();
}

void datmcp_set_updated( const depthmcp_t n )
{
    DATMCP_CHK_N( n, );

    DATMCP_LOCK();
    __updated[ n ] = 1;
    DATMCP_UNLOCK();
}

uint8_t datmcp_check_updated( const depthmcp_t n )
{
    uint8_t result;

    DATMCP_CHK_N( n, 0 );

    DATMCP_LOCK();
    result = __updated[ n ];
    DATMCP_UNLOCK();

    return ( result );
}

/** \} */
