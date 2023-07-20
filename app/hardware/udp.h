/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  udp.h
 *  \brief  Задача сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef UDP_H
#define UDP_H

#ifdef  UDP_IMPLEMENT
#define UDP_EXT
#define UDP_EXT_INIT( dec, init ) \
	dec = init
#else
#define UDP_EXT extern
#define UDP_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \addtogroup udp
 *  \{
 */

/**
 *  \brief Количество сетей Ethernet.
 *  \hideinitializer
 */
#define NET_COUNT  2
#define IP_MASK_FORMAT  "192.168.%d.255"
#define IP_MASK_FORMAT_SNMP  "192.168.%d.%d"
#define IP_MASK_FORMAT_WIN  "192.168.%d.%d"

#define HEADER_ID__TST  SEND_PKF

/**
 *  \brief Очередь исходящих сетевых сообщений.
 */
UDP_EXT std::queue<MESSAGE_REMOTE> udp_t_queue;

/**
 *  \brief Мьютекс очереди исходящих сообщений.
 */
UDP_EXT_INIT( pthread_mutex_t udp_t_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#endif /* UDP_H */
