/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data.h
 *  \brief  Application state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATA_H
#define DATA_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATA_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup data Application state-space representation
 *  \ingroup core
 *
 *	Structure of <b>data-dict.yu</b> file:
 *
 *  <b>DATA_TYPE</b> - Dictionary of variables types. //
 *  \param DATA_TYPE Data type.
 *  \param TYPE_FORMAT Format string for values output.
 *  \param TYPE_STRTO string-to-value function.
 *
 *  <b>DATA_THREAD</b> - Dictionary of threads that watch the changes of state-space (watching threads). // Словарь потоков, которые следят за изменениями пространства состояний (наблюдают за потоками).
 *  \param DATA_THREAD Thread name.
 *
 *  <b>DATA_VAR</b> - Dictionary of variables.// словарь переменных
 *  \param DATA_VAR Variable name.
 *  \param VAR_TYPE Variable type.
 *  \param VAR_LEN Array length (single variable if equals to 1).
 *  \param VAR_STRING value-to-string function.
 *
 *  Usage example:
 *
 *  \code
 *  // Set value of variable
 *  data_set_word( var_name, value );
 *
 *  // Check that value has been changed
 *  if ( data_get_changed( peek_thread, var_name )) {
 *
 *      // Get value
 *      value = data_get_word( var_name );
 *
 *      // Reset data changes flag of variable
 *      data_reset( peek_thread, var_name );
 *  }
 *  \endcode
 *
 *  Support of triple modular redundancy.
 *  <b>DATA_VOTED</b> - Majority opinion variable.
 *  \param DATA_VOTED Variable name.
 *  \param VOTED_TYPE Variable type.
 *  \param VOTED_STRING value-to-string function.
 *
 *  Usage example:
 *
 *  \code
 *  // Set channel value of variable
 *  voted_set_byte( var_name, channel, value, &vote_byte );
 *
 *  // Check that majority opinion has been changed
 *  if ( data_get_changed( peek_thread, var_name )) {
 *
 *      // Check that majority opinion is valid
 *      if ( voted_valid( var_name )) {
 *
 *          // Get majority opinion value
 *          value = data_get_byte( var_name );
 *      }
 *      // Reset data changes flag of variable
 *      data_reset( peek_thread, var_name );
 *  }
 *  \endcode
 *  \{
 */

typedef int thread_t;
typedef int var_t;

#define DATA_VOTED( var, type, string ) \
    DATA_ARRAY( 5, var, type, string ) \
    DATA_ARRAY( 5, var##__VALID, byte, string_byte )  // 4

/** \cond */

#define DATA_THREAD( thread ) peek_##thread,
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

/**
 *  \brief Identifiers of watching threads.
 */
enum {
#include "data-dict.h"
	peek_COUNT
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) type_##T,
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

/**
 *  \brief Data types identifiers.
 */
enum {
#include "data-dict.h"
	type_COUNT
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto )
#define DATA_VAR( var, type, string ) \
	var_##var,

#define T0( N, var, type, string ) \
	var_##var, \
	var_##var##_0 = var_##var,

#define T( N, var, type, string ) \
	var_##var##_##N,

#define DATA_ARRAY( len, var, type, string ) \
	UNFOLD( len, T0, T, var, type, string )

/**
 *  \brief Variables identifiers.
 */
enum {
#include "data-dict.h"
	var_COUNT
};

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY
#undef T0
#undef T

/** \endcond */

/**
 *  \brief Set data changes flag.
 *  \param item Variable.
 */
_EXT_C void data_set_changed( const var_t item );

/**
 *  \brief Reset data changes flag for all watching threads.
 *  \param item Variable.
 */
_EXT_C void data_reset_changed( const var_t item );

/**
 *  \brief Set data changes flags for all variables of watching thread.
 *  \param thread Watching thread.
 */
_EXT_C void data_set_all_changed( const thread_t thread );

/**
 *  \brief Get data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 *  \return Change flag.
 */
_EXT_C int data_get_changed( const thread_t thread, const var_t item );

/**
 *  \brief At least one variable from watching list of thread has been changed.
 *  \param thread Watching thread.
 *  \return Change flag.
 */
_EXT_C int data_get_changed_any( const thread_t thread );

/**
 *  \brief Add variables to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Variables list (var_COUNT is list terminator).
 *  Only first variable of array will be added if array in list.
 */
_EXT_C void data_watch( const thread_t thread, const var_t *vars );

/**
 *  \brief Add arrays to watching list of thread.
 *  \param thread Watching thread.
 *  \param vars Arrays list (var_COUNT is list terminator).
 *  Variable and the rest of array will be added if variable in list.
 */
_EXT_C void data_watch_array( const thread_t thread, const var_t *vars );

/**
 *  \brief Clear watching list of thread.
 *  \param thread Watching thread.
 */
_EXT_C void data_clear_watch( const thread_t thread );

/**
 *  \brief Reset data changes flag of variable.
 *  \param thread Watching thread.
 *  \param item Variable.
 */
_EXT_C void data_reset( const thread_t thread, const var_t item );

/**
 *  \brief Reset data changes flags of all variables of thread.
 *  \param thread Watching thread.
 */
_EXT_C void data_reset_all( const thread_t thread );

/**
 *  \brief Search variable by name.
 *  \param s Variable name.
 *  \return Variable or var_COUNT.
 */
_EXT_C var_t data_atovar( const char *s );

/**
 *  \brief Get variable name.
 *  \param item Variable.
 */
_EXT_C char *data_vartoa( const var_t item );

/** \cond */

#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	_EXT_C char *string_##T( char *s, T value );
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	_EXT_C T data_get_##T( const var_t item ); \
	_EXT_C char *data_get_string_##T( char *s, const var_t item ); \
	_EXT_C void data_set_##T( const var_t item, T value );
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY


#define DATA_THREAD( thread )
#define DATA_TYPE( T, format, strto ) \
	_EXT_C void voted_set_##T( const var_t item, byte h, T value, bool ( *vote )( T *, bool, T, bool, T, bool, T )); \
	_EXT_C void joint_set_##T( const var_t item, T value );
#define DATA_VAR( var, type, string )
#define DATA_ARRAY( len, var, type, string )

#include "data-dict.h"

#undef DATA_THREAD
#undef DATA_TYPE
#undef DATA_VAR
#undef DATA_ARRAY

/** \endcond */


/**
 *  \brief Set value of variable of any type.
 *  \param item Variable.
 *  \param value Value pointer.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void data_set( const var_t item, void *value );

/**
 *  \brief Set taken from string value of variable.
 *  \param item Variable.
 *  \param s String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C void data_set_string( const var_t item, const char *s );

/**
 *  \brief Get value of variable as string.
 *  \param s String buffer.
 *  \param item Variable.
 *  \return String.
 *  Useful only in case variable type is unknown in compile-time.
 */
_EXT_C char *data_get_string( char *s, const var_t item );

/**
 *  \brief Get variable type as string.
 *  \param item Variable.
 */
_EXT_C char *data_get_type( const var_t item );

/**
 *  \brief Check voted value of variable is valid.
 *  \param item Variable.
 *  \return Validity flag.
 */
_EXT_C bool voted_valid( const var_t item );

/**
 *  \brief Lock data access (callback).
 */
_VAR_C( void ( *data_lock_callback )( void ));

/**
 *  \brief Unlock data access (callback).
 */
_VAR_C( void ( *data_unlock_callback )( void ));

/** \} */

/**
 *  \brief Count number of set bits in value.
 *  \param i Value.
 *  \return Number of bits.
 */
_EXT_C int popcount( int i );

#include "h-end.h"

#endif /* DATA_H */
