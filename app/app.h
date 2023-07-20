/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  app.h
 *  \brief  Application.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef APP_H
#define APP_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( APP_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup app Application
 *  \{
 */

/**
 *  \brief Show-must-go-on flag.
 */
_VAR_C( int app_alive );

/**
 *  \brief Application exit status.
 */
_VAR_C_INIT( int app_return, 0 );

/**
 *  \brief Initialize application.
 */
_EXT int app_init( int argc, char *argv[]);

/**
 *  \brief Uninitialize application.
 */
_EXT void app_uninit( void );

/**
 *  \brief Debug interface title message.
 *  \hideinitializer
 *//*
#define __CONSOLE_TITLE__ __CRLF__ \
"" __CRLF__ \
"" __CRLF__ \
"" __CRLF__;
 */

/**
 *  \brief predict main function.
 */
_EXT_C int _main( int argc, char *argv[]);

/** \} */

#include "h-end.h"

#endif /* APP_H */
