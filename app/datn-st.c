/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-st.c
 *  \brief  Структуры данных для сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATN_ST_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datn
 *  \{
 */

void datn_msg2vector( DATN_VECTOR *datn, void *sou )
{
	DATN_ST *st = ( DATN_ST *) sou;

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) datn->var =st->var;

#define T( N, len, var, type, title, string ) datn->var[ N ] = st->var[ N ];

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

/**
 *  \brief Структура вектора состояния.
 */
#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

/** \endcond */

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif
}

/** \} */
