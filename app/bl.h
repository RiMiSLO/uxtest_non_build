/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  bl.h
 *  \brief  Операции с блоками УПО.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef BL_H
#define BL_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( BL_IMPLEMENT )
#include "h-begin.h"

/**
 *  \defgroup bl Операции с блоками УПО
 *  \ingroup app
 *  \{
 */

/**
 *  \brief Очередь входящих сетевых сообщений.
 */
_VAR( std::queue<MESSAGE_REMOTE> bl_queue );

/**
 *  \brief Мьютекс очереди входящих сообщений.
 */
_VAR_INIT( pthread_mutex_t bl_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#include "h-end.h"

#endif /* BL_H */
