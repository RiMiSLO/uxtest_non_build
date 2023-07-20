/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  test-net.h
 *  \brief  Тест сети Ethernet.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef TEST_NET_H
#define TEST_NET_H

#ifdef  TEST_NET_IMPLEMENT
#define TEST_NET_EXT
#define TEST_NET_EXT_INIT( dec, init ) \
	dec = init
#else
#define TEST_NET_EXT extern
#define TEST_NET_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup test_net Тест сети Ethernet
 *  \ingroup app
 *  \{
 */

#define TEST_NET_RATE_MIN  1    /**< Минимальная частота передачи сообщений. \hideinitializer */
#define TEST_NET_RATE_MAX  100  /**< Максимальная частота передачи сообщений. \hideinitializer */

/**
 *  \brief Данные об обмене пакетами со станцией.
 */
typedef struct {
    dword num [ NET_COUNT ];
    dword miss[ NET_COUNT ];
} TEST_NET_DATA_ITEM;

/**
 *  \brief Данные об обмене пакетами со всеми станциями.
 */
typedef struct {
    TEST_NET_DATA_ITEM st[ MAX_QMRMO ];
} TEST_NET_DATA;

/**
 *  \brief Тестовый пакет.
 */
typedef struct {
	word rate;
	word __;
	struct TNETSTATISTIC stat[ NET_COUNT ];
    TEST_NET_DATA_ITEM st[ MAX_QMRMO ];

} TEST_NET_PACKET;

/**
 *  \brief Сетевая статистика.
 */
typedef struct {
	struct TNETSTATISTIC stat[ NET_COUNT ];
} NET_STAT_PACKET;

/**
 *  \brief Результаты теста.
 */
TEST_NET_EXT TEST_NET_DATA test_net_result[ MAX_QMRMO ];

/**
 *  \brief Обобщенные счетчики.
 */
TEST_NET_EXT TEST_NET_DATA_ITEM test_net_result_short[ MAX_QMRMO ];

/**
 *  \brief Признаки обновления информации.
 */
TEST_NET_EXT bool test_net_renewed[ MAX_QMRMO ];

#define NET_STAT_COUNT  ( sizeof( struct TNETSTATISTIC ) / sizeof( _DWORD ))

/**
 *  \brief Сетевая статистика.
 */
typedef struct {
	union {
		struct TNETSTATISTIC stat[ NET_COUNT ];
		_DWORD data[ NET_COUNT ][ NET_STAT_COUNT ];
	};
} NET_STAT;

TEST_NET_EXT NET_STAT net_stat[ MAX_QMRMO ];

/**
 *  \brief Очередь входящих сетевых сообщений.
 */
TEST_NET_EXT std::queue<MESSAGE_REMOTE> test_net_queue;

/**
 *  \brief Мьютекс очереди входящих сообщений.
 */
TEST_NET_EXT_INIT( pthread_mutex_t test_net_mutex, PTHREAD_MUTEX_INITIALIZER );

/**
 *  \brief Мьютекс результатов тестирования сети.
 */
TEST_NET_EXT_INIT( pthread_mutex_t test_net_mutex_result, PTHREAD_MUTEX_INITIALIZER );

/**
 *  \brief Извлечение сетевой статитситки из сообщения.
 *  \param data Указатель на данные сообщения.
 *  \param abonent Номер абонента.
 */
TEST_NET_EXT void net_get_stat( NET_STAT_PACKET *data, int8_t abonent );

/**
 *  \brief Извлечение сетевой статитситки из сообщения МИ.
 *  \param data Указатель на данные сообщения.
 *  \param abonent Номер абонента.
 */
TEST_NET_EXT void net_get_stat2( struct TNETSTATISTIC *data, int8_t abonent, uint8_t num);

/** \} */

#endif /* TEST_NET_H */
