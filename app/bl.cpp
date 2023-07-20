/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  bl.cpp
 *  \brief  Операции с блоками УПО.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define BL_IMPLEMENT
#include "include.h"
#include "cintelhex.h"

/**
 *  \addtogroup bl
 *  \{
 */

static const int bl_watch[] = {
	var_bl_addr,
	var_COUNT
};

#define IDLE_DELAY  500  /* мс */

static int __bl_addr;                             /**< Адрес блока. */
static word dly;                                  /**< Задержка передачи блоков. */

static ST_MASK __st_mask;                         /**< Маска задействованных станций. */
static ST_MASK __st_ok;                           /**< Маска подтверждений операции. */
static ST_MASK __st_error;                        /**< Маска ошибок. */

static struct FILEDAT __msg;                      /**< Сообщение. */
static struct timeval __confirm_to;               /**< Таймер приема подтверждения. */

static QString *__p_file;                         /**< Имя файла программы. */
static size_t __p_size;                           /**< Размер программы. */
static uint8_t *__p_buf;                          /**< Буфер программы. */
static uint32_t __p_rest, __p_seek;

#define __CONFIRM_TO_LIMIT  2                     /**< Кол-во таймутов вызывающее ошибку. */
static int __confirm_to_cnt;

static MESSAGE_REMOTE *__queue_front( void );
static void __queue_pop( void );

static char __s[ 256 ];

/**
 *  \brief Инициализация задачи.
 */
int bl_init( void )
{
	data_watch( peek_td, bl_watch );
	__bl_addr = 0;
	dly = IDLE_DELAY;

	return ( CO_READY );  /* CO_SKIP */
}

/**
 *  \brief Деинициализация задачи.
 */
void bl_uninit( void )
{
}

/**
 *  \brief Задача - работа с блокоми УПО.
 */
CORO_DEFINE( bl )
{
	static struct timeval t;
	static MESSAGE_REMOTE* mg;
	static uint8_t wait;
	static int percent;
    static int bl_app_id = 0;

	CORO_BEGIN();
	CORO_WAIT( datn_get_dword( varn_verify, st_tst ) != 0, );

	for ( ; ; ) {

        // Работаем по таймеру

		if ( dly ) {
			t = clock_t_set( dly );
			CORO_WAIT( clock_time_after( t ), );
		} else {
			CORO_YIELD();
		}

        //------------------------------------------
        // Обработка сообщений

		mg = __queue_front();
		while ( mg ) {
			ST_MASK ab;

			switch ( mg->cmd ) {

			case CMD_RESULT:
				/* Принят результат операции */
				ab = BIT( mg->abonent );
				if (( ab & __st_mask ) == 0 ) break;  /* Абонента уже отбросили... */

				if ( *( byte *) mg->get_data() == END_OK ) {
					SET_VAR( __st_ok, ab );
				} else {
					SET_VAR( __st_error, ab );
					CLEAR_VAR( __st_mask, ab );
				}
				break;
			}
			__queue_pop();
			mg = __queue_front();
		}

        //------------------------------------------
        // Запуск операции

		if ( data_get_changed( peek_td, var_bl_addr )) {
            // Изменился режим программирования блока

			if ( __bl_addr == 0 ) {
				__bl_addr = data_get_byte( var_bl_addr );
				if ( __bl_addr ) {
                    // Начало операции

					bool ok = false;
					uint16_t crc;
					int i;

                    // Поиск ПО для блока
					__p_file = NULL;
					for ( i = 0; i < bl_count; i++ ) {
						if ( bl_list[ i ].addr == __bl_addr ) {
							if ( bl_list[ i ].hex < map_hex_count ) {
								__p_file = &map_hex[ bl_list[ i ].hex ];
                                bl_app_id = i;
							}
							break;
						}
					}

					if ( __p_file ) {
                        // Чтение файла программы

						FILE *f = fopen( ini_at_home( encode_system( __s, __p_file, sizeof( __s ))), "rb" );
						if ( f != NULL ) {
							__st_ok = __st_mask = st_host_mask;
							__st_error = 0;
							/* Размер образа */
							fseek( f, 0, SEEK_END );
							__p_size = ftell( f );

							fseek( f, 0, SEEK_SET );
							if ( __p_size ) {
                                // Файл не нулевого размера

								char *buf = ( char *) malloc( __p_size );
								if ( fread( buf, 1, __p_size, f ) == __p_size ) {
                                    // Файл прочитан

                                    ok = true;
                                    __p_buf = (uint8_t *) malloc(__p_size);
                                    memcpy(( void *)__p_buf, buf, __p_size);
/*
									ihex_recordset_t *rs = ihex_rs_from_string( buf );
									if ( ihex_errno() == 0 ) {
										__p_size = ihex_rs_get_size( rs );
										if ( __p_size ) {
											__p_buf = ( uint8_t *) malloc( __p_size + sizeof( crc ));
											ihex_mem_copy( rs, ( void *) __p_buf, __p_size, 0x08000000
											, IHEX_WIDTH_8BIT, IHEX_ORDER_BIGENDIAN );
											if ( ihex_errno() == 0 ) {
                                                //!? len( hex ) - len( LR_IROM1 ) == 6
												crc = CRC16( __p_buf, __p_size - 6 );
												memcpy( __p_buf + __p_size, &crc, sizeof( crc ));
												__p_size += sizeof( crc );
												ok = true;
											} else {
												ERROR( ihex, ihex_error());
											}
											ihex_rs_free( rs );
										} else {
											ERROR( file_is_empty, ini_at_home( encode_console( __s, __p_file, sizeof( __s ))));
										}
									} else {
										ERROR( ihex, ihex_error());
									}
 */





								} else {
									ERROR( file_read, ini_at_home( encode_console( __s, __p_file, sizeof( __s ))));
								}
								free( buf );
							} else {
								ERROR( file_is_empty, ini_at_home( encode_console( __s, __p_file, sizeof( __s ))));
							}
							fclose( f );
						} else {  /* Нет файла программы */
							ERROR( file_open, ini_at_home( encode_console( __s, __p_file, sizeof( __s )))
							, strerror( errno ));
						}
					} else {
						ERROR( no_hex_name, __bl_addr );
					}

					if ( ok ) {
                        // Инициализация операции
//__p_size = 6000;
						__msg.wAdr = __bl_addr;
						__p_rest = __p_size;
						__p_seek = 0;
						percent = 0;
                        dly = 0;

                        printf(__CRLF__ "Sending program %s (0x%04X), addr %d..."__CRLF__"0%%\r"
                            , ini_at_home( encode_console( __s, __p_file, sizeof( __s ))), crc, __bl_addr );

						journal_freeze_time();
						journal_printf( "", "Передача ПО %s (0x%04X) для блока УПО %d."
                            , ini_at_home( encode_utf8( __s, __p_file, sizeof( __s ))), crc, __bl_addr );

					} else {
                        // Нет операции
						data_set_byte( var_bl_addr, 0 );
						__bl_addr = 0;
					}
				}
			}
			data_reset( peek_td, var_bl_addr );
		}

        //------------------------------------------
        // Операция

		if ( __bl_addr ) {
            if ( __st_mask ) {
                // Есть станции - получатели

                if ( __st_ok == __st_mask ) {
                    // Получены все подтверждения

                    // Отправить новую часть
					if ( __p_rest ) {
						uint32_t chunk = __p_rest;
						if ( chunk < sizeof( __msg.byData )) {
							memset( __msg.byData + chunk, 0, sizeof( __msg.byData ) - chunk );
						} else {
							chunk = sizeof( __msg.byData );
						}
						__msg.dwLenData = __p_rest;
						__msg.dwSeekSet = __p_seek;
						memcpy( __msg.byData, __p_buf + __p_seek, chunk );
                        message_transmit( SEND_ALL, CMD_EX, __st_mask, (bl_app_id << 8) + CMD_BL_PROG, sizeof( __msg )
                            , &__msg, false, &wait );

						__p_rest -= chunk;
						__p_seek += chunk;

                        // Ожидание отправки сообщения
						CORO_WAIT( wait != MESSAGE_IN_QUEUE, );

                        // Процент выполнения
						int p = ( __p_seek * 100 ) / __p_size;
						if ( p > percent ) {
							percent = p;
							printf( "%d%%    \r", percent );
						}
						__st_ok = 0;

                        // Сброс таймаута
						__confirm_to = clock_t_set( ini_bl_confirm );
						__confirm_to_cnt = 0;
					} else {
                        // Больше нечего отправлять - завершить операцию
						__bl_addr = 0;
					}
                } else {
                    // Не все подтверждения

                    // Проверка таймаута
					if ( clock_time_after( __confirm_to )) {
						ST_MASK err = ~__st_ok & __st_mask;
						if ( ++__confirm_to_cnt < __CONFIRM_TO_LIMIT ) {
                            // Повторная передача
							message_transmit( SEND_ALL, CMD_EX, err, CMD_BL_PROG, sizeof( __msg )
                                , &__msg, false, &wait );
                            printf( __CRLF__ "retry send part" __CRLF__ );

                            // Ожидание отправки сообщения
							CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
							__confirm_to = clock_t_set( ini_bl_confirm );
						} else {
							__st_error = err;
							__st_mask = __st_ok;
						}
					}
				}
            } else {
                // Нет получателей
				__bl_addr = 0;
			}

            if ( __bl_addr == 0 ) {
                // Операция завершена

				data_set_byte( var_bl_addr, 0 );
                free( __p_buf );

                // Сбросить задержки
                dly = IDLE_DELAY;

				journal_freeze_time();
				if ( __st_error == 0 ) {
					printf( __CRLF__ "OK" __CRLF__ );
					journal_print( "", "Передача ПО завершена успешно." );
					gui_send_event( EVENT_BL_PROG_OK );
				} else {
					st_mask2string( __st_error, __s, 0, sizeof( __s ));
					printf( __CRLF__ "FAILED" __CRLF__ );
					journal_printf( "", "Передача ПО завершена, неисправность: %s", __s );
					gui_send_event( EVENT_BL_PROG_ERROR );
				}
			}
		}

        //------------------------------------------

		CORO_YIELD();
	}
	CORO_END();
}

static MESSAGE_REMOTE *__queue_front( void )
{
	MESSAGE_REMOTE *result = NULL;

	pthread_mutex_lock( &bl_mutex );

	if ( !bl_queue.empty()) {
		result = &bl_queue.front();
	}
	pthread_mutex_unlock( &bl_mutex );

   return ( result );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &bl_mutex );

	bl_queue.front().free_data();
	bl_queue.pop();

	pthread_mutex_unlock( &bl_mutex );
}

/** \} */
