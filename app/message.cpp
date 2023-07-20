/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  message.cpp
 *  \brief  Сообщение сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define MESSAGE_IMPLEMENT
#include "include.h"

static void message_transmit_net( int net, int abonent, uint32_t cmd, uint32_t mask, uint32_t ex_cmd, uint32_t sz
, void *data, bool in_heap, uint8_t *transmitted )
{
	MESSAGE_REMOTE mg;

	mg.net = net;
	mg.abonent = abonent;
	mg.cmd = cmd;
	if ( cmd == CMD_EX ) {
		mg.mask = mask;
		mg.ex_cmd = ex_cmd;
	}
	if ( in_heap || ( sz > sizeof( mg.dw_data ))) {
		mg._short = 0;
		mg.data = data;
		mg.len = ( in_heap ) ? sz : -sz;
	} else {
		mg._short = 1;
		memcpy( &mg.dw_data, data, sz );
		mg.len = -sz;
	}
	mg.init_transmitted( transmitted );


    pthread_mutex_lock( &udp_t_mutex );
    udp_t_queue.push( mg );
    pthread_mutex_unlock( &udp_t_mutex );
}

void message_transmit( int abonent, uint32_t cmd, uint32_t mask, uint32_t ex_cmd, uint32_t sz, void *data
, bool in_heap, uint8_t *transmitted )
{
    message_transmit_net( 0, abonent, cmd, mask, ex_cmd, sz, data, in_heap, transmitted );
}

void message_transmit2( int abonent, uint32_t cmd, uint32_t mask, uint32_t ex_cmd, uint32_t sz, void *data
, bool in_heap, uint8_t *transmitted )
{
    //mask <<= MI_ID_MIN;

	message_transmit_net( 2, abonent, cmd, mask, ex_cmd, sz, data, in_heap, transmitted );
}
