/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  mi.cpp
 *  \brief  Операции с МИ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define MI_IMPLEMENT
#include "include.h"
#include <QDir>

/**
 *  \addtogroup mi
 *  \{
 */

static const int mi_watch[] = {
	var_mi_operate,
	var_COUNT
};

static const int mcp_watch[] = {
    var_mcp_operate,
    var_COUNT
};

#define IDLE_DELAY  500  /* мс */
#define WORK_DELAY  5    /* мс */

static int __operate;                             /**< Операция. */

#define STAGE_SENDING  0
#define STAGE_SENT     1
#define STAGE_WRITING  2
#define STAGE_WRITTEN  3
#define STAGE_READING 4
#define STAGE_READING_PROCESS 5

static int __stage;                               /**< Этап. */

static word dly;                                  /**< Задержка исполнения. */

static MI_MASK __mcp_mask;                         /**< Маска задействованных МИ - mcp. */
static MI_MASK __mcp_ok;                           /**< Маска подтверждений операции - mcp. */
static MI_MASK __mcp_error;                        /**< Маска ошибок - mcp. */

static QString __fn;
static FILE *__fd;
static struct FILERDDAT mcp_read_block;
static struct FILEWRDAT mcp_block;                 /**< Буфер передачи блоков. */
static struct FILEWRDAT mcp_block_print;                 /**< Буфер передачи блоков. */
FILEWRDAT* p_mcp_block_print;


static struct ROMOPER mi_cmd_eeprom;              /**< Команда записи в EEPROM */
static QString str0="";
static uchar snmp_buf[512];

#define BLOCK_SIZE  ( struct_field_size( FILEWRDAT, byData ))

static struct timeval __confirm_to;               /**< Таймер приема подтверждения. */

#define __CONFIRM_TO_LIMIT  2                     /**< Кол-во таймутов вызывающее ошибку. */
static int __confirm_to_cnt;

static MESSAGE_REMOTE *__queue_front( void );
static void __queue_pop( void );

#define M_FILE_ID    "Файл ИД МИ/МЦП: %s"
#define M_FILE_GRD   "Файл градуировок МИ/МЦП: %s"
#define M_NOT_FOUND  "(отсутствует)"
#define M_MORE_1     "(более одного в папке)"

static char __s[ 256 ], __s0[ 256 ];
static struct timeval __t_start, __t_stop, __t_delta;

/**
 *  \brief Инициализация задачи.
 */
int mi_init( void )
{
    //data_watch( peek_mi, mi_watch );
    data_watch( peek_mcp, mcp_watch);
	__operate = MI_NONE;
	dly = IDLE_DELAY;

    //mi_comp_count = 0;

	return ( CO_READY );  /* CO_SKIP */
}

/**
 *  \brief Деинициализация задачи.
 */
void mi_uninit( void )
{
}

/**
 *  \brief Задача - работа с МИ.
 */
CORO_DEFINE( mi )
{

	static struct timeval t;
	static MESSAGE_REMOTE* mg;
	static uint8_t wait;
    static size_t size;

    static FILEWRDAT* data;
    static uint8_t read_abonent, read_abonent_ok, read_abonent_err;

	CORO_BEGIN();

	CORO_WAIT( datn_get_dword( varn_verify, st_tst ) != 0, );
	/* Пауза */
	t = clock_t_set( ini_report_start_delay );
	CORO_WAIT( clock_time_after( t ), );

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
            int flag=0;
			MI_MASK ab;
			switch ( mg->cmd ) {

			case CMD_RESULT:
                /* Принят результат операции */
                mg->abonent -= SEND_MCP1;
                ab = BIT( mg->abonent ); // 1,2,3

                if (( ab & __mcp_mask ) == 0 ){
                    break;}  /* Абонента уже отбросили... */

				if ( *( byte *) mg->get_data() == END_OK ) {
                    SET_VAR( __mcp_ok, ab );
				} else {
                    SET_VAR( __mcp_error, ab );
                    CLEAR_VAR( __mcp_mask, ab );
				}
				break;

            case CMD_MI_FILE_REC:
//                flag=1;
////                printf("step2");
////                //void* b = mg->data;

//                p_mcp_block_print = (FILEWRDAT*) mg->get_data();

//                  printf("num table -> %d\n", p_mcp_block_print->wTblNum);
//                printf("указатель чтения -> %d\n", p_mcp_block_print->dwSeekSet);
//                printf("колличество считанныых байт -> %d\n", p_mcp_block_print->wLenData);
//                __stage = STAGE_READING_PROCESS;
                    //__stage == STAGE_READING;
                break;
			}
			__queue_pop();

            if(flag)
                break;
            else
                mg = __queue_front();


		}
		/* Запуск операции */
        if ( data_get_changed( peek_mcp, var_mcp_operate )) {
            if ( __operate == MCP_NONE ) {
                /* Стартовая пауза для обновления журнала */
                t = clock_t_set( IDLE_DELAY );
                CORO_WAIT( clock_time_after( t ), );

                __operate = data_get_byte( var_mcp_operate );
                if ( __operate != MI_NONE ) {
                    /* Открыть файл ИД или градуировок */
                    QString dir  = ( __operate == MI_WRITE_ID ) ? ini_dir_mi_id  : ini_dir_mi_grd; // /КОФ/КМСИП/
                    QString mask = ( __operate == MI_WRITE_ID ) ? ini_mask_mi_id : ini_mask_mi_grd; // *.id
                    QDir d( dir, mask, QDir::Name, QDir::Files | QDir::NoDotAndDotDot );
                    journal_freeze_time();
                    switch ( d.count()) {

                    case 0:
                        /* Нет файлов */
                        ERROR( file_not_found, encode_console( __s, &mask, sizeof( __s ))
                        , encode_console( __s0, &dir, sizeof( __s0 )), strerror( errno ));
                        journal_printf( "", ( __operate == MI_WRITE_ID ) ? M_FILE_ID : M_FILE_GRD, M_NOT_FOUND );

                        goto L_ERROR_1;

                    case 1:
                        __fn = d.entryInfoList()[ 0 ].absoluteFilePath();
                        __fd = fopen( encode_utf8( __s, &__fn, sizeof( __s )), "rb" );
                        if ( __fd == NULL ) {
                            /* Ошибка доступа к файлу */
                            ERROR( file_open, encode_console( __s, &__fn, sizeof( __s )), strerror( errno ));

                            goto L_ERROR_1;
                        }
                        __mcp_mask = data_get_dword( var_mcp_mask );
                        __mcp_error = 0;
                        mcp_block.wTblNum = ( __operate == MI_WRITE_ID ) ? TBL_ID : TBL_GRD;
                        mcp_block.dwSeekSet = 0;
                        __stage = STAGE_SENDING;
                        //__stage = STAGE_READING;
                        /* Размер образа */
                        fseek( __fd, 0, SEEK_END );
                        size = ftell( __fd );
                        fseek( __fd, 0, SEEK_SET );
                        dly = WORK_DELAY;
                        gettimeofday( &__t_start, NULL );

                        __mcp_ok = 0;
                        /* Сброс таймаута */
                        __confirm_to = clock_t_set( ini_mi_confirm );
                        __confirm_to_cnt = 0;

                        journal_printf( "", ( __operate == MI_WRITE_ID ) ? M_FILE_ID : M_FILE_GRD
                        , encode_utf8( __s, &__fn, sizeof( __s )));
                        break;

                        default:
                        /* Файлов больше чем один */
                        ERROR( file_more_1, encode_console( __s, &mask, sizeof( __s ))
                        , encode_console( __s0, &dir, sizeof( __s0 )), strerror( errno ));
                        journal_printf( "", ( __operate == MI_WRITE_ID ) ? M_FILE_ID : M_FILE_GRD, M_MORE_1 );
                        L_ERROR_1:
                        data_set_byte( var_mcp_operate, MI_NONE );
                        data_set_dword( var_mcp_mask, _MAXDWORD );  /* Ошибка файла */
                        __operate = MI_NONE;
                    }
                }
            }

            data_reset( peek_mcp, var_mcp_operate );
            gui_send_event( EVENT_MI_OPERATE );
        }

        /* Проверка записанной таблицы */
        if( __stage == STAGE_READING){
            mcp_read_block.wTblNum=0;
            mcp_read_block.dwSeekSet=0;
            message_transmit2( SEND_MCP1, CMD_MI_READ, 0, 0, sizeof( mcp_read_block ), &mcp_read_block );
            message_transmit2( SEND_MCP2, CMD_MI_READ, 0, 0, sizeof( mcp_read_block ), &mcp_read_block );
            message_transmit2( SEND_MCP3, CMD_MI_READ, 0, 0, sizeof( mcp_read_block ), &mcp_read_block );
            t = clock_t_set( IDLE_DELAY+IDLE_DELAY+IDLE_DELAY+IDLE_DELAY+IDLE_DELAY+IDLE_DELAY );
            CORO_WAIT( clock_time_after( t ), );
            //__stage = STAGE_READING_PROCESS;
        }

        if( __stage == STAGE_READING_PROCESS){
            //printf("popal in read\n");
        fread(mcp_block_print.byData, 512, 1, __fd);


        if(mcp_block_print.byData==0){
        }
            __stage=MCP_NONE;
            __operate = MI_NONE;
        }


        /* Операция */
        if ( __operate != MI_NONE ) {
            if ( __stage == STAGE_SENDING ) {  /* Передача таблицы */
                /* Считать блок */
                if ( mcp_block.dwSeekSet < size ) {
                    mcp_block.wLenData = ( WORD )( size - mcp_block.dwSeekSet );
                    uint32_t l = ( mcp_block.wLenData > BLOCK_SIZE ) ? BLOCK_SIZE : mcp_block.wLenData;
                    if ( fread( mcp_block.byData, l, 1, __fd )) {
                        /* Отправить блок */
                        message_transmit2( SEND_BI, CMD_MI_SEND_TABLE, 0, 0, sizeof( mcp_block ), &mcp_block, false, &wait );
//                      message_transmit2( SEND_MCP2, CMD_MI_SEND_TABLE, 0, 0, sizeof( mcp_block ), &mcp_block, false, &wait );
//                      message_transmit2( SEND_MCP3, CMD_MI_SEND_TABLE, 0, 0, sizeof( mcp_block ), &mcp_block, false, &wait );

                        /* Ожидание отправки сообщения */
                        CORO_WAIT( wait != MESSAGE_IN_QUEUE, );
                        mcp_block.dwSeekSet += BLOCK_SIZE;
                    } else {
                        /* Ошибка чтения файла */
                        __operate = MI_NONE;
                        ERROR( file_read, ini_at_home( encode_console( __s, &__fn, sizeof( __s ))));
                        SET_VAR( __mcp_error, _MAXDWORD );  /* Ошибка файла */
                    }
                }
                if ( mcp_block.dwSeekSet >= size ) {
                    __stage = STAGE_SENT;  /* Таблица передана */
                    /* Процент выполнения */
                    data_set_int( var_mcp_percent, 49 );
                }

            }

            else if ( __mcp_mask ) {  /* Есть получатели */
                if ( __mcp_ok == __mcp_mask ) {  /* Получены все подтверждения */
                    switch ( __stage ) {
                    /* Таблица передана */
                    case STAGE_SENT:
                        __stage = STAGE_WRITING;
                        mi_cmd_eeprom.wTblNum = mcp_block.wTblNum;
                        mi_cmd_eeprom.wROMOpCode = ROM_WRITE;
                        message_transmit2( SEND_MCP1, CMD_MI_PROG_TABLE, 0, 0, sizeof( mi_cmd_eeprom ), &mi_cmd_eeprom );
                        message_transmit2( SEND_MCP2, CMD_MI_PROG_TABLE, 0, 0, sizeof( mi_cmd_eeprom ), &mi_cmd_eeprom );
                        message_transmit2( SEND_MCP3, CMD_MI_PROG_TABLE, 0, 0, sizeof( mi_cmd_eeprom ), &mi_cmd_eeprom );
                        /* Процент выполнения */
                        data_set_int( var_mcp_percent, 99 );
                        __mcp_ok = 0;
                        /* Сброс таймаута */
                        __confirm_to = clock_t_set( ini_mi_confirm );
                        __confirm_to_cnt = 0;
                        break;
                    /* Таблица записана */
                    case STAGE_WRITING:
                        /* Таблица записана */
                        __stage = STAGE_WRITTEN;
                        __operate = MI_NONE;
                        break;
                    /* Ошибка в логике функции */
                    default:
                        ERROR( unreachable );
                        __operate = MI_NONE;
                    }
                } else {  /* Не все подтверждения */
                    /* Проверка таймаута */
                    if ( clock_time_after( __confirm_to )) {
                        MI_MASK err = ~__mcp_ok & __mcp_mask;
                        if ( ++__confirm_to_cnt < __CONFIRM_TO_LIMIT ) {
                            /* Повторная передача */
                            switch ( __stage ) {
                            /* Передача таблицы */
                            case STAGE_SENT:
                                mcp_block.dwSeekSet = 0;
                                fseek( __fd, 0, SEEK_SET );
                                __stage = STAGE_SENDING;
                                break;
                            /* Запись таблицы */
                            case STAGE_WRITING:
                                /* Таблица записана */
                                __stage = STAGE_SENT;
                                break;
                            /* Ошибка в логике функции */
                            default:
                                ERROR( unreachable );
                                __operate = MI_NONE;
                            }
                            /* Сброс таймаута */
                            __confirm_to = clock_t_set( ini_mi_confirm );
                        } else {
                            SET_VAR( __mcp_error, err );
                            __mcp_mask = __mcp_ok;
                            __operate = MI_NONE;
                        }
                    }
                }
            } else {  /* Нет получателей */
                __operate = MI_NONE;
            }
            if ( __operate == MI_NONE ) {  /* Операция завершена */
                data_set_byte( var_mcp_operate, MI_NONE );
                data_set_dword( var_mcp_mask, __mcp_error );
                /* Сбросить задержки */
                dly = IDLE_DELAY;
                gettimeofday( &__t_stop, NULL );
                timersub( &__t_start, &__t_stop, &__t_delta );
                data_set_int( var_mcp_percent, __t_delta.tv_sec );
                /* Закрыть файл */
                fclose( __fd );
                gui_send_event( EVENT_MI_OPERATE );
            }
        }
        CORO_YIELD();
	}
	CORO_END();
}

static MESSAGE_REMOTE *__queue_front( void )
{
	MESSAGE_REMOTE *mg = NULL;

	pthread_mutex_lock( &mi_mutex );

	if ( !mi_queue.empty()) {
		mg = &mi_queue.front();
	}
	pthread_mutex_unlock( &mi_mutex );

   return ( mg );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &mi_mutex );

	mi_queue.front().free_data();
	mi_queue.pop();

	pthread_mutex_unlock( &mi_mutex );
}

/** \} */
