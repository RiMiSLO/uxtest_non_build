
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  DATMCP.h
 *  \brief  Array of application state-space representation.
 *  \author  Nikita Lagutin
 *  \copyright  See the LICENSE file.
 */

#ifndef DATMCP_H
#define DATMCP_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATMCP_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup DATMCP Array of application state-space representation
 *  \ingroup core
 *
 *  Max length of array (DATMCP_DEPTH) can not exceed 32 without increasing the size of __changed_t.
 *  \{
 */

/**
 *  \brief Length of array of state-space.
 *  \hideinitializer
 */
#define DATMCP_DEPTH  24

#define DATMCP_MASK   0xFF

/**
 *  \brief Link protocol display code.
 *  \hideinitializer
 */
#define DATMCP_PROT

typedef int threadmcp_t;
typedef int varmcp_t;
typedef unsigned char depthmcp_t;

/** \cond */

#define DATMCP_THREAD( thread ) peekmcp_##thread,
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

/**
 *  \brief Identifiers of watching threads.
 */
enum {
#include "datmcp-dict.h"
    peekmcp_COUNT
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) typemcp_##T,
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

/**
 *  \brief Data types identifiers.
 */
enum {
#include "datmcp-dict.h"
    typemcp_COUNT
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
    varmcp_##var,

#define T0( N, var, type, title, string ) \
    varmcp_##var, \
    varmcp_##var##_0 = varmcp_##var,

#define T( N, var, type, title, string ) \
    varmcp_##var##_##N,

#define DATMCP_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T0, T, var, type, title, string )

/**
 *  \brief Variables identifiers.
 */
enum {
#include "datmcp-dict.h"
    varmcp_COUNT
};

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
#undef T0
#undef T

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) type var;
#define DATMCP_ARRAY( len, var, type, title, string ) type var[ len ];

/**
 *  \brief Structure of state-space.
 */
typedef struct {
#include "datmcp-dict.h"
} DATMCP_VECTOR;

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

/** \endcond */

/**
 *  \brief Set data changes flag.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datmcp_set_changed( const varmcp_t item, const depthmcp_t n );

/**
 *  \brief Set data changes flags for all variables of watching thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datmcp_set_all_changed( const threadmcp_t thread, const depthmcp_t n );

/**
 *  \brief Get data changes flag of variable (without data lock).
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datmcp_get_changed_wo_lock( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n );

/**
 *  \brief Get data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datmcp_get_changed( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n );

/**
 *  \brief At least one variable from watching list of thread has been changed.
 *  \param thread Watching thread.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datmcp_get_changed_any( const threadmcp_t thread, const depthmcp_t n );

/**
 *  \brief Add variables to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Variables list (varm_COUNT is list terminator).
 *  Only first variable of array will be added if array in list.
 */
_EXT_C void datmcp_watch( const threadmcp_t thread, const varmcp_t *vars );

/**
 *  \brief Add arrays to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Arrays list (varm_COUNT is list terminator).
 *  Variable and the rest of array will be added if variable in list.
 */
_EXT_C void datmcp_watch_array( const threadmcp_t thread, const varmcp_t *vars );

/**
 *  \brief Clear watching list of thread.
 *  \param thread Watching thread.
 */
_EXT_C void datmcp_clear_watch( const threadmcp_t thread );

/**
 *  \brief Reset data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datmcp_reset( const threadmcp_t thread, const varmcp_t item, const depthmcp_t n );

/**
 *  \brief Reset data changes flags of all variables of thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datmcp_reset_all( const threadmcp_t thread, const depthmcp_t n );

/**
 *  \brief Search variable by name.
 *  \param s Variable name.
 *  \param from From variable.
 *  \param to Up to variable.
 */
_EXT_C varmcp_t datmcp_atovar( const char *s, const varmcp_t from, const varmcp_t to );

/**
 *  \brief Get variable name.
 *  \param item Variable.
 *  \return Name.
 */
_EXT_C char *datmcp_vartoa( const varmcp_t item );

/**
 *  \brief Set state-space from snapshot.
 *  \param DATMCP - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datmcp_set_vector( const DATMCP_VECTOR *DATMCP, const depthmcp_t n );

/**
 *  \brief Set state-space from snapshot (test variables only _ut_*).
 *  \param DATMCP - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datmcp_set_vector__ut( const DATMCP_VECTOR *DATMCP, const depthmcp_t n );

/**
 *  \brief Get state-space data pointer.
 *  \param n State-space index.
 *  \return Data pointer.
 */
_EXT_C DATMCP_VECTOR *datmcp_get_vector( const depthmcp_t n );

/**
 *  \brief Compare vectors.
 *  \param DATMCP1 - 1st state-space.
 *  \param DATMCP2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datmcp_cmp_vector( const DATMCP_VECTOR *datmcp1, const DATMCP_VECTOR *datmcp2 );

/**
 *  \brief Compare vectors (test variables only _ut_*).
 *  \param DATMCP1 - 1st state-space.
 *  \param DATMCP2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datmcp_cmp_vector__ut( const DATMCP_VECTOR *datmcp1, const DATMCP_VECTOR *datmcp2 );

/** \cond */

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
    _EXT_C char *strmcp_##T( char *s, T value, int *col );
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto ) \
    _EXT_C T datmcp_get_##T( const varmcp_t item, const depthmcp_t n ); \
    _EXT_C char *datmcp_get_string_##T( char *s, const varmcp_t item, const depthmcp_t n ); \
    _EXT_C void datmcp_set_##T( const varmcp_t item, T value, const depthmcp_t n );
#define DATMCP_VAR( var, type, title, string )
#define DATMCP_ARRAY( len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY

/** \endcond */

/**
 *  \brief Set value of variable of any type.
 *  \param item Variable.
 *  \param value Value pointer.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datmcp_set( const varmcp_t item, void *value, const depthmcp_t n );

/**
 *  \brief Set taken from string value of variable.
 *  \param item Variable.
 *  \param s String.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datmcp_set_string( const varmcp_t item, const char *s, const depthmcp_t n );

/**
 *  \brief Get value of variable as string.
 *  \param s String buffer.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C char *datmcp_get_string( char *s, const varmcp_t item, const depthmcp_t n );

/**
 *  \brief Get variable type as string.
 *  \param item Variable.
 */
_EXT_C char *datmcp_get_type( const varmcp_t item );

/**
 *  \brief Get title of variable.
 *  \param item Variable.
 */
_EXT_C char *datmcp_get_title( const varmcp_t item );

/**
 *  \brief Lock data access (callback).
 */
_EXT void ( *datmcp_lock_callback )( void );

/**
 *  \brief Unlock data access (callback).
 */
_EXT void ( *datmcp_unlock_callback )( void );

/**
 *  \brief Lock data access.
 */
_EXT_C void datmcp_lock( void );

/**
 *  \brief Unlock data access.
 */
_EXT_C void datmcp_unlock( void );

_EXT_C void datmcp_clear_updated( const depthmcp_t n );
_EXT_C void datmcp_set_updated( const depthmcp_t n );
_EXT_C uint8_t datmcp_check_updated( const depthmcp_t n );

/** \} */

#include "h-end.h"

#endif /* DATMCP_H */

