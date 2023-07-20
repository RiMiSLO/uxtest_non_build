/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datmcp-mcp.c
 *  \brief  Разбор состояния МИ в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATMCP_MCP_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datmcp
 *  \{
 */

void datmcp_head2vector( uint16_t pool, uint8_t abonent )
{
    datmcp_set_byte( varmcp_failure_hard, DATMCP_GET( pool, FAILURE_HARD ), abonent );
    datmcp_set_byte( varmcp_failure_soft, DATMCP_GET( pool, FAILURE_SOFT ), abonent );
    datmcp_set_byte( varmcp_link0, DATMCP_GET( pool, LINK_0 ), abonent );
    datmcp_set_byte( varmcp_link1, DATMCP_GET( pool, LINK_1 ), abonent );
    //datmcp_set_byte( varmcp_net0, DATMCP_GET( pool, NET_0 ), abonent );
    //datmcp_set_byte( varmcp_net1, DATMCP_GET( pool, NET_1 ), abonent );
    datmcp_set_byte( varmcp_eeprom, DATMCP_GET( pool, EEPROM ), abonent ); // вот это ошибка записи
    datmcp_set_byte( varmcp_cicle, DATMCP_GET( pool, ERR_CICLE ), abonent );
    datmcp_set_byte( varmcp_differ_1, DATMCP_GET( pool, DIFFER_1 ), abonent );
    datmcp_set_byte( varmcp_differ_2, DATMCP_GET( pool, DIFFER_2 ), abonent );
    datmcp_set_byte( varmcp_differ_3, DATMCP_GET( pool, DIFFER_3 ), abonent );
    datmcp_set_byte( varmcp_mcp_have_err, DATMCP_GET( pool, HAVE_MI_ERR ), abonent );
    if ( datmcp_get_byte(varmcp_mcp_have_err, abonent)!=0){
    datmcp_set_byte( varmcp_mcp_num_err, pool>>12, abonent);
    }
    else {
        datmcp_set_byte( varmcp_mcp_num_err, 100, abonent);
    }

    //datmcp_set_byte( varmcp_red, !!( pool & DATMCP_RED ), abonent );
//    if ( ( datmcp_get_byte( varmcp_net0, abonent ))
//      && ( datmcp_get_byte( varmcp_red, abonent ) == 0 )
//      && ( datmcp_get_byte( varmcp_net1, abonent )) ){
//        datmcp_set_byte( varmcp_red, 1, abonent );
//    }

}

/** \} */
