/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  getch.h
 *  \brief  Read from debug interface.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef GETCH_H
#define GETCH_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( GETCH_IMPLEMENT )
#include "h-begin.h"

#ifdef LINK_CONSOLE

/**
 *  \addtogroup char
 *  \{
 */

_EXT_C void getch_init( void );
_EXT_C void getch_uninit( void );
_EXT_C int getch( void );
_EXT_C int getch_1( void );
_EXT_C int getch_escseq( int chr );

/** \} */

#endif /* LINK_CONSOLE */

/**
 *  \defgroup char_char Characters definitions
 *  \ingroup char
 *  \{
 */

#define CHAR_TAB        '\x09'
#define CHAR_LF         '\x0A'
#define CHAR_CR         '\x0D'
#define CHAR_ESC        '\x1B'
#define CHAR_SPACE      '\x20'
#define LAST_CHAR       '\x7E'  /**< Last character to be displayed. */
#define CHAR_BACKSPACE  '\x7F'
#define CHAR_UP         '\x41'
#define CHAR_DOWN       '\x42'
#define CHAR_RIGHT      '\x43'
#define CHAR_LEFT       '\x44'
#define CHAR_ENTER      CHAR_LF
#define __CRLF__        "\x0A"

/** \} */

#include "h-end.h"

#endif /* GETCH_H */
