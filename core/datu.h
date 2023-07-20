
/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  datu.h
 *  \brief  Array of application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATU_H
#define DATU_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATU_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup datn Array of application state-space representation
 *  \ingroup core
 *
 *  Max length of array (DATU_DEPTH) can not exceed 32 without increasing the size of __changed_t.
 *  \{
 */

/**
 *  \brief Length of array of state-space.
 *  \hideinitializer
 */
#define DATU_DEPTH  32

#define DATU_MASK   0xFF

/**
 *  \brief Link protocol display code.
 *  \hideinitializer
 */
#define DATU_PROT

typedef int threadu_t;
typedef int varu_t;
typedef unsigned char depthu_t;

/** \cond */

#define DATU_THREAD( thread ) peeku_##thread,
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

/**
 *  \brief Identifiers of watching threads.
 */
enum {
#include "datu-dict.h"
    peeku_COUNT
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) typeu_##T,
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

/**
 *  \brief Data types identifiers.
 */
enum {
#include "datu-dict.h"
    typeu_COUNT
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
    varu_##var,

#define T0( N, var, type, title, string ) \
    varu_##var, \
    varu_##var##_0 = varu_##var,

#define T( N, var, type, title, string ) \
    varu_##var##_##N,

#define DATU_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T0, T, var, type, title, string )

/**
 *  \brief Variables identifiers.
 */
enum {
#include "datu-dict.h"
    varu_COUNT
};

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T0
#undef T

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) type var;
#define DATU_ARRAY( len, var, type, title, string ) type var[ len ];

/**
 *  \brief Structure of state-space.
 */
typedef struct {
#include "datu-dict.h"
} DATU_VECTOR;

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

/** \endcond */

/**
 *  \brief Set data changes flag.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datu_set_changed( const varu_t item, const depthu_t n );

/**
 *  \brief Set data changes flags for all variables of watching thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datu_set_all_changed( const threadu_t thread, const depthu_t n );

/**
 *  \brief Get data changes flag of variable (without data lock).
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datu_get_changed_wo_lock( const threadu_t thread, const varu_t item, const depthu_t n );

/**
 *  \brief Get data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datu_get_changed( const threadu_t thread, const varu_t item, const depthu_t n );

/**
 *  \brief At least one variable from watching list of thread has been changed.
 *  \param thread Watching thread.
 *  \param n State-space index.
 *  \return Change flag.
 */
_EXT_C int datu_get_changed_any( const threadu_t thread, const depthu_t n );

/**
 *  \brief Add variables to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Variables list (varn_COUNT is list terminator).
 *  Only first variable of array will be added if array in list.
 */
_EXT_C void datu_watch( const threadu_t thread, const varu_t *vars );

/**
 *  \brief Add arrays to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Arrays list (varn_COUNT is list terminator).
 *  Variable and the rest of array will be added if variable in list.
 */
_EXT_C void datu_watch_array( const threadu_t thread, const varu_t *vars );

/**
 *  \brief Clear watching list of thread.
 *  \param thread Watching thread.
 */
_EXT_C void datu_clear_watch( const threadu_t thread );

/**
 *  \brief Reset data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \param n State-space index.
 */
_EXT_C void datu_reset( const threadu_t thread, const varu_t item, const depthu_t n );

/**
 *  \brief Reset data changes flags of all variables of thread.
 *  \param thread Watching thread.
 *  \param n State-space index.
 */
_EXT_C void datu_reset_all( const threadu_t thread, const depthu_t n );

/**
 *  \brief Search variable by name.
 *  \param s Variable name.
 *  \param from From variable.
 *  \param to Up to variable.
 */
_EXT_C varn_t datu_atovar( const char *s, const varu_t from, const varu_t to );

/**
 *  \brief Get variable name.
 *  \param item Variable.
 *  \return Name.
 */
_EXT_C char *datu_vartoa( const varu_t item );

/**
 *  \brief Set state-space from snapshot.
 *  \param datn - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datu_set_vector( const DATU_VECTOR *datu, const depthu_t n );

/**
 *  \brief Set state-space from snapshot (test variables only _ut_*).
 *  \param datn - Snapshot pointer.
 *  \param n State-space index.
 */
_EXT_C int datu_set_vector__ut( const DATU_VECTOR *datu, const depthu_t n );

/**
 *  \brief Get state-space data pointer.
 *  \param n State-space index.
 *  \return Data pointer.
 */
_EXT_C DATU_VECTOR *datu_get_vector( const depthu_t n );

/**
 *  \brief Compare vectors.
 *  \param datn1 - 1st state-space.
 *  \param datn2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datu_cmp_vector( const DATU_VECTOR *datu1, const DATU_VECTOR *datu2 );

/**
 *  \brief Compare vectors (test variables only _ut_*).
 *  \param datn1 - 1st state-space.
 *  \param datn2 - 2nd state-space.
 *  \return Equality flag.
 */
_EXT_C int datu_cmp_vector__ut( const DATU_VECTOR *datu1, const DATU_VECTOR *datu2 );

/** \cond */

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
    _EXT_C char *stru_##T( char *s, T value, int *col );
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto ) \
    _EXT_C T datu_get_##T( const varu_t item, const depthu_t n ); \
    _EXT_C char *datu_get_string_##T( char *s, const varu_t item, const depthu_t n ); \
    _EXT_C void datu_set_##T( const varu_t item, T value, const depthu_t n );
#define DATU_VAR( var, type, title, string )
#define DATU_ARRAY( len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY

/** \endcond */

/**
 *  \brief Set value of variable of any type.
 *  \param item Variable.
 *  \param value Value pointer.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datu_set( const varu_t item, void *value, const depthu_t n );

/**
 *  \brief Set taken from string value of variable.
 *  \param item Variable.
 *  \param s String.
 *  \param n State-space index.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void datu_set_string( const varu_t item, const char *s, const depthu_t n );

/**
 *  \brief Get value of variable as string.
 *  \param s String buffer.
 *  \param item Variable.
 *  \param n State-space index.
 *  \return String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C char *datu_get_string( char *s, const varu_t item, const depthu_t n );

/**
 *  \brief Get variable type as string.
 *  \param item Variable.
 */
_EXT_C char *datu_get_type( const varu_t item );

/**
 *  \brief Get title of variable.
 *  \param item Variable.
 */
_EXT_C char *datu_get_title( const varu_t item );

/**
 *  \brief Lock data access (callback).
 */
_EXT void ( *datu_lock_callback )( void );

/**
 *  \brief Unlock data access (callback).
 */
_EXT void ( *datu_unlock_callback )( void );

/**
 *  \brief Lock data access.
 */
_EXT_C void datu_lock( void );

/**
 *  \brief Unlock data access.
 */
_EXT_C void datu_unlock( void );

_EXT_C void datu_clear_updated( const depthu_t n );
_EXT_C void datu_set_updated( const depthu_t n );
_EXT_C uint8_t datu_check_updated( const depthu_t n );

/** \} */

#include "h-end.h"

#endif /* DATN_H */

