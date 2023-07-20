/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  char.h
 *  \brief  Debug interface.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef CHAR_H
#define CHAR_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CHAR_IMPLEMENT )
#include "h-begin.h"

#ifdef LINK_CONSOLE

/**
 *  \defgroup char Debug interface
 *  \ingroup console
 *  \{
 */

#define ESCSEQ_INSIDE  0  /**< Inside of escape sequence. \hideinitializer */
#define ESCSEQ_1       1  /**< Single character. \hideinitializer */
#define ESCSEQ_OK      2  /**< Last character of escape sequence. \hideinitializer */

#define RX_SIZE  80  /**< Size of input buffer. \hideinitializer */

#ifndef COMMA
#define COMMA ,
#endif

/**
 *  \brief Received message pointer.
 *  \note Must be reset in NULL after message handling.
 */
_VAR_C( char *char_received );

/**
 *  \brief Flag of displaying of received characters.
 */
_VAR_C( int char_echo );

/**
 *  \brief Clear input message.
 */
_EXT_C void char_clear_input( void );

/**
 *  \brief Print data in hex format.
 *  \param data Data pointer.
 *  \param len Data length.
 */
_EXT_C void printh( byte *data, uint32_t len );

/**
 *  \brief Print data in hex format by columns.
 *  \param data Data pointer.
 *  \param len Data length.
 *  \param cols Columns count.
 */
_EXT_C void printhr( byte *data, uint32_t len, uint32_t cols );

/**
 *  \brief Print program version.
 */
_EXT_C void print_version( void );

/** \} */

#endif /* LINK_CONSOLE */

/**
 *  \brief Flush output.
 */
_EXT_C void flush( void );

/**
 *  \brief Print message.
 *  \param PRE Message type.
 *  \param FORMAT Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define __echo( PRE, FORMAT, ... ) do { \
	printf( __CRLF__ "*** " PRE " *** %s() %s (%d)" __CRLF__ "    " FORMAT, \
	__FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__ ); \
} while ( 0 )

/**
 *  \brief Print message about function call.
 *  \param FORMAT Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define __echo_call( FORMAT, ... ) __echo( "CALL", FORMAT, ## __VA_ARGS__ )

/**
 *  \brief Print debug message.
 *  \param FORMAT Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define __echo_debug( FORMAT, ... ) __echo( "INFO", FORMAT, ## __VA_ARGS__ )

/**
 *  \brief Print warning.
 *  \param FORMAT Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define __echo_warn( FORMAT, ... ) do { \
	__echo( "WARN", ); \
	printf( FORMAT, ## __VA_ARGS__ ); \
	flush(); \
} while ( 0 )

/**
 *  \brief Print error.
 *  \param FORMAT Message format.
 *  \param err Error number.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define __echo_error( FORMAT, err, ... ) do { \
	__echo( "ERROR #%X", COMMA err ); \
	printf( FORMAT, ## __VA_ARGS__ ); \
	flush(); \
} while ( 0 )

/**
 *  \brief Console width.
 */
_VAR_C_INIT( int console_col_count, 80 );

/**
 *  \brief Console height.
 */
_VAR_C_INIT( int console_row_count, 25 );

#include "h-end.h"

#endif /* CHAR_H */
