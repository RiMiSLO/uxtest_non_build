/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  test-net.cpp
 *  \brief  Тест сети Ethernet.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define TEST_NET_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup test_net
 *  \{
 */

static const int test_net_watch[] = {
	var_test_net_command,
	var_test_net_rate,
	var_COUNT
};

#define __started_PAUSE  1  /**< Пауза при запуске (сек) */

static word dly;                                      /**< Задержка передачи тестовых сообщений. */
static uint32_t __started;                            /**< Признак - тест выполняется. */
static TEST_NET_PACKET test_net_packet;               /**< Буфер передачи тестовых сообщений. */
static dword __last_num[ DATN_DEPTH ][ NET_COUNT ];   /**< Номера последних принятых сообщений. */

static MESSAGE_REMOTE *__queue_front( void );
static void __queue_pop( void );

static TEST2 cmd_test;  /* Буфер для команды */

#define M_RESULT     "Тест сети %d: "
#define M_RESULT_1   "Нет принятых сообщений."
#define M_RESULT_2   "Потеряно %.2f%% сообщений."
#define M_RESULT_3   "Количество принятых сообщений по сетями отличается на %.2f%%."
#define M_RESULT_4   "Не передано ни одного сообщения."

static void __set_delay( word rt )
{
	if (( rt < TEST_NET_RATE_MIN ) || ( rt > TEST_NET_RATE_MAX )) return;

	test_net_packet.rate = rt;
	dly = ( 1000 / rt );
}

/**
 *  \brief Инициализация задачи.
 */
int test_net_init( void )
{
	data_watch( peek_test_net, test_net_watch );
	memset( &test_net_packet, 0, sizeof( test_net_packet ));
	memset( &net_stat, 0xFF, sizeof( net_stat ));
	__set_delay( TEST_NET_RATE_MIN );
	__started = 0;

	return ( CO_READY ); /* THR_SKIP */
}

/**
 *  \brief Деинициализация задачи.
 */
void test_net_uninit( void )
{
}

/**
 *  \brief Задача - тест сети Ethernet.
 */
CORO_DEFINE( test_net )
{
	static struct timeval t;
	static MESSAGE_REMOTE* mg;
	static uint8_t wait;
    static uint8_t rt;

	CORO_BEGIN();

	for ( ; ; ) {
		/* Работаем по таймеру */
		t = clock_t_set( dly );
		CORO_WAIT( clock_time_after( t ), );

		/* Обработка сообщений */
		mg = __queue_front();
		while ( mg ) {
			if (( mg->cmd == CMD_TEST_NET ) && ( mg->net <= 2 )) {
				dword *num;
				int net = mg->net;
				TEST_NET_PACKET *data = ( TEST_NET_PACKET *) mg->get_data();
				int ab = mg->abonent;

				/* Обработка тестовых сообщений */
                if ( net == 2 ) {
                    /* МИ */
                    num = &test_net_result[ ab ].st[ ab ].num[ 0 ];
                    net = ( *num == data->st[ ab ].num[ 0 ]) ? 1 : 0;
				} else {
					num = &test_net_result[ ab ].st[ ab ].num[ net ];
				}
				/* Результаты абонента */
				pthread_mutex_lock( &test_net_mutex_result );
				/* Обработка статистики */
				for ( int i = 0; i < ini_net_count; i++ ) {
					net_stat[ ab ].stat[ i ] = data->stat[ i ];
				}
				for ( int i = 0; i < ini_net_count; i++ ) {
					test_net_result_short[ ab ].miss[ i ] = 0;
					test_net_result_short[ ab ].num [ i ] = 0;
				}
				/* Копировать данные, кроме кол-ва отправленных сообщений */
				for ( int i = 0; i < st_count; i++ ) {
					if ( i == ab ) continue;
					for ( int ii = 0; ii < ini_net_count; ii++ ) {
						dword miss = data->st[ i ].miss[ ii ];
						dword n = data->st[ i ].num [ ii ];

						test_net_result[ ab ].st[ i ].miss[ ii ] = miss;
						test_net_result[ ab ].st[ i ].num [ ii ] = n;
						test_net_result_short[ ab ].miss[ ii ] += miss;
                        test_net_result_short[ ab ].num [ ii ] += n;
					}
                }
                /* Копировать кол-во отправленных */
				for ( int i = 0; i < ini_net_count; i++ ) {
                    test_net_result[ ab ].st[ ab ].num[ i ] = data->st[ ab ].num[ i ];
                }
                test_net_renewed[ ab ] = true;
				/* Собственные результаты */
				if ( *num > __last_num[ ab ][ net ]) {
					dword delta = ( *num - __last_num[ ab ][ net ] - 1 );  /* Потеряно */
                    test_net_result[ st_tst ].st[ ab ].miss[ net ] += delta;
                    test_net_result_short[ st_tst ].miss[ net ] += delta;  /* Потеряно всего */
                    test_net_result[ st_tst ].st[ ab ].num[ net ] = *num;  /* Должно быть принято от абонента */
					test_net_result_short[ st_tst ].num[ net ]+= delta + 1;  /* Должно быть принято всего */
					__last_num[ ab ][ net ] = *num;
				}
				test_net_renewed[ st_tst ] = true;
				pthread_mutex_unlock( &test_net_mutex_result );
			}
			__queue_pop();
			mg = __queue_front();
		}
		/* Управление */
		if ( data_get_changed( peek_test_net, var_test_net_command )) {
			if ( data_get_byte( var_test_net_command )) {
				rt = data_get_byte( var_test_net_rate );
				/* Запуск теста */
				pthread_mutex_lock( &test_net_mutex_result );
				/* ВУ */
				memset( test_net_result, 0, sizeof( test_net_result ));
				memset( test_net_result_short, 0, sizeof( test_net_result_short ));
				memset( test_net_renewed, 0, sizeof( test_net_renewed ));
				memset( __last_num, 0, sizeof( __last_num ));
				test_net_renewed[ st_tst ] = true;
				/* МИ */
				pthread_mutex_unlock( &test_net_mutex_result );

				datn_set_word( varn_test_perform, PERFORM_SET( TEST, TEST_ETHERNET ), st_tst );
				datn_set_byte( varn_test_net, TEST_PERFORM, st_tst );
				datn_set_word( varn_failure
				, CLEAR_VALUE( datn_get_word( varn_failure, st_tst ), FAILURE_MASK( ETHERNET )), st_tst );
				/* Очистить очередь тестовых сообщений */
				while ( __queue_front()) __queue_pop();
				/* Отправить команду запуска теста */
				cmd_test.wTestNum = TEST_ETHERNET;
				cmd_test.wRepeat = 0;
				cmd_test.wParam[ 0 ] = rt;
				cmd_test.wParam[ 1 ] = ini_test_net_miss;
                message_transmit( SEND_ALL, CMD_EX, st_mask, EX_START_TEST, sizeof( TEST2 ), &cmd_test, false, &wait );
				/* Ожидание обработки сообщения */
				CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
                if ( mcp_count ) {
                    message_transmit2( SEND_MCP1, CMD_EX, 0x0010, EX_START_TEST_MI, sizeof( TEST2 ), &cmd_test );
                    message_transmit2( SEND_MCP2, CMD_EX, 0x0020, EX_START_TEST_MI, sizeof( TEST2 ), &cmd_test );
                    message_transmit2( SEND_MCP3, CMD_EX, 0x0040, EX_START_TEST_MI, sizeof( TEST2 ), &cmd_test );
                }
				__set_delay( rt );
				data_reset( peek_test_net, var_test_net_rate );
				__started = 1 + __started_PAUSE * rt;
			} else {
				/* Останов теста */
				dword transmitted, must_received;
				double missed;
				byte result = TEST_PASSED;
				/* Отправить команду */
				cmd_test.wTestNum = TEST_ETHERNET;
				cmd_test.wRepeat = 0;
                cmd_test.wParam[ 0 ] = 0;
				cmd_test.wParam[ 1 ] = 0;
                message_transmit( SEND_ALL, CMD_EX, _MAXDWORD, EX_STOP_TEST, sizeof( TEST2 ), &cmd_test, false, &wait );
				/* Ожидание обработки сообщения */
				CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
                if ( mcp_count ) {
                    //message_transmit2( SEND_BI, CMD_EX, _MAXDWORD, EX_STOP_TEST_MI, sizeof( TEST2 ), &cmd_test );
                    message_transmit2( SEND_MCP1, CMD_EX, 0x0010, EX_STOP_TEST_MI, sizeof( TEST2 ), &cmd_test );
                    message_transmit2( SEND_MCP2, CMD_EX, 0x0020, EX_STOP_TEST_MI, sizeof( TEST2 ), &cmd_test );
                    message_transmit2( SEND_MCP3, CMD_EX, 0x0040, EX_STOP_TEST_MI, sizeof( TEST2 ), &cmd_test );
               }
				/* Анализ результатов теста */
				pthread_mutex_lock( &test_net_mutex_result );
				journal_freeze_time();
				for ( int i = 0; i < ini_net_count; i++ ) {
					/* ВУ */
					must_received = test_net_result_short[ st_tst ].num[ i ];
					if ( must_received == 0 ) {
						/* Ничего не принято */
						result = TEST_FAILED;
						journal_printf( "", M_RESULT M_RESULT_1, i + 1 );
						continue;
					}
					missed = ( double ) test_net_result_short[ st_tst ].miss[ i ] * 100 / ( double ) must_received;
					if ( missed > ( double ) ini_test_net_miss ) {
						/* Потеряно много процентов) */
						result = TEST_FAILED;
						journal_printf( "", M_RESULT M_RESULT_2, i + 1, missed );
					}
					missed = ( double )( must_received - test_net_result_short[ st_tst ].num[ 0 ]) * 100 / ( double ) must_received;
					if ( missed < 0.0 ) missed = -missed;
					if ( missed > ( double ) ini_test_net_miss ) {
						/* Большая разница кол-ва принятых между сетями */
						result = TEST_FAILED;
						journal_printf( "", M_RESULT M_RESULT_3, i + 1, missed );
					}
					transmitted = test_net_result[ st_tst ].st[ st_tst ].num[ i ];
					if ( transmitted == 0 ) {
						/* Ничего не переданно */
						result = TEST_FAILED;
						journal_printf( "", M_RESULT M_RESULT_4, i + 1 );
					}
				}
				pthread_mutex_unlock( &test_net_mutex_result );
				datn_set_word( varn_test_perform, PERFORM_SET( TEST, NO_TEST ), st_tst );
				datn_set_byte( varn_test_net, result, st_tst );
				if ( result == TEST_FAILED ) {
					datn_set_word( varn_failure
					, SET_VALUE( datn_get_word( varn_failure, st_tst ), FAILURE_MASK( ETHERNET )), st_tst );
				}
				__set_delay( TEST_NET_RATE_MIN );
				__started = 0;
			}
			data_reset( peek_test_net, var_test_net_command );
		}
		/* Тест */
		if ( __started ) {
			if ( __started == 1 ) {
				/* Установка частоты */
				if ( data_get_changed( peek_test_net, var_test_net_rate )) {
					__set_delay( data_get_byte( var_test_net_rate ));
					data_reset( peek_test_net, var_test_net_rate );
				}
				/* Передача сообщения */
				/* ВУ */
				pthread_mutex_lock( &test_net_mutex_result );
				memcpy( test_net_packet.st, test_net_result[ st_tst ].st, sizeof( TEST_NET_DATA ));
				pthread_mutex_unlock( &test_net_mutex_result );

                message_transmit( SEND_ALL, CMD_TEST_NET, 0, 0
                , sizeof( TEST_NET_PACKET ), &test_net_packet, false, &wait );

                /* Ожидание обработки сообщения */
				CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
                if ( mcp_count ) {
                    /* МИ */
                    message_transmit2( SEND_BI, CMD_TEST_NET, 0, 0, sizeof( TEST_NET_PACKET ), &test_net_packet, false, &wait );
                    message_transmit2( SEND_MCP1, CMD_TEST_NET, 0, 0, sizeof( TEST_NET_PACKET ), &test_net_packet, false, &wait );
                    message_transmit2( SEND_MCP2, CMD_TEST_NET, 0, 0, sizeof( TEST_NET_PACKET ), &test_net_packet, false, &wait );
                    message_transmit2( SEND_MCP3, CMD_TEST_NET, 0, 0, sizeof( TEST_NET_PACKET ), &test_net_packet, false, &wait );
					/* Ожидание обработки сообщения */
                    CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
				}
				pthread_mutex_lock( &test_net_mutex_result );
				for ( int i = 0; i < ini_net_count; i++ ) {
					test_net_result[ st_tst ].st[ st_tst ].num[ i ]++;
				}
                test_net_renewed[ st_tst ] = true;
				pthread_mutex_unlock( &test_net_mutex_result );
			} else {
				/* Пауза перед началом передачи тестовых пакетов */
				--__started;
			}
		}
		CORO_YIELD();
	}
	CORO_END();
}

void net_get_stat( NET_STAT_PACKET *data, int8_t abonent )
{
	pthread_mutex_lock( &test_net_mutex_result );
	for ( int i = 0; i < ini_net_count; i++ ) {
		net_stat[ abonent ].stat[ i ] = data->stat[ i ];
	}
	pthread_mutex_unlock( &test_net_mutex_result );
}

void net_get_stat2( struct TNETSTATISTIC *data, int8_t abonent, uint8_t num)
{
    pthread_mutex_lock( &test_net_mutex_result );
    net_stat[ abonent ].stat[ num ] = *data;
    pthread_mutex_unlock( &test_net_mutex_result );
}

static MESSAGE_REMOTE *__queue_front( void )
{
	MESSAGE_REMOTE *mg = NULL;

	pthread_mutex_lock( &test_net_mutex );
	if ( !test_net_queue.empty()) {
		mg = &test_net_queue.front();
	}
	pthread_mutex_unlock( &test_net_mutex );

   return ( mg );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &test_net_mutex );
	test_net_queue.front().free_data();
	test_net_queue.pop();
	pthread_mutex_unlock( &test_net_mutex );
}

/** \} */
