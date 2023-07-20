/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  udp-desc.h
 *  \brief  Задача сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef UDP_DESC_H
#define UDP_DESC_H

#ifdef  UDP_DESC_IMPLEMENT
#define UDP_DESC_EXT
#define UDP_DESC_EXT_INIT( dec, init ) \
	dec = init
#else
#define UDP_DESC_EXT extern
#define UDP_DESC_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup udp Сетевой обмен
 *  \ingroup app
 *  \{
 */

/**
 *  \brief Номер порта для приема (UDP).
 *  \hideinitializer
 */
#define udp_port_in_DEFAULT  4950

/**
 *  \brief Номер порта для передачи (UDP).
 *  \hideinitializer
 */
#define udp_port_out_DEFAULT  4949

/**
 *  \brief Интервал проверки сообщений (мкс).
 *  \hideinitializer
 */
#define udp_delay_usec_DEFAULT  50

/**
 *  \brief Задержка инициализации сети при старте программы (мс).
 *  \hideinitializer
 */
#define udp_start_delay_DEFAULT  1000

/**
 *  \brief Таймаут связи со станциями (мс).
 *  \hideinitializer
 */
#define udp_online_DEFAULT  3000 // было 2000

/**
 *  \brief Номер подсети 1.
 *  \hideinitializer
 */
#define udp_net1_DEFAULT  100

/**
 *  \brief Номер подсети 2.
 *  \hideinitializer
 */
#define udp_net2_DEFAULT  101

/**
 *  \brief Номер ПКФ в подсетях.
 *  \hideinitializer
 */
#define udp_host_DEFAULT  4

/**
  * \brief Номер ПКФ для snmp
  * \hideinitializer
  */
#define snmp_host_DEFAULT 7

/**
 *  \brief Номер подсети 1 для передачи и приема от МИ.
 *  \hideinitializer
 */
#define udp_net1_mi_DEFAULT  110

/**
 *  \brief Номер подсети 2 для передачи в МИ.
 *  \hideinitializer
 */
#define udp_net2_mi_DEFAULT  111

/**
 *  \brief Признак наличия подсети МИ.
 *  \hideinitializer
 */
#define net_mi_DEFAULT  true

/**
 *  \brief Интервал опроса станций (мс).
 *  \hideinitializer
 */
#define udp_poll_DEFAULT  100

/**
 *  \brief Слушаем ВУ при запуске (мс).
 *  \hideinitializer
 */
#define udp_poll_first_DEFAULT  3000

/**
 *  \brief Интервал опроса МИ (мс).
 *  \hideinitializer
 */
#define udp_poll_mi_DEFAULT  200

/**
 *  \brief Интервал опроса при штатной работе (мс).
 *  \hideinitializer
 */
#define udp_poll_operate_DEFAULT  1000

/**
 *  \brief Интервал запроса статистики (сек).
 *  \hideinitializer
 */
#define udp_stat_request_sec_DEFAULT  30

/**
 *  \brief Номер порта для передачи протокола (UDP).
 *  \hideinitializer
 */
#define udp_port_prot_DEFAULT  8632

/**
 *  \brief IP адрес передачи протокола.
 *  \hideinitializer
 */
#define udp_ip_prot_DEFAULT  "127.0.0.1"

/** \} */

#endif /* UDP_DESC_H */
