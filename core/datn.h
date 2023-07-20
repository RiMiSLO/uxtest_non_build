
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datn.h
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATN_H
#define DATN_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATN_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup datn Array of application state-space representation
 *  \ingroup core
 *
 *  Max length of array (DATN_DEPTH) can not exceed 32 without increasing the size of __changed_t.
 *  \{
 */

/**
 *  \brief Length of array of state-space.
 *  \hideinitializer
 */
#define DATN_DEPTH  32

#define DATN_MASK   0xFF

/**
 *  \brief Link protocol display code.
 *  \hideinitializer
 */
#define DATN_PROT

typedef int threadn_t;
typedef int varn_t;
typedef unsigned char depthn_t;

/** \cond */

#define DATN_THREAD( thread ) peekn_##thread,
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

/**
 *  \brief Identifiers of watching threads.
 */
enum {
#include "datn-dict.h"
	peekn_COUNT
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) typen_##T,
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

/**
 *  \brief Data types identifiers.
 */
enum {
#include "datn-dict.h"
	typen_COUNT
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
	varn_##var,

#define T0( N, var, type, title, string ) \
	varn_##var, \
	varn_##var##_0 = varn_##var,

#define T( N, var, type, title, string ) \
	varn_##var##_##N,

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T0, T, var, type, title, string )

/**
 *  \brief Variables identifiers.
 */
enum {
#include "datn-dict.h"
	varn_COUNT
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T0
#undef T

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) type var;
#define DATN_ARRAY( len, var, type, title, string ) type var[ len ];

/**
 *  \brief Structure of state-space.
 */
typedef struct {
#include "datn-dict.h"
} DATN_VECTOR;

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

/** \endcond */

/**
 *  \brief Set data changes flag.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datn_set_changed( const varn_t item, const depthn_t n );

/**
 *  \brief Set data changes flags for all variables of watching thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datn_set_all_changed( const threadn_t thread, const depthn_t n );

/**
 *  \brief Get data changes flag of variable (without data lock).
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datn_get_changed_wo_lock( const threadn_t thread, const varn_t item, const depthn_t n );

/**
 *  \brief Get data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datn_get_changed( const threadn_t thread, const varn_t item, const depthn_t n );

/**
 *  \brief At least one variable from watching list of thread has been changed.
 *  \param thread Watching thread.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datn_get_changed_any( const threadn_t thread, const depthn_t n );

/**
 *  \brief Add variables to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Variables list (varn_COUNT is list terminator).
 *  Only first variable of array will be added if array in list.
 */
_EXT_C void datn_watch( const threadn_t thread, const varn_t *vars );

/**
 *  \brief Add arrays to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Arrays list (varn_COUNT is list terminator).
 *  Variable and the rest of array will be added if variable in list.
 */
_EXT_C void datn_watch_array( const threadn_t thread, const varn_t *vars );

/**
 *  \brief Clear watching list of thread.
 *  \param thread Watching thread.
 */
_EXT_C void datn_clear_watch( const threadn_t thread );

/**
 *  \brief Reset data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datn_reset( const threadn_t thread, const varn_t item, const depthn_t n );

/**
 *  \brief Reset data changes flags of all variables of thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datn_reset_all( const threadn_t thread, const depthn_t n );

/**
 *  \brief Search variable by name.
 *  \param s Variable name.
 *  \param from From variable.
 *  \param to Up to variable.
 */
_EXT_C varn_t datn_atovar( const char *s, const varn_t from, const varn_t to );

/**
 *  \brief Get variable name.
 *  \param item Variable.
 *  \return Name.
 */
_EXT_C char *datn_vartoa( const varn_t item );

/**
 *  \brief Set state-space from snapshot.
 *  \param datn - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datn_set_vector( const DATN_VECTOR *datn, const depthn_t n );

/**
 *  \brief Set state-space from snapshot (test variables only _ut_*).
 *  \param datn - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datn_set_vector__ut( const DATN_VECTOR *datn, const depthn_t n );

/**
 *  \brief Get state-space data pointer.
 *  \param n State-space index.
 *  \return Data pointer.
 */
_EXT_C DATN_VECTOR *datn_get_vector( const depthn_t n );

/**
 *  \brief Compare vectors.
 *  \param datn1 - 1st state-space.
 *  \param datn2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datn_cmp_vector( const DATN_VECTOR *datn1, const DATN_VECTOR *datn2 );

/**
 *  \brief Compare vectors (test variables only _ut_*).
 *  \param datn1 - 1st state-space.
 *  \param datn2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datn_cmp_vector__ut( const DATN_VECTOR *datn1, const DATN_VECTOR *datn2 );

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
	_EXT_C char *strn_##T( char *s, T value, int *col );
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto ) \
	_EXT_C T datn_get_##T( const varn_t item, const depthn_t n ); \
	_EXT_C char *datn_get_string_##T( char *s, const varn_t item, const depthn_t n ); \
	_EXT_C void datn_set_##T( const varn_t item, T value, const depthn_t n );
#define DATN_VAR( var, type, title, string )
#define DATN_ARRAY( len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

/** \endcond */

/**
 *  \brief Set value of variable of any type.
 *  \param item Variable.
 *  \param value Value pointer.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datn_set( const varn_t item, void *value, const depthn_t n );

/**
 *  \brief Set taken from string value of variable.
 *  \param item Variable.
 *  \param s String.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datn_set_string( const varn_t item, const char *s, const depthn_t n );

/**
 *  \brief Get value of variable as string.
 *  \param s String buffer.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C char *datn_get_string( char *s, const varn_t item, const depthn_t n );

/**
 *  \brief Get variable type as string.
 *  \param item Variable.
 */
_EXT_C char *datn_get_type( const varn_t item );

/**
 *  \brief Get title of variable.
 *  \param item Variable.
 */
_EXT_C char *datn_get_title( const varn_t item );

/**
 *  \brief Lock data access (callback).
 */
_EXT void ( *datn_lock_callback )( void );

/**
 *  \brief Unlock data access (callback).
 */
_EXT void ( *datn_unlock_callback )( void );

/**
 *  \brief Lock data access.
 */
_EXT_C void datn_lock( void );

/**
 *  \brief Unlock data access.
 */
_EXT_C void datn_unlock( void );

_EXT_C void datn_clear_updated( const depthn_t n );
_EXT_C void datn_set_updated( const depthn_t n );
_EXT_C uint8_t datn_check_updated( const depthn_t n );

/** \} */

#include "h-end.h"

#endif /* DATN_H */

