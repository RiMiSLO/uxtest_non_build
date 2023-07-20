/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  td.h
 *  \brief  Операции с технологическим диском.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef TD_H
#define TD_H

#ifdef  TD_IMPLEMENT
#define TD_EXT
#define TD_EXT_INIT( dec, init ) \
	dec = init
#else
#define TD_EXT extern
#define TD_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup td Операции с технологическим диском
 *  \ingroup app
 *  \{
 */

#define TD_NONE   0
#define TD_WRITE  1
#define TD_CHECK  2
#define TD_OK     3
#define TD_ERROR  4
#define TD_ABORT  5

/**
 *  \brief Очередь входящих сетевых сообщений.
 */
TD_EXT std::queue<MESSAGE_REMOTE> td_queue;

/**
 *  \brief Мьютекс очереди входящих сообщений.
 */
TD_EXT_INIT( pthread_mutex_t td_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#endif /* TD_H */
