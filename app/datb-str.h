/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-str.h
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATB_STR_H
#define DATB_STR_H

#ifdef  DATB_STR_IMPLEMENT
#define DATB_STR_EXT
#define DATB_STR_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATB_STR_EXT extern
#define DATB_STR_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datb
 *  \{
 */

DATB_STR_EXT char *strb_state( char *s, byte val, int *col );
DATB_STR_EXT char *strb_error( char *s, byte val, int *col );
DATB_STR_EXT char *strb_rserr( char *s, byte val, int *col );
DATB_STR_EXT char *strb_differ( char *s, byte val, int *col );
DATB_STR_EXT char *strb_link( char *s, byte val, int *col );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATB_STR_H */
