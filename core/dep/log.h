/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  log.h
 *  \brief  Logging.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef LOG_H
#define LOG_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( LOG_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup log_level Logging level
 *  \ingroup log
 *  \{
 */
#define LOG_LEVEL_OFF    0  /**< Logging off. \hideinitializer */
#define LOG_LEVEL_ERROR  1  /**< Errors. \hideinitializer */
#define LOG_LEVEL_WARN   2  /**< Errors and warnings. \hideinitializer */
#define LOG_LEVEL_INFO   3  /**< Errors, warnings and info. \hideinitializer */

/** \} */

#ifdef LOGGING

/**
 *  \defgroup log Logging
 *  \ingroup core
 *
 *	Structure of <b>log-dict.yu</b> file:
 *
 *  <b>LOG_WARN</b> - Dictionary of warnings.
 *  \param LOG_WARN Identifier of warning.
 *  \param WARN_FORMAT Message format.
 *  \param WARN_REPEAT Flag of repeated warning logging.
 *
 *  <b>LOG_INFO</b> - Dictionary of info messages.
 *  \param LOG_WARN Identifier of info.
 *  \param LOG_FORMAT Message format.
 *  \param LOG_REPEAT Flag of repeated info logging.
 *  \{
 */

/**
 *  \defgroup log_repeat Repeated logging flags
 *  \{
 */
#define LOG_NEVER  0    /**< Do not log message. \hideinitializer */
#define LOG_ONCE   1    /**< Log message once. \hideinitializer */
#define LOG_EVER   255  /**< Log message every time. \hideinitializer */

/** \} */

/**
 *  \brief Write message into log-file.
 *  \param pre Message type.
 *  \param format Message format.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define log_printf( pre, format, ... ) do { \
	if ( log_file == NULL ) break; \
	fprintf( log_file, "\n%s\t" pre "\t", clock_stamp( NULL, __log_str )); \
	fprintf( log_file, format, ## __VA_ARGS__ ); \
	fflush( log_file ); \
} while ( 0 )

/**
 *  \brief Write message for dictionary into log-file.
 *  \param pre Message type.
 *  \param name Identifier.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define log_message( pre, name, ... ) do { \
	if ( log_file == NULL ) break; \
	if (( ini_log_level >= LOG_LEVEL_##pre ) && pre##_repeat[ pre##_##name ]) { \
		if ( log_lock_callback ) log_lock_callback(); \
		log_printf( #pre, pre##_format[ pre##_##name ], ## __VA_ARGS__ ); \
		fflush( log_file ); \
		if ( pre##_repeat[ pre##_##name ] < LOG_EVER ) { \
			if ( --pre##_repeat[ pre##_##name ] == LOG_NEVER ) { \
				fprintf( log_file, " (this message is reported last time)" ); \
			} \
		} \
		if ( log_unlock_callback ) log_unlock_callback(); \
	} \
} while ( 0 )

/**
 *  \brief Write warning into log-file.
 *  \param name Identifier.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define WARN( name, ... )  __log_message( WARN, name, ## __VA_ARGS__ );

/**
 *  \brief Write info message into log-file.
 *  \param name Identifier.
 *  \param ... Message arguments.
 *  \hideinitializer
 */
#define INFO( name, ... )  __log_message( INFO, name, ## __VA_ARGS__ );

/** \cond */

#define LOG_WARN( name, format, repeat ) \
	WARN_##name,
#define LOG_INFO( name, format, repeat )

enum {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO

#define LOG_WARN( name, format, repeat )
#define LOG_INFO( name, format, repeat ) \
	INFO_##name,

enum {
#include "log-dict.h"
};

#undef LOG_WARN
#undef LOG_INFO

/** \endcond */

_VAR_C_INIT( FILE *log_file, NULL );

_VAR_C( char __log_str[ 32 ]);

/**
 *  \brief Warnings formats.
 */
_EXT_C const char *const WARN_format[];

/**
 *  \brief Info messages formats.
 */
_EXT_C const char *const INFO_format[];

/**
 *  \brief Error writing counters.
 */
_EXT_C uint8_t ERROR_repeat[];

/**
 *  \brief Warning writing counters.
 */
_EXT_C uint8_t WARN_repeat[];

/**
 *  \brief Info message writing counters.
 */
_EXT_C uint8_t INFO_repeat[];

/**
 *  \brief Open log-file.
 *  \param mode "wa" or "a+".
 */
_EXT_C void log_open( const char *mode );

/**
 *  \brief Close log-file.
 */
_EXT_C void log_close( void );

/**
 *  \brief Clear log-file.
 */
_EXT_C void log_clear( void );

/**
 *  \brief Lock log-file access (callback).
 */
_VAR_C( void ( *log_lock_callback )( void ));

/**
 *  \brief Unlock log-file access (callback).
 */
_VAR_C( void ( *log_unlock_callback )( void ));

/** \} */

#else /* LOGGING */

#define log_printf( pre, format, ... ) NULL
#define log_message( pre, name, ... ) NULL
#define WARN( name, ... ) NULL
#define INFO( name, ... ) NULL

#endif /* LOGGING */

#include "h-end.h"

#endif /* LOG_H */
