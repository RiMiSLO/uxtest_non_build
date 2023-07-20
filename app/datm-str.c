/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datm-str.c
 *  \brief  Функции преобразования значений к строковому виду.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATM_STR_IMPLEMENT
#include "include.h"
#include "string.h"

#define STR_TURN_OFF            "выкл."
#define STR_TURN_ON             "останов"
#define STR_WORK                "измерение"
#define STR_RESET               "сброс"
#define STR_UNDEFINED           "?"
#define STR_NONE                " "
#define STR_IGNORE              "ШИ не исп"
#define STR_UNKNOWN             "неизвестная ошибка"

char *strm_state( char *s, byte val, int *col )
{
    ( void ) s;

    switch ( val ) {
    case MI_ON         : *col = COLOR_OK;
        break;
    case MI_ON_RESET   :
    case MI_WORK_RESET : *col = COLOR_FAIL;
        break;
    }
    switch ( val ) {
    case MI_OFF        : return __BR_O__ STR_TURN_OFF __BR_C__;
    case MI_ON         : return __BR_O__ STR_TURN_ON __BR_C__;
    case MI_ON_RESET   :
    case MI_WORK_RESET : return __BR_O__ STR_RESET __BR_C__;
    case MI_WORK       : return __BR_O__ STR_WORK __BR_C__;
    }
    return __BR_O__ STR_UNDEFINED __BR_C__;
}

DATM_STR_EXT char *strmcp_state( char *s, byte val, int *col ){
    ( void ) s;

    switch ( val ) {
    case MCP_ON         : *col = COLOR_OK;
        break;
    case MCP_WORK         : *col = COLOR_OK;
        break;
    default:              *col = COLOR_FAIL;
        break;
    }
    switch ( val ) {
    case MCP_OFF        : return __BR_O__ STR_TURN_OFF __BR_C__;
    case MCP_STOP       : return __BR_O__ STR_TURN_ON __BR_C__;
    case MCP_IGNORE     : return __BR_O__ STR_IGNORE __BR_C__;
    case MCP_RESET      : return __BR_O__ STR_RESET __BR_C__;
    case MCP_WORK       : return __BR_O__ STR_WORK __BR_C__;
    default             : return __BR_O__ STR_UNKNOWN __BR_C__;
    }
    return __BR_O__ STR_UNDEFINED __BR_C__;

}

char *strm_crc16( char *s, word val, int *col )
{
    ( void ) col;

    sprintf( s, "%04X", val );
    return ( s );
}

char *strm_date( char *s, dword val, int *col )
{
    ( void ) col;

    if ( val == 0 ) return ( STR_NONE );

    sprintf( s, "%02X.%02X.%02X%02X", ( byte ) DATE_M_GET( val, D ), ( byte ) DATE_M_GET( val, M )
    , ( byte ) DATE_M_GET( val, Y ), ( byte ) DATE_M_GET( val, C ));
    return ( s );
}

char *strm_time( char *s, dword val, int *col )
{
    ( void ) col;

    if ( val == 0 ) return ( STR_NONE );

    sprintf( s, "%02X:%02X:%02X", ( byte ) TIME_M_GET( val, H ), ( byte ) TIME_M_GET( val, M ), ( byte ) TIME_M_GET( val, S ));
    return ( s );
}

char *strm_sn( char *s, dword val, int *col )
{
    ( void ) col;

    if ( val == 0 ) return ( STR_NONE );

    sprintf( s, "%03u", val );
    return ( s );
}

char *strm_have( char *s, dword val, int *col )
{
    ( void ) col;
    if (val >10 ){
    return (__BR_O__ "ок" __BR_C__);
    }

        //sprintf(s,"%d", val+1);
    sprintf(s,"%d", val+1);
        return (s);

}

DATM_STR_EXT char *strm_mi_error_st( char *s, byte val, int *col ){

    (void) col;

    char a[8];
    uint8_t b;
    for (int i = 0; i < sizeof(a); i++ ){
        a[i]=0;
    }
    if( val == 0){
        return (__BR_O__ "ок" __BR_C__);
    }
    else {
        if ( val >> 7){
            a[0] = 8;
        }
        b = val >> 6;
        b = b << 7;
        if (b){
            a[1]=7;
        }
        b = val >> 5;
        b = b << 7;
        if (b){
            a[2]=6;
        }
        b = val >> 4;
        b = b << 7;
        if (b){
            a[3]=5;
        }
        b = val >> 3;
        b = b << 7;
        if (b){
            a[4]=4;
        }
        b = val >> 2;
        b = b << 7;
        if (b){
            a[5]=3;
        }
        b = val >> 1;
        b = b << 7;
        if (b){
            a[6]=2;
        }
        b = val;
        b = b << 7;
        if (b){
            a[7]=1;
        }


    sprintf(s, "%d,%d,%d,%d,%d,%d,%d,%d", a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0]);
    return s;

}}

