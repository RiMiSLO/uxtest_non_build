/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  coding.cpp
 *  \brief  Преобразование кодировок.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef CODING_H
#define CODING_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CODING_IMPLEMENT )
#include "h-begin.h"

_EXT_C void coding_init( const char *console, const char *system );

#ifdef __cplusplus
#include <QString>

_EXT QString decode_cp1251( const char *src );
_EXT QString decode_system( const char *src );
_EXT const char *encode_cp1251( char *dst, QString *src, uint16_t len );
_EXT const char *encode_console( char *dst, QString *src, uint16_t len );
_EXT const char *encode_system( char *dst, QString *src, uint16_t len );
_EXT const char *encode_utf8( char *dst, QString *src, uint16_t len );

#endif

#include "h-end.h"

#endif /* CODING32_H */
