/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  crc.h
 *  \brief  Вычисление CRC.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef CRC_H
#define CRC_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( CRC_IMPLEMENT )
#include "h-begin.h"

_EXT_C uint32_t crc32_calc( const uint8_t *buf, uint32_t len );
_EXT_C uint16_t crc16_calc( const uint8_t *buf, uint32_t len );

#include "h-end.h"

#endif /* CRC_H */
