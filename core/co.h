/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  co.h
 *  \brief  Coroutines and unit tests declarations.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef CO_H
#define CO_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CO_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup coro Coroutines.
 *  \ingroup core
 *  \{
 */
/** \cond */

#define CORO( name ) \
	_VAR_C( CORO_CONTEXT( name )); \
	_VAR_C_INIT( int name##_alive, CO_SKIP ); \
	_EXT_C int name##_init( void ); \
	_EXT_C void name##_uninit( void ); \
	_EXT_C CORO_DEFINE( name );

#include "co-dict.h"

#undef CORO

/** \endcond */

/** \} */

#ifdef LINK_UT

/**
 *  \addtogroup ut
 *  \{
 */

/**
 * \brief Restart unit testing of 2nd stage.
 */
extern void ut_2_restart( void );

/** \cond */

#define UT_1( test ) \
	extern void ut_##test( void );
#define UT_2( test ) \
	extern int ut_##test##_alive;

#include "ut-dict.h"

#undef UT_2
#undef UT_1

/** \endcond */

/** \} */

#endif /* LINK_UT */

#include "h-end.h"

#endif /* CO_H */
