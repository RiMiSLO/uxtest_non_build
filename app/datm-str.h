/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datm-str.h
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATM_STR_H
#define DATM_STR_H

#ifdef  DATM_STR_IMPLEMENT
#define DATM_STR_EXT
#define DATM_STR_EXT_INIT( dec, init ) \
    dec = init
#else
#define DATM_STR_EXT extern
#define DATM_STR_EXT_INIT( dec, init ) \
    extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datm
 *  \{
 */

DATM_STR_EXT char *strm_state( char *s, byte val, int *col );
DATM_STR_EXT char *strm_crc16( char *s, word val, int *col );
DATM_STR_EXT char *strm_date( char *s, dword val, int *col );
DATM_STR_EXT char *strm_time( char *s, dword val, int *col );
DATM_STR_EXT char *strm_sn( char *s, dword val, int *col );
DATM_STR_EXT char *strm_have( char *s, dword val, int *col );
DATM_STR_EXT char *strmcp_state( char *s, byte val, int *col );
DATM_STR_EXT char *strm_mi_error_st( char *s, byte val, int *col );
/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATM_STR_H */
