/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-str.c
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATB_STR_IMPLEMENT
#include "include.h"

#define STR_UNDEFINED           "?"
#define STR_OK                  "ок"
#define STR_ERROR               "ошиб."
#define STR_DIFFER              "откл."

#define STR_TURN_OFF            "выкл."
#define STR_TURN_ON             "вкл."
#define STR_CHECKUP             "проверка"
#define STR_WORK                "работа"

static const char * const STR_LINK[ BL_CH_COUNT ] = { "CAN1 ", "CAN2 ", "CAN3 " };

char *strb_state( char *s, byte val, int *col )
{
	( void ) s;

	switch ( val ) {
	case REGWORK_VU_ON   : *col = COLOR_OK;
		break;
	}
	switch ( val ) {
	case REGWORK_VU_OFF  : return __BR_O__ STR_TURN_OFF __BR_C__;
	case REGWORK_VU_CHECK: return __BR_O__ STR_CHECKUP __BR_C__;
	case REGWORK_VU_ON   : return __BR_O__ STR_TURN_ON __BR_C__;
	case REGWORK_VU_WORK : return __BR_O__ STR_WORK __BR_C__;
	}
	return __BR_O__ STR_UNDEFINED __BR_C__;
}

char *strb_error( char *s, byte val, int *col )
{
	( void ) s;

	if ( val ) {
		*col = COLOR_FAIL;
		return __BR_O__ STR_ERROR __BR_C__;
	}
	*col = COLOR_OK;
	return __BR_O__ STR_OK __BR_C__;
}

char *strb_rserr( char *s, byte val, int *col )
{
	( void ) col;

	return strb_error( s, val, col );
}

char *strb_differ( char *s, byte val, int *col )
{
	( void ) s;

	if ( val ) {
		*col = COLOR_FAIL;
		return __BR_O__ STR_DIFFER __BR_C__;
	}
	*col = COLOR_OK;
	return __BR_O__ STR_OK __BR_C__;
}

char *strb_link( char *s, byte val, int *col )
{
	unsigned int l = strlen( __BR_O__ );
	int i;

	if ( val == 0 ) {
		*col = COLOR_OK;
		return __BR_O__ STR_OK __BR_C__;
	}

	*col = COLOR_FAIL;
	strcpy( s, __BR_O__ );
	for ( i = 0; i < BL_CH_COUNT; i++ ) {
		if ( val & BIT( i )) {
			strcpy( s + l, STR_LINK[ i ]);
			l += strlen( STR_LINK[ i ]);
		}
	}
	strcpy( s + l - 1, __BR_C__ );
	return ( s );
}
