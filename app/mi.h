/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  mi.h
 *  \brief  Операции с МИ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef MI_H
#define MI_H

#ifdef  MI_IMPLEMENT
#define MI_EXT
#define MI_EXT_INIT( dec, init ) \
	dec = init
#else
#define MI_EXT extern
#define MI_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup mi Операции с МИ
 *  \ingroup app
 *  \{
 */

#define MI_NONE         0
#define MI_WRITE_GRAD   1
#define MI_WRITE_ID     2
#define MI_OK           3
#define MI_ERROR        4
#define MI_MAX_COUNT    8

#define MCP_NONE        0
#define MCP_WRITE_GRAD  1
#define MCP_WRITE_ID    2
#define MCP_OK          3
#define MCP_ERROR       4
#define STAGE_READING   5


#pragma pack( 1 )

typedef struct {
	WORD   wSn;
	WORD   wCSComp;
	_DWORD dwDateComp;
	_DWORD dwTimeComp;
} MI_COMP;

#pragma pack()

/**
 *  \brief Список коррект. кодов МИ.
 */
MI_EXT MI_COMP mi_comp[ MAX_QMRMO ];


/**
 *  \brief Число элементов в списке коррект. кодов МИ.
 */
MI_EXT uint8_t mi_comp_count;

/**
 *  \brief Число элементов в списке коррект. кодов МИ.
 */
MI_EXT uint8_t mcp_comp_count;

/**
 *  \brief Очередь входящих сетевых сообщений.
 */
MI_EXT std::queue<MESSAGE_REMOTE> mi_queue;

/**
 *  \brief Мьютекс очереди входящих сообщений.
 */
MI_EXT_INIT( pthread_mutex_t mi_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#endif /* MI_H */
