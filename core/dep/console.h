/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  console.h
 *  \brief  Debug console.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef CONSOLE_H
#define CONSOLE_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CONSOLE_IMPLEMENT )
#include "h-begin.h"

#ifdef LINK_CONSOLE

/**
 *  \defgroup console Debug console.
 *  \ingroup core
 *
 *	Structure of <b>console-dict.h</b> file:
 *
 *  <b>CONSOLE_CMD</b> - Console command.
 *  \param cmd Command name.
 *  \param parn Command parameters count.
 *
 *  <b>CONSOLE_JOB</b> - Cyclic console job.
 *  \param cmd Command name.
 *  \param parn Command parameters count.
 *
 *  \{
 */

/**
 *  \brief Command line prompt.
 *  \hideinitializer
 */
#define CONSOLE_PROMPT       "$"

/**
 *  \brief Command line prompt in case of selecting command from list.
 *  \hideinitializer
 */
#define CONSOLE_PROMPT_LIST  "#"

/**
 *  \brief Console commands count.
 */
_VAR_C( const int CONSOLE_COMMAND_COUNT );

/**
 *  \brief List of console commands.
 */
_EXT_C const char *const console_command[];

/**
 *  \brief Index of selected from list command.
 */
_VAR_C( int console_index );

#define cyclic_delay_MIN  1
#define cyclic_delay_MAX  600

/**
 *  \brief Print prompt.
 */
_EXT_C void console_prompt( void );

/**
 *  \brief Check console is waiting for command.
 */
_EXT_C bool console_command_waited( void );

/** \} */

#endif /* LINK_CONSOLE */

#include "h-end.h"

#endif /* CONSOLE_H */
