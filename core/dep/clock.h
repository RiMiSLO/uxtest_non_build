/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  clock.h
 *  \brief  Real-time clock.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef CLOCK_H
#define CLOCK_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CLOCK_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup hardware Hardware
 *  \{
 *
 *  \defgroup clock Real-time clock
 *  \{
 */

#define TICK_PER_SEC  1000  /**< Ticks per second. \hideinitializer */

/**
 *  \brief Initialize clock.
 */
_EXT_C void clock_init( void );

/**
 *  \brief Time to string converting.
 *  \param tval time pointer (current time is used in case of NULL).
 *  \param st String buffer.
 *  \return String.
 */
_EXT_C const char *clock_stamp( const struct timeval *tval, char *st );

/**
 *  \brief Current time to string converting without centiseconds.
 *  \param st String buffer.
 *  \return String.
 */
_EXT_C const char *clock_stamp_short( char *st );

/**
 *  \brief Current datetime to string converting without centiseconds.
 *  \param sd String buffer for date.
 *  \param st String buffer for time.
 *  \return Current datetime.
 */
_EXT_C time_t clock_stamp_datetime( char* sd, char* st );

/**
 *  \brief Current time setup.
 *  \param Hours.
 *  \param m Minutes.
 *  \param s Seconds.
 *  \param cs Centiseconds.
 *  Function is not implemented.
 */
_EXT_C void clock_set_time( uint32_t h, uint32_t m, uint32_t s, uint32_t cs );

/**
 *  \brief Get time of program running (max 497 days).
 *  \return Time in centiseconds.
 */
_EXT_C uint32_t clock_time_running( void );

/**
 *  \brief Calculate time when timer interval has expired (timeout).
 *  \param ms Interval in milliseconds.
 *  \return Timeout.
 */
_EXT_C struct timeval clock_t_set( const int ms );

_EXT_C void __clock_t_add( struct timeval *t, const int ms );
_EXT_C int __clock_time_after( struct timeval *t );
_EXT_C int __clock_t_rest( struct timeval *t );

_EXT_C time_t dt2time_t( dword d, dword t );
_EXT_C void time_t2dt( time_t tt, dword *d, dword *t );

/**
 *  \brief Prolong timeout.
 *  \param t Timeout.
 *  \param ms Additional interval in milliseconds.
 */
#define clock_t_add( t, ms ) __clock_t_add(&( t ), ( ms ))

/**
 *  \brief Check timeout.
 *  \param t Timeout.
 *  \return Timeout flag.
 */
#define clock_time_after( t ) __clock_time_after( &( t ))

/**
 *  \brief Check timeout.
 *  \param t Timeout.
 *  \return NO timeout flag.
 */
#define clock_time_before( t ) ( !clock_time_after( t ))

/**
 *  \brief Calculate remaining time until timeout.
 *  \param t Timeout.
 *  \return Remaining time.
 */
#define clock_t_rest( t ) __clock_t_rest( &( t ))

/** \} */

/** \} */

#include "h-end.h"

#endif /* CLOCK_H */
