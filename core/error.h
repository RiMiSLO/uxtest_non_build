/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  error.h
 *  \brief  Error messages.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef ERROR_H
#define ERROR_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( ERROR_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup error Error messages
 *  \ingroup core
 *
 *	Structure of <b>error-dict.yu</b> file:
 *
 *  <b>DATA_ERROR</b> - Dictionary of errors.
 *  \param DATA_ERROR Identifier of error.
 *  \param ERROR_BIT Bit in pool of errors (var_error).
 *  \param ERROR_FORMAT Message format.
 *  \param ERROR_REPEAT Flag of repeated error logging.
 *  \{
 */

/**
 *  \brief Output error message and set flag in pool of errors.
 *  \param err Identifier of error.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define ERROR_MESSAGE( err, ... ) do { \
	DATA_SET_ERROR( BIT( ERR_BIT_##err )); \
	log_message( ERROR, err, ## __VA_ARGS__ ); \
	__echo_error( ERROR_format[ ERROR_##err ], ERR_BIT_##err, ## __VA_ARGS__ ); \
} while ( 0 )

#define ERROR  ERROR_MESSAGE

/**
 *  \brief Clear flag in pool of errors.
 *  \param err Identifier of error.
 *  \hideinitializer
 */
#define ERROR_CLEAR( err ) do { \
	DATA_CLEAR_ERROR( BIT( __concat( ERR_BIT_, err ))); \
} while ( 0 )

/**
 *  \brief Clear all error flags.
 *  \hideinitializer
 */
#define ERROR_CLEAR_ALL() do { \
	DATA_RESET_ERROR(); \
} while ( 0 )

/**
 *  \brief Verify error flag is cleared.
 *  \param err Identifier of error.
 *  \param ... Operation if verification failed.
 *  \hideinitializer
 */
#define assert_unerring( err, ... ) do { \
	bool bit = ( BIT( __concat( ERR_BIT_, err )) & DATA_GET_ERROR()); \
	if ( bit ) { \
		ut_fault( "expected unerring", __FILE__,  __FUNCTION__, __LINE__ ); \
		__VA_ARGS__; \
	} \
} while ( 0 )

/** \cond */

#define DATA_ERROR( name, bit, format, repeat ) \
	ERROR_##name,

enum {
#include "error-dict.h"
};

#undef DATA_ERROR

#define DATA_ERROR( name, bit, format, repeat ) \
	ERR_BIT_##name = bit,

enum {
#include "error-dict.h"
};

#undef DATA_ERROR

/** \endcond */

/**
 *  \brief Error messages formats.
 */
_EXT_C const char *const ERROR_format[];

/** \} */

#include "h-end.h"

#endif /* ERROR_H */
