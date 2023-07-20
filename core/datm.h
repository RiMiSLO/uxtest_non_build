
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datm.h
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATM_H
#define DATM_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATM_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup datm Array of application state-space representation
 *  \ingroup core
 *
 *  Max length of array (DATM_DEPTH) can not exceed 32 without increasing the size of __changed_t.
 *  \{
 */

/**
 *  \brief Length of array of state-space.
 *  \hideinitializer
 */
#define DATM_DEPTH  24

#define DATM_MASK   0xFF

/**
 *  \brief Link protocol display code.
 *  \hideinitializer
 */
#define DATM_PROT

typedef int threadm_t;
typedef int varm_t;
typedef unsigned char depthm_t;

/** \cond */

#define DATM_THREAD( thread ) peekm_##thread,
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

/**
 *  \brief Identifiers of watching threads.
 */
enum {
#include "datm-dict.h"
	peekm_COUNT
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) typem_##T,
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

/**
 *  \brief Data types identifiers.
 */
enum {
#include "datm-dict.h"
	typem_COUNT
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
	varm_##var,

#define T0( N, var, type, title, string ) \
	varm_##var, \
	varm_##var##_0 = varm_##var,

#define T( N, var, type, title, string ) \
	varm_##var##_##N,

#define DATM_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T0, T, var, type, title, string )

/**
 *  \brief Variables identifiers.
 */
enum {
#include "datm-dict.h"
	varm_COUNT
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T0
#undef T

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) type var;
#define DATM_ARRAY( len, var, type, title, string ) type var[ len ];

/**
 *  \brief Structure of state-space.
 */
typedef struct {
#include "datm-dict.h"
} DATM_VECTOR;

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

/** \endcond */

/**
 *  \brief Set data changes flag.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datm_set_changed( const varm_t item, const depthm_t n );

/**
 *  \brief Set data changes flags for all variables of watching thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datm_set_all_changed( const threadm_t thread, const depthm_t n );

/**
 *  \brief Get data changes flag of variable (without data lock).
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datm_get_changed_wo_lock( const threadm_t thread, const varm_t item, const depthm_t n );

/**
 *  \brief Get data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datm_get_changed( const threadm_t thread, const varm_t item, const depthm_t n );

/**
 *  \brief At least one variable from watching list of thread has been changed.
 *  \param thread Watching thread.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datm_get_changed_any( const threadm_t thread, const depthm_t n );

/**
 *  \brief Add variables to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Variables list (varm_COUNT is list terminator).
 *  Only first variable of array will be added if array in list.
 */
_EXT_C void datm_watch( const threadm_t thread, const varm_t *vars );

/**
 *  \brief Add arrays to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Arrays list (varm_COUNT is list terminator).
 *  Variable and the rest of array will be added if variable in list.
 */
_EXT_C void datm_watch_array( const threadm_t thread, const varm_t *vars );

/**
 *  \brief Clear watching list of thread.
 *  \param thread Watching thread.
 */
_EXT_C void datm_clear_watch( const threadm_t thread );

/**
 *  \brief Reset data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datm_reset( const threadm_t thread, const varm_t item, const depthm_t n );

/**
 *  \brief Reset data changes flags of all variables of thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datm_reset_all( const threadm_t thread, const depthm_t n );

/**
 *  \brief Search variable by name.
 *  \param s Variable name.
 *  \param from From variable.
 *  \param to Up to variable.
 */
_EXT_C varm_t datm_atovar( const char *s, const varm_t from, const varm_t to );

/**
 *  \brief Get variable name.
 *  \param item Variable.
 *  \return Name.
 */
_EXT_C char *datm_vartoa( const varm_t item );

/**
 *  \brief Set state-space from snapshot.
 *  \param datm - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datm_set_vector( const DATM_VECTOR *datm, const depthm_t n );

/**
 *  \brief Set state-space from snapshot (test variables only _ut_*).
 *  \param datm - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datm_set_vector__ut( const DATM_VECTOR *datm, const depthm_t n );

/**
 *  \brief Get state-space data pointer.
 *  \param n State-space index.
 *  \return Data pointer.
 */
_EXT_C DATM_VECTOR *datm_get_vector( const depthm_t n );

/**
 *  \brief Compare vectors.
 *  \param datm1 - 1st state-space.
 *  \param datm2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datm_cmp_vector( const DATM_VECTOR *datm1, const DATM_VECTOR *datm2 );

/**
 *  \brief Compare vectors (test variables only _ut_*).
 *  \param datm1 - 1st state-space.
 *  \param datm2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datm_cmp_vector__ut( const DATM_VECTOR *datm1, const DATM_VECTOR *datm2 );

/** \cond */

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
	_EXT_C char *strm_##T( char *s, T value, int *col );
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto ) \
	_EXT_C T datm_get_##T( const varm_t item, const depthm_t n ); \
	_EXT_C char *datm_get_string_##T( char *s, const varm_t item, const depthm_t n ); \
	_EXT_C void datm_set_##T( const varm_t item, T value, const depthm_t n );
#define DATM_VAR( var, type, title, string )
#define DATM_ARRAY( len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY

/** \endcond */

/**
 *  \brief Set value of variable of any type.
 *  \param item Variable.
 *  \param value Value pointer.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datm_set( const varm_t item, void *value, const depthm_t n );

/**
 *  \brief Set taken from string value of variable.
 *  \param item Variable.
 *  \param s String.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datm_set_string( const varm_t item, const char *s, const depthm_t n );

/**
 *  \brief Get value of variable as string.
 *  \param s String buffer.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C char *datm_get_string( char *s, const varm_t item, const depthm_t n );

/**
 *  \brief Get variable type as string.
 *  \param item Variable.
 */
_EXT_C char *datm_get_type( const varm_t item );

/**
 *  \brief Get title of variable.
 *  \param item Variable.
 */
_EXT_C char *datm_get_title( const varm_t item );

/**
 *  \brief Lock data access (callback).
 */
_EXT void ( *datm_lock_callback )( void );

/**
 *  \brief Unlock data access (callback).
 */
_EXT void ( *datm_unlock_callback )( void );

/**
 *  \brief Lock data access.
 */
_EXT_C void datm_lock( void );

/**
 *  \brief Unlock data access.
 */
_EXT_C void datm_unlock( void );

_EXT_C void datm_clear_updated( const depthm_t n );
_EXT_C void datm_set_updated( const depthm_t n );
_EXT_C uint8_t datm_check_updated( const depthm_t n );

/** \} */

#include "h-end.h"

#endif /* DATM_H */

