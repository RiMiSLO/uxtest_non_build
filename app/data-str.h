/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data-str.h
 *  \brief  String converting.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATA_STR_H
#define DATA_STR_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATA_STR_IMPLEMENT )
#include "h-begin.h"

/**
 *  \addtogroup data
 *  \{
 */

#define __strtol( s, end )    strtol( s, end, 0 )
#define __strtoul( s, end )   strtoul( s, end, 0 )
#define __strtoll( s, end )   strtoll( s, end, 0 )
#define __strtoull( s, end )  strtoull( s, end, 0 )
#define __strtof( s, end )    (( float ) strtod( s, end ))

/**
 *  \brief byte-to-hex converting.
 *  \param s String buffer.
 *  \param val Value.
 *  \return String.
 */
_EXT_C char *string_byte_h( char *s, byte val );

/**
 *  \brief word-to-hex converting.
 *  \param s String buffer.
 *  \param val Value.
 *  \return String.
 */
_EXT_C char *string_word_h( char *s, word val );

/**
 *  \brief dword-to-hex converting.
 *  \param s String buffer.
 *  \param val Value.
 *  \return String.
 */
_EXT_C char *string_dword_h( char *s, dword val );

_EXT_C char *string_percent( char *s, int val );

/** \} */

#include "h-end.h"

#endif /* DATA_STR_H */
