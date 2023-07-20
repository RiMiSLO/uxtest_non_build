/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ini.h
 *  \brief  Configuration options.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef INI_H
#define INI_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( INI_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup ini Configuration options
 *  \ingroup core
 *  \{
 */

/** \cond */

#define INI_INT( name, defval ) _VAR_C_INIT( int ini_##name, defval );
#define INI_BOOL( name, defval ) _VAR_C_INIT( bool ini_##name, defval );
#define INI_STR( name, len, defval ) _VAR_C_INIT( char *ini_##name, ( char *) defval );

#ifdef __cplusplus
#define INI_QSTR( name, defval ) _VAR_C_INIT( QString ini_##name, defval );

#else
#define INI_QSTR( name, defval )

#endif

#include "ini-dict.h"

#undef INI_QSTR
#undef INI_STR
#undef INI_BOOL
#undef INI_INT

/** \endcond */

/**
 *  \brief Configuration file name.
 */
_VAR_C_INIT( const char *ini_path, "./" __PROGRAM__ ".ini" );

_EXT_C bool ini_set_file( char *fn );
_EXT_C void ini_load( void );
_EXT_C void ini_save( void );

/**
 *  \brief Add home directory to path.
 */
_EXT_C char *ini_at_home( const char *fn );

/** \} */

#include "h-end.h"

#endif /* INI_H */
