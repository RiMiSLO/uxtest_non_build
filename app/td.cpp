/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  td.cpp
 *  \brief  Операции с технологическим диском.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define TD_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup td
 *  \{
 */

static const int td_watch[] = {
	var_td_operate,
	var_COUNT
};

#define IDLE_DELAY  500  /* мс */

static int __operate;                             /**< Операция. */
static word dly;                                  /**< Задержка исполнения. */

static ST_MASK __st_mask;                         /**< Маска задействованных станций. */
static ST_MASK __st_ok;                           /**< Маска подтверждений операции. */
static ST_MASK __st_error;                        /**< Маска ошибок. */
static FILE *__disk_td;

static struct DISKMEM td_block;                   /**< Буфер передачи блоков. */
static const _DWORD *td_block_data = ( _DWORD *) td_block.byData;

static struct timeval __confirm_to;               /**< Таймер приема подтверждения. */

#define __CONFIRM_TO_LIMIT  2                     /**< Кол-во таймутов вызывающее ошибку. */
static int __confirm_to_cnt;

static bool __wrong_td = false;                   /**< Признак ошибки контольной суммы ТД. */

static MESSAGE_REMOTE *__queue_front( void );
static void __queue_pop( void );

static char __s[ 256 ];
static struct timeval __t_start, __t_stop, __t_delta;

/**
 *  \brief Инициализация задачи.
 */
int td_init( void )
{
	data_watch( peek_td, td_watch );
	__operate = TD_NONE;
	dly = IDLE_DELAY;

	return ( CO_READY );  /* CO_SKIP */
}

/**
 *  \brief Деинициализация задачи.
 */
void td_uninit( void )
{
}

/**
 *  \brief Задача - работа с ТД.
 */
CORO_DEFINE( td )
{
	static struct timeval t;
	static MESSAGE_REMOTE* mg;
	static uint8_t wait;
	static size_t size;
	static _DWORD chsum;
	static int percent;

	CORO_BEGIN();

	CORO_WAIT( datn_get_dword( varn_verify, st_tst ) != 0, );
	/* Пауза */
	t = clock_t_set( ini_report_start_delay );
	CORO_WAIT( clock_time_after( t ), );

//	__disk_td = fopen( ini_at_home( encode_system( __s, &ini_disk_td, sizeof( __s ))), "rb" );
    __disk_td = fopen( ini_at_home( ini_disk_td.toUtf8()), "rb" );
	if ( __disk_td != NULL ) {
		data_set_byte( var_td_check, TEST_PERFORM );
		/* Считать первый блок */
		if ( fread( td_block.byData, DISK_BLOCK_SIZE, 1, __disk_td )) {
			PBOOTSECTOR boot = ( PBOOTSECTOR ) td_block.byData;
			size = boot->dwSizeFileSystem - BEGIN_FILE_SYSTEM;
			chsum = boot->dwCheckSum;
		} else {  /* Образ пуст */
			ERROR( file_is_empty, ini_at_home( encode_console( __s, &ini_disk_td, sizeof( __s ))));
            dbg_td = 1;
            data_set_byte( var_td_check, TEST_FAILED );
		}
		CORO_YIELD();
		/* Переход к началу файловой системы */
		fseek( __disk_td, BEGIN_FILE_SYSTEM * DISK_BLOCK_SIZE, SEEK_SET );
		/* Подсчет контрольной суммы */
		while ( fread( td_block.byData, DISK_BLOCK_SIZE, 1, __disk_td )) {
			for ( uint32_t i = 0; i < ( DISK_BLOCK_SIZE / sizeof( _DWORD )); i++ ) chsum -= td_block_data[ i ];
			--size;
			CORO_YIELD();
		}
		/* Проверка контрольной суммы и размера */
		__wrong_td = ( chsum != 0 ) || ( size != 0 );
		data_set_byte( var_td_check, ( __wrong_td ) ? TEST_FAILED : TEST_PASSED );
        if (__wrong_td)
            dbg_td = 2;
		if ( __wrong_td ) ERROR( disk_td_chsum );
		/* Закрыть файл */
		fclose( __disk_td );
	} else {  /* Нет образа */
		ERROR( file_open, ini_at_home( encode_console( __s, &ini_disk_td, sizeof( __s ))), strerror( errno ));
        dbg_td = 3;
        data_set_byte( var_td_check, TEST_FAILED );
	}
	gui_send_event( EVENT_TD_VERIFY );

	for ( ; ; ) {
		/* Работаем по таймеру */
		if ( dly ) {
			t = clock_t_set( dly );
			CORO_WAIT( clock_time_after( t ), );
		} else {
			CORO_YIELD();
		}
		/* Обработка сообщений */
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
		/* Неверная контольная сумма образа */
		if ( __wrong_td ) continue;
		/* Запуск операции */
		if ( data_get_changed( peek_td, var_td_operate )) {
			if ( __operate == TD_NONE ) {
				__operate = data_get_byte( var_td_operate );
				if ( __operate != TD_NONE ) {
					/* Открыть файл ТД */
                    //__disk_td = fopen( ini_at_home( encode_system( __s, &ini_disk_td, sizeof( __s ))), "rb" );
                    __disk_td = fopen( ini_at_home(encode_utf8( __s, &ini_disk_td, sizeof( __s ))), "rb" );
					if ( __disk_td != NULL ) {
						__st_ok = __st_mask = data_get_dword( var_td_mask );
						__st_error = 0;
						td_block.dwAdressBlock = ( dword ) -1;
						/* Размер образа */
						fseek( __disk_td, 0, SEEK_END );
						size = ftell( __disk_td ) / DISK_BLOCK_SIZE;
						fseek( __disk_td, 0, SEEK_SET );
						percent = 0;
						dly = 0;
						gettimeofday( &__t_start, NULL );
					} else {  /* Нет образа ТД */
						ERROR( file_open, ini_at_home( encode_console( __s, &ini_disk_td, sizeof( __s )))
						, strerror( errno ));
						data_set_byte( var_td_operate, TD_NONE );
						data_set_dword( var_td_mask, BIT( st_tst ));  /* Бит st_tst - ошибка образа */
						__operate = TD_NONE;
					}
				}
			} else {
				if (( __operate == TD_CHECK ) && ( data_get_byte( var_td_operate ) == TD_NONE )) {
					/* Прервать проверку ТД */
					__operate = TD_NONE;
					data_set_dword( var_td_mask, _MAXDWORD );
					data_set_int( var_td_percent, 0 );
					/* Сбросить задержки */
					dly = IDLE_DELAY;
					/* Закрыть файл */
					fclose( __disk_td );
				}
			}
			data_reset( peek_td, var_td_operate );
			gui_send_event( EVENT_TD_OPERATE );
		}
		/* Операция */
		if ( __operate != TD_NONE ) {
			if ( __st_mask ) {  /* Есть станции - получатели */
				if ( __st_ok == __st_mask ) {  /* Получены все подтверждения */
					/* Считать блок */
					if ( fread( td_block.byData, DISK_BLOCK_SIZE, 1, __disk_td )) {
						/* Отправить блок */
						++td_block.dwAdressBlock;
						message_transmit( SEND_ALL, CMD_EX, __st_mask
						, ( __operate == TD_WRITE ) ? CMD_TD_WRITE_BLOCK : CMD_TD_CHECK_BLOCK
						, sizeof( td_block ), &td_block, false, &wait );
						/* Ожидание отправки сообщения */
						CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
						/* Процент выполнения */
						int p = ( td_block.dwAdressBlock * 100 ) / size;
						if ( p > percent ) {
							percent = p;
							data_set_int( var_td_percent, percent );
						}
						__st_ok = 0;
						/* Сброс таймаута */
						__confirm_to = clock_t_set( ini_td_confirm );
						__confirm_to_cnt = 0;
					} else {
						if ( td_block.dwAdressBlock == ( dword ) -1 ) {  /* Файл пуст */
							ERROR( file_is_empty, ini_at_home( encode_console( __s, &ini_disk_td, sizeof( __s ))));
							SET_VAR( __st_error, BIT( st_tst ));  /* Бит st_tst - ошибка образа */
						}
						__operate = TD_NONE;
					}
				} else {  /* Не все подтверждения */
					/* Проверка таймаута */
					if ( clock_time_after( __confirm_to )) {
						ST_MASK err = ~__st_ok & __st_mask;
						if ( ++__confirm_to_cnt < __CONFIRM_TO_LIMIT ) {
							/* Повторная передача */
							message_transmit( SEND_ALL, CMD_EX, err
							, ( __operate == TD_WRITE ) ? CMD_TD_WRITE_BLOCK : CMD_TD_CHECK_BLOCK
							, sizeof( td_block ), &td_block, false, &wait );
							/* Ожидание отправки сообщения */
							CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
							__confirm_to = clock_t_set( ini_td_confirm );
						} else {
							SET_VAR( __st_error, err );
							__st_mask = __st_ok;
						}
					}
				}
			} else {  /* Нет получателей */
				__operate = TD_NONE;
			}
			if ( __operate == TD_NONE ) {  /* Операция завершена */
				data_set_byte( var_td_operate, TD_NONE );
				data_set_dword( var_td_mask, __st_error );
				/* Сбросить задержки */
				dly = IDLE_DELAY;
				gettimeofday( &__t_stop, NULL );
				timersub( &__t_start, &__t_stop, &__t_delta );
				data_set_int( var_td_percent, __t_delta.tv_sec );
				/* Закрыть файл */
				fclose( __disk_td );
				gui_send_event( EVENT_TD_OPERATE );
			}
		}
		CORO_YIELD();
	}
	CORO_END();
}

static MESSAGE_REMOTE *__queue_front( void )
{
	MESSAGE_REMOTE *td_result = NULL;

	pthread_mutex_lock( &td_mutex );

	if ( !td_queue.empty()) {
		td_result = &td_queue.front();
	}
	pthread_mutex_unlock( &td_mutex );

   return ( td_result );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &td_mutex );

	td_queue.front().free_data();
	td_queue.pop();

	pthread_mutex_unlock( &td_mutex );
}

/** \} */
