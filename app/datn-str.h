/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-str.h
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */


#ifndef DATN_STR_H
#define DATN_STR_H

#ifdef  DATN_STR_IMPLEMENT
#define DATN_STR_EXT
#define DATN_STR_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATN_STR_EXT extern
#define DATN_STR_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datn
 *  \{
 */

#define __strtol( s, end )    strtol( s, end, 0 )
#define __strtoul( s, end )   strtoul( s, end, 0 )
#define __strtoll( s, end )   strtoll( s, end, 0 )
#define __strtoull( s, end )  strtoull( s, end, 0 )
#define __strtof( s, end )    (( float ) strtod( s, end ))

#ifdef _MSC_VER
#define strtoull _strtoui64

#endif

#define __BR_O__  ""
#define __BR_C__  ""

#define COLOR_DEFAULT  0x474747
#define COLOR_OK       0x008000
#define COLOR_FAIL     0xFF0000
#define COLOR_PROBLEM  0xccff00

DATN_STR_EXT char *strn_byte_h( char *s, byte val, int *col );
DATN_STR_EXT char *strn_word_h( char *s, word val, int *col );
DATN_STR_EXT char *strn_dword_h( char *s, dword val, int *col );

DATN_STR_EXT char *strn_state( char *s, byte val, int *col );
DATN_STR_EXT char *strn_failure( char *s, byte val, int *col );
DATN_STR_EXT char *strn_percent( char *s, int val, int *col );
DATN_STR_EXT char *strn_date( char *s, dword val, int *col );
DATN_STR_EXT char *strn_time( char *s, dword val, int *col );
DATN_STR_EXT char *strn_link( char *s, byte val, int *col );
DATN_STR_EXT char *strn_test( char *s, byte val, int *col );
DATN_STR_EXT char *strn_perform( char *s, word val, int *col );
DATN_STR_EXT char *strn_crc32( char *s, dword val, int *col );
DATN_STR_EXT char *strn_snmp1(char *s, char val, int *col);

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATN_STR_H */
