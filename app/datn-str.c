/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-str.c
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATN_STR_IMPLEMENT
#include "include.h"


#define STR_UNDEFINED           "?"
#define STR_PASSED              "ок"
#define STR_FAILED              "неиспр."

#define STR_TEST_NONE           " "
#define STR_TEST_PERFORM        "идет..."
#define STR_TEST_PASSED         STR_PASSED
#define STR_TEST_FAILED         STR_FAILED
#define STR_TEST_ERROR          "ошиб."
#define STR_TEST_ABORTED        "ост."

#define STR_PERFORM_NONE        " "

#define STR_LINK_NONE           " "
#define STR_LINK_CONNECTED      "ок"
#define STR_LINK_DISCONNECTED   "разрыв"
#define STR_LINK_FAILED         STR_FAILED

#define STR_TURN_OFF            "выкл."
#define STR_TURN_ON             "вкл."
#define STR_RESTORE_WORK        "восст."
#define STR_WORK                "работа"
#define STR_WORK_PASS           "пассив."
#define STR_KPA                 "ПКФ"

#define STR_NO_TEST             " "
#define STR_TEST_KBD            "клавиатура"
#define STR_TEST_EXPRESSRAM     "ОЗУ (э)"
#define STR_TEST_RAM            "ОЗУ"
#define STR_TEST_HDD            "диск"
#define STR_TEST_EXPRESSHDD     "диск (э)"
#define STR_TEST_CMOS           "таймеры"
#define STR_TEST_PIC            "прерывания"
#define STR_TEST_MOUSE          "мышь"
#define STR_TEST_SPEAKER        "динамик"
#define STR_TEST_EXPRESSVIDEO   "видео (э)"
#define STR_TEST_VIDEO          "видео"
#define STR_TEST_ETHERNET       "сеть"

#define STR_PACK_NONE           " "
#define STR_PACK_1              ""
#define STR_PACK_2              ""
#define STR_PACK_3              ""
#define STR_PACK_AUTORUN        "нач.вкл"

/* XXX
    Порядок тестов как в NM_TEST!
 */
const char * const STR_TEST[] = {
	__BR_O__ STR_NO_TEST __BR_C__,
	__BR_O__ STR_NO_TEST __BR_C__,
	__BR_O__ STR_NO_TEST __BR_C__,
	__BR_O__ STR_TEST_ETHERNET __BR_C__,
	__BR_O__ STR_TEST_KBD __BR_C__,
	__BR_O__ STR_TEST_EXPRESSRAM __BR_C__,
	__BR_O__ STR_TEST_RAM __BR_C__,
	__BR_O__ STR_TEST_HDD __BR_C__,
	__BR_O__ STR_TEST_EXPRESSHDD __BR_C__,
	__BR_O__ STR_TEST_CMOS __BR_C__,
	__BR_O__ STR_TEST_PIC __BR_C__,
	__BR_O__ STR_TEST_MOUSE __BR_C__,
	__BR_O__ STR_TEST_SPEAKER __BR_C__,
	__BR_O__ STR_TEST_EXPRESSVIDEO __BR_C__,
	__BR_O__ STR_TEST_VIDEO __BR_C__,
};

char *pack_title[ PACK__LIMIT_ ] = {
	__BR_O__ STR_PACK_NONE __BR_C__,
	__BR_O__ STR_PACK_1 __BR_C__,
	__BR_O__ STR_PACK_2 __BR_C__,
	__BR_O__ STR_PACK_3 __BR_C__,
	__BR_O__ STR_PACK_AUTORUN __BR_C__,
};

char *strn_byte_h( char *s, byte val, int *col )
{
	( void ) col;

	return ( string_byte_h( s, val ));
}

char *strn_word_h( char *s, word val, int *col )
{
	( void ) col;

	return ( string_word_h( s, val ));
}

char *strn_dword_h( char *s, dword val, int *col )
{
	( void ) col;

	return ( string_dword_h( s, val ));
}

char *strn_crc32( char *s, dword val, int *col )
{
	( void ) col;

    sprintf( s, "%08X", val );
	return ( s );
}

char *strn_state( char *s, byte val, int *col )
{
	( void ) s;

	switch ( val ) {
	case TURN_ON      : *col = COLOR_OK;
		break;
	}
	switch ( val ) {
	case TURN_OFF     : return __BR_O__ STR_TURN_OFF __BR_C__;
	case TEST_STATE   :
	case TURN_ON      : return __BR_O__ STR_TURN_ON __BR_C__;
	case RESTORE_WORK : return __BR_O__ STR_RESTORE_WORK __BR_C__;
	case WORK         : return __BR_O__ STR_WORK __BR_C__;
	case PKF          : return __BR_O__ STR_KPA __BR_C__;
	}
	return __BR_O__ STR_UNDEFINED __BR_C__;
}

char *strn_failure( char *s, byte val, int *col )
{
	( void ) s;

	if ( val ) {
        *col = COLOR_FAIL;
        return __BR_O__ STR_FAILED __BR_C__;
	}
	*col = COLOR_OK;
    return __BR_O__ STR_PASSED __BR_C__;
}

char *strn_percent( char *s, int val, int *col )
{
	( void ) col;

	return ( string_percent( s, val ));
}

char *strn_date( char *s, dword val, int *col )
{
	( void ) col;

	sprintf( s, "%02X.%02X.%04X", ( byte ) DATE_GET( val, D ), ( byte ) DATE_GET( val, M ), ( word ) DATE_GET( val, Y ));
	return ( s );
}

char *strn_time( char *s, dword val, int *col )
{
	( void ) col;

	sprintf( s, "%02X:%02X:%02X", ( byte ) TIME_GET( val, H ), ( byte ) TIME_GET( val, M ), ( byte ) TIME_GET( val, S ));
	return ( s );
}

char *strn_link( char *s, byte val, int *col )
{
	( void ) s;

	switch ( val ) {
	case LINK_CONNECTED   : *col = COLOR_OK;
		break;
	case LINK_FAILED      : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case LINK_NONE        : return __BR_O__ STR_LINK_NONE __BR_C__;
	case LINK_CONNECTED   : return __BR_O__ STR_LINK_CONNECTED __BR_C__;
	case LINK_DISCONNECTED: return __BR_O__ STR_LINK_DISCONNECTED __BR_C__;
	case LINK_FAILED      : return __BR_O__ STR_LINK_FAILED __BR_C__;
	}
	return __BR_O__ STR_UNDEFINED __BR_C__;
}

char *strn_test( char *s, byte val, int *col )
{
	( void ) s;

	switch ( val ) {
	case TEST_PASSED : *col = COLOR_OK;
		break;
	case TEST_ERROR  :
	case TEST_FAILED : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TEST_NONE   : return __BR_O__ STR_TEST_NONE __BR_C__;
	case TEST_PERFORM: return __BR_O__ STR_TEST_PERFORM __BR_C__;
	case TEST_PASSED : return __BR_O__ STR_TEST_PASSED __BR_C__;
	case TEST_FAILED : return __BR_O__ STR_TEST_FAILED __BR_C__;
	case TEST_ERROR  : return __BR_O__ STR_TEST_ERROR __BR_C__;
	case TEST_ABORTED: return __BR_O__ STR_TEST_ABORTED __BR_C__;
	}
	return __BR_O__ STR_UNDEFINED __BR_C__;
}

char *strn_perform( char *s, word val, int *col )
{
	word n = PERFORM_GET( val, PACK );

	( void ) s;

	if (( n != PACK_NONE ) && ( n < PACK__LIMIT_ )) {
        *col = COLOR_OK;
		return ( char *) pack_title[ n ];
	}
	n = PERFORM_GET( val, TEST );
	if (( n >= TEST__FIRST_ ) && ( n <= TEST__LAST_ )) {
		*col = COLOR_OK;
		return ( char *) STR_TEST[ n ];
	}
	return __BR_O__ STR_PERFORM_NONE __BR_C__;
}
char *strn_snmp1(char *s, char val, int *col){

    //oidznachint = reciveSNMPint(snmpSendMess1, 42, "192.168.0.247");
    *col = COLOR_OK;
    return "(char)oidznachint";


}
