/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-vote.c
 *  \brief  Функции голосование состояния блоков.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATB_VOTE_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datb
 *  \{
 */

valb_t voteb_state( valb_t value_1, valb_t value_2, valb_t value_3 )
{



//        printf("\n\nv1:%d\n", value_1);
//        printf("v2:%d\n", value_2);
//        printf("v3:%d\n\n", value_3);

	if (( value_1 == value_2 ) && ( value_2 == value_3 )) return ( value_1 );

	/* Неопределенное состояние - отобразить "красным" */
	return (( valb_t ) -1 );
}

valb_t voteb_failure( valb_t value_1, valb_t value_2, valb_t value_3 )
{
	return ( value_1 || value_2 || value_3 );
}

uint8_t voteh_state( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 )
{
	/* 2 из 3 */
	*value = ( value_1 & value_2 ) | (( value_1 | value_2 ) & value_3 );

	if ( *value != 0 ) return ( 0 );

	/* 1 из 3 */
	if ( value_1 ) {
		*value = value_1;

		return ( 1 );
	}
	if ( value_2 ) {
		*value = value_2;

		return ( 2 );
	}
	if ( value_3 ) {
		*value = value_3;

		return ( 3 );
	}

	return ( 0 );
}

uint8_t voteh_error( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 )
{
	uint8_t sum = value_1 + value_2 + value_3;

	*value = ( sum > 0 );

	if ( sum != 1 ) return ( 0 );
	if ( value_1 ) return ( 1 );
	if ( value_2 ) return ( 2 );
	return ( 3 );
}

uint8_t voteh_h_link( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 )
{
	*value = value_1 | value_2 | value_3;

	return ( 0 );
}

/** \} */
