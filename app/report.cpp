 /*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  report.cpp
 *  \brief  Задача формирования отчета.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define REPORT_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup report
 *  \{
 */

#define T( N ) varn_test_ + N,

static const int report_watch[] = {
	varn_state,
	varn_link_0,
	varn_link_1,
	UNFOLD( TEST_COUNT, T, T )
	varn_COUNT
};

#undef T

static const int report_watch2[] = {
    varmcp_stateM,
    varmcp_test_1,
    varmcp_test_net,
    varmcp_col_sn,
    varmcp_col_verify_c,
    varmcp_COUNT
};

static const int report_watch3[] = {
    varm_state,
    varm_test_2,
    varm_col_sn,
    varm_col_verify_c,
    varm_COUNT
};

static const int report_watch_local[] = {
	var_td_operate,
	var_td_percent,
    var_mi_operate,
    var_mcp_percent,
    var_mcp_operate,
	var_COUNT
};

/**
 *  \brief Состояние теста.
 */
typedef struct {
	/**
	 *  \brief Маска тестируемых рабочих станций / МИ.
	 */
	uint32_t perform;

	/**
	 *  \brief Маска результата теста.
	 */
	uint32_t mask;

	/**
	 *  \brief Обобщенный результат.
	 */
	uint8_t state;

} __TEST;

/**
 *  \brief Состояние тестов станций.
 */
static __TEST __test[ TEST_COUNT ];

/**
 *  \brief Состояние тестов МЦП.
 */
static __TEST __test_mcp[ varmcp_test_net - varmcp_test_1 + 1 ]; // тут было тест ми varm_test_1 and varm_test_net

/**
  * \brief Состояние теста МИ
  */

static __TEST __test_mi[varm_test_2 + 1];

/**
 *  \brief Состояние операции с ТД.
 */
static int __td_operate, __td_operate_last;
static ST_MASK __td_mask;

/**
 *  \brief Состояние операции с МИ.
 */
static int __mi_operate, __mi_operate_last;
static ST_MASK __mi_mask;

/**
 *  \brief Состояние операции с МCP.
 */
static int __mcp_operate, __mcp_operate_last;
static ST_MASK __mcp_mask;


#define ID_EMPTY        0
#define ID_TD_OPERATE   0x100
#define ID_MI_OPERATE   0x200

#define LINE_NOT_FOUND  0

/**
 *  \brief Идентификаторы тестов/операций по строкам.
 */
static int __lines[ SUMMARY_COUNT + 1 ];

/**
 *  \brief Cтрок обобщенного состояния отображенно.
 */
static uint8_t __showed;

/**
 *  \brief Признак - текущее время зафиксировано.
 */
static bool __freezed;

static void report_string( uint8_t st, int col, const char *s );
static void report_state( uint8_t st, uint8_t state );
static void report_mi_state( uint8_t st, uint8_t state );
static void report_mcp_state( uint8_t st, uint8_t state );
static void report_mi_sn( uint8_t st, uint32_t col, uint32_t sn, uint32_t sn_ok );
static void report_mcp_sn( uint8_t st, uint32_t col, uint32_t sn, uint32_t sn_ok );
static void report_mi_verify_c( uint8_t st, uint32_t col );
static void report_link( uint8_t st, uint8_t net, uint8_t val );
static void report_test( uint8_t st, varn_t var, uint8_t val );
static void report_mi_test( uint8_t st, varn_t var, uint8_t val );
static void report_mcp_test( uint8_t st, varn_t var, uint8_t val );
static void report_td_operate( uint8_t op, ST_MASK mask );
static void report_mi_operate( uint8_t op, ST_MASK mask );
static void deduce_test_state( uint8_t st, varn_t var, uint8_t val );
static void deduce_mi_test_state( uint8_t st, varm_t var, uint8_t val );
static void deduce_mcp_test_state( uint8_t st, uint var, uint8_t val );
static void deduce_td_operate( uint8_t op, ST_MASK mask );
static void deduce_mi_operate( uint8_t op, ST_MASK mask );

static uint8_t search_summary( int id );
static void alloc_summary( int id );
static void free_summary( int id );
static void refresh_summary( int id, bool focus = true );
static void refresh_summary2( int id, bool focus = true );
static void refresh_summary3( int id, bool focus = true );



#define STR_TURN_OFF_JR         "Станция выключена."
#define STR_TURN_ON_JR          "Станция включена."
#define STR_RESTORE_WORK_JR     "Восстановление работы."
#define STR_WORK_JR             "Реализация закона управления."
#define STR_WORK_PASS_JR        "Реализация закона управления (пассивный)."
#define STR_KPA_JR              "ПКФ включен."
#define STR_STATE_UNDEF_JR      "Ошибка! Неопределенное состояние."

static const char *state_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case TURN_ON      : *col = COLOR_OK;
		break;
	case TURN_OFF     : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TURN_OFF     : return STR_TURN_OFF_JR;
	case TEST_STATE   :
	case TURN_ON      : return STR_TURN_ON_JR;
	case RESTORE_WORK : return STR_RESTORE_WORK_JR;
	case WORK         : return STR_WORK_JR;
	case PKF          : return STR_KPA_JR;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_MI_OFF_JR         "МИ выключен."
#define STR_MI_ON_JR          "МИ включен."
#define STR_MI_RESET_JR       "МИ перезапущен по WDT."
#define STR_MI_WORK_JR        "Режим измерений."

static const char *state_mi_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case MI_ON         : *col = COLOR_OK;
		break;
	case MI_ON_RESET   :
	case MI_WORK_RESET :
	case MI_OFF        : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case MI_OFF        : return STR_MI_OFF_JR;
	case MI_ON         : return STR_MI_ON_JR;
	case MI_ON_RESET   :
	case MI_WORK_RESET : return STR_MI_RESET_JR;
	case MI_WORK       : return STR_MI_WORK_JR;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_MCP_OFF_JR         "МЦП выключен."
#define STR_MCP_ON_JR          "МЦП включен."
#define STR_MCP_RESET_JR       "МЦП перезапущен по WDT."
#define STR_MCP_WORK_JR        "Режим измерений."

static const char *state_mcp_template_jr( uint8_t val, int *col )
{
    switch ( val ) {
    case MCP_ON         : *col = COLOR_OK;
        break;
    case MCP_ON_RESET   :
    case MCP_WORK_RESET :
    case MCP_OFF        : *col = COLOR_FAIL;
        break;
    }
    switch ( val ) {
    case MCP_OFF        : return STR_MCP_OFF_JR;
    case MCP_ON         : return STR_MCP_ON_JR;
    case MCP_ON_RESET   :
    case MCP_WORK_RESET : return STR_MCP_RESET_JR;
    case MCP_WORK       : return STR_MCP_WORK_JR;
    }
    *col = COLOR_FAIL;
    return STR_STATE_UNDEF_JR;
}


#define STR_LINK_NONE_JR          "Станция не подключена по сети %d."
#define STR_LINK_CONNECTED_JR     "Соединение по сети %d установлено."
#define STR_LINK_DISCONNECTED_JR  "Соединение по сети %d разорвано."
#define STR_LINK_FAILED_JR        "Неисправность сети %d."

static const char *link_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case LINK_CONNECTED   : *col = COLOR_OK;
		break;
	case LINK_FAILED      : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case LINK_NONE        : return STR_LINK_NONE_JR;
	case LINK_CONNECTED   : return STR_LINK_CONNECTED_JR;
	case LINK_DISCONNECTED: return STR_LINK_DISCONNECTED_JR;
	case LINK_FAILED      : return STR_LINK_FAILED_JR;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_TEST_PERFORM_SUM    "%s выполняется..."
#define STR_TEST_PASSED_SUM     "%s успешно пройден."
#define STR_TEST_FAILED_SUM     "%s завершено, неисправность."
#define STR_TEST_ERROR_SUM      "%s не может быть выполнен."
#define STR_TEST_ABORTED_SUM    "%s прерван."

static const char *test_template_sum( uint8_t val, int *col )
{
	switch ( val ) {
	case TEST_PASSED : *col = COLOR_OK;
		break;
	case TEST_FAILED :
	case TEST_ERROR  : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TEST_PERFORM: return STR_TEST_PERFORM_SUM;
	case TEST_PASSED : return STR_TEST_PASSED_SUM;
	case TEST_FAILED : return STR_TEST_FAILED_SUM;
	case TEST_ERROR  : return STR_TEST_ERROR_SUM;
	case TEST_ABORTED: return STR_TEST_ABORTED_SUM;
	}
	return STR_TEST_ABORTED_SUM;
}

#define STR_TEST_PERFORM_JR     "%s запущен..."
#define STR_TEST_PASSED_JR      STR_TEST_PASSED_SUM
#define STR_TEST_FAILED_JR      STR_TEST_FAILED_SUM
#define STR_TEST_ERROR_JR       STR_TEST_ERROR_SUM
#define STR_TEST_ABORTED_JR     STR_TEST_ABORTED_SUM

static const char *test_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case TEST_PASSED : *col = COLOR_OK;
		break;
	case TEST_FAILED :
	case TEST_ERROR  : *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TEST_PERFORM: return STR_TEST_PERFORM_JR;
	case TEST_PASSED : return STR_TEST_PASSED_JR;
	case TEST_FAILED : return STR_TEST_FAILED_JR;
	case TEST_ERROR  : return STR_TEST_ERROR_JR;
	case TEST_ABORTED: return STR_TEST_ABORTED_JR;
	}
	return STR_TEST_ABORTED_JR;
}

#define STR_TD_WRITE_JR         "Операция загрузки ТД:"
#define STR_TD_CHECK_JR         "Операция проверки ТД:"
#define STR_TD_OK_JR            "Операция с ТД завершена успешно."
#define STR_TD_ERROR_JR         "Операция с ТД завершена, неисправность:"
#define STR_TD_ABORT_JR         "Проверка ТД прервана."

static const char *td_operate_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case TD_OK   : *col = COLOR_OK;
		break;
	case TD_ERROR: *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TD_WRITE: return STR_TD_WRITE_JR;
	case TD_CHECK: return STR_TD_CHECK_JR;
	case TD_OK   : return STR_TD_OK_JR;
	case TD_ERROR: return STR_TD_ERROR_JR;
	case TD_ABORT: return STR_TD_ABORT_JR;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_TD_WRITE_SUM        "Загрузка ТД..."
#define STR_TD_CHECK_SUM        "Проверка ТД..."
#define STR_TD_WRITE_OK_SUM     "Загрузка ТД завершена успешно."
#define STR_TD_CHECK_OK_SUM     "Проверка ТД завершена успешно."
#define STR_TD_WRITE_ERROR_SUM  "Загрузка ТД завершена, неисправность."
#define STR_TD_CHECK_ERROR_SUM  "Проверка ТД завершена, неисправность."
#define STR_TD_ABORT_SUM        STR_TD_ABORT_JR

static const char *td_operate_template_sum( uint8_t val, uint8_t old, int *col )
{
	switch ( val ) {
	case TD_OK   : *col = COLOR_OK;
		break;
	case TD_ERROR: *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case TD_WRITE: return STR_TD_WRITE_SUM;
	case TD_CHECK: return STR_TD_CHECK_SUM;
	case TD_OK   : return ( old == TD_WRITE ) ? STR_TD_WRITE_OK_SUM : STR_TD_CHECK_OK_SUM;
	case TD_ERROR: return ( old == TD_WRITE ) ? STR_TD_WRITE_ERROR_SUM : STR_TD_CHECK_ERROR_SUM;
	case TD_ABORT: return STR_TD_ABORT_SUM;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_MI_WRITE_ID_JR      "Загрузка ИД МИ/МЦП:"
#define STR_MI_WRITE_GRAD_JR    "Загрузка градуировок МИ/МЦП:"
#define STR_MI_OK_JR            "Операция загрузки данных в МИ/МЦП завершена успешно."
#define STR_MI_ERROR_JR         "Операция загрузки данных в МИ/МЦП завершена, неисправность:"

static const char *mi_operate_template_jr( uint8_t val, int *col )
{
	switch ( val ) {
	case MI_OK   : *col = COLOR_OK;
		break;
	case MI_ERROR: *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
	case MI_WRITE_ID  : return STR_MI_WRITE_ID_JR;
	case MI_WRITE_GRAD: return STR_MI_WRITE_GRAD_JR;
	case MI_OK        : return STR_MI_OK_JR;
	case MI_ERROR     : return STR_MI_ERROR_JR;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_MI_WRITE_ID_SUM          "Загрузка ИД МИ/МЦП..."
#define STR_MI_WRITE_GRAD_SUM        "Загрузка градуировок МИ/МЦП..."
#define STR_MI_WRITE_ID_OK_SUM       "Загрузка ИД МИ/МЦП завершена успешно."
#define STR_MI_WRITE_GRAD_OK_SUM     "Загрузка градуировок МИ/МЦП завершена успешно."
#define STR_MI_WRITE_ID_ERROR_SUM    "Загрузка ИД МИ/МЦП завершена, неисправность."
#define STR_MI_WRITE_GRAD_ERROR_SUM  "Загрузка градуировок МИ/МЦП завершена, неисправность."

static const char *mi_operate_template_sum( uint8_t val, uint8_t old, int *col )
{
	switch ( val ) {
    case MCP_OK   : *col = COLOR_OK;
		break;
    case MCP_ERROR: *col = COLOR_FAIL;
		break;
	}
	switch ( val ) {
    case MCP_WRITE_ID  : return STR_MI_WRITE_ID_SUM;
    case MCP_WRITE_GRAD: return STR_MI_WRITE_GRAD_SUM;
    case MCP_OK        : return ( old == MI_WRITE_ID ) ? STR_MI_WRITE_ID_OK_SUM : STR_MI_WRITE_GRAD_OK_SUM;
    case MCP_ERROR     : return ( old == MI_WRITE_ID ) ? STR_MI_WRITE_ID_ERROR_SUM : STR_MI_WRITE_GRAD_ERROR_SUM;
	}
	*col = COLOR_FAIL;
	return STR_STATE_UNDEF_JR;
}

#define STR_MI_SN_OK_JR     "Заводской номер: %d."
#define STR_MI_SN_ERROR_JR  "Неверный заводской номер: %d, задан: %d."
#define STR_MI_SN_NONE_JR   "Незадан заводской номер."

static const char *mi_sn_template_jr( uint32_t col )
{
	switch ( col ) {

	case COLOR_FAIL:
		return STR_MI_SN_ERROR_JR;

	case COLOR_OK:
		return STR_MI_SN_OK_JR;
	}
	return STR_MI_SN_NONE_JR;
}

#define STR_MI_VERIFY_C_OK_JR  "Корректный верификатор коррект. кодов."
#define STR_MI_VERIFY_C_ERROR_JR  "Неверный верификатор коррект. кодов."

#define STR_RESULT_TEST_HDD  "Сбойных блоков: %u, со времением доступа > 150мс: %u."

/**
 *  \brief Инициализация задачи.
 */
int report_init( void )
{
	char s[ 256 ];
    char s1[ 1024 ];


	datn_watch( peekn_report, report_watch );
    datm_watch( peekm_report, report_watch3 );
	data_watch( peek_report, report_watch_local );
    datmcp_watch( peekmcp_report, report_watch2);
	memset( __test, 0, sizeof( __test ));
    memset( __test_mcp, 0, sizeof( __test ));
    memset( __test_mi, 0, sizeof( __test ));

	memset( __lines, ID_EMPTY, sizeof( __lines ));
	__td_operate_last = __td_operate = TD_NONE;
	__mi_operate_last = __mi_operate = MI_NONE;
    __mcp_operate_last = __mcp_operate = MCP_NONE;

	journal_open( ini_at_home( encode_system( s, &ini_journal_path, sizeof( s ))));
    //journal_openmcp( ini_at_home( encode_system( s1, &ini_journal_mcp, sizeof( s1 ))));

	return CO_READY;
}

/**
 *  \brief Деинициализация задачи.
 */
void report_uninit( void )
{
	journal_close();
}

/**
 *  \brief Задача формирования отчета.
 */
CORO_DEFINE( report )
{
	static struct timeval t;
	static int i, ii;
    static uint32_t testmi[24];

	CORO_BEGIN();

	/* Пауза */
	t = clock_t_set( ini_report_start_delay );
	CORO_WAIT( clock_time_after( t ), );

	for ( ; ; ) {
		/* Работаем по таймеру */
		t = clock_t_set( ini_report_delay );
		CORO_WAIT( clock_time_after( t ), );

		__freezed = false;
		/* Проверка изменения состояния рабочих станций */
		for ( i = 0; i < st_count; i++ ) {
			if ( datn_get_changed_any( peekn_report, i )) {
				/* Что-то изменилось */
				if ( datn_get_changed( peekn_report, varn_state, i )) {
					/* Изменилось состояние модуля */
					report_state( i, datn_get_byte( varn_state, i ));
				}
				for ( ii = 0; ii < ini_net_count; ii++ ) {
					if ( datn_get_changed( peekn_report, varn_link + ii, i )) {
						/* Изменилось состояние сетевого подключения */
                        report_link( i, ii, datn_get_byte( varn_link + ii, i ));
					}
				}
                for ( ii = varn_test_; ii < varn_test_ + TEST_COUNT; ii++ ) {
					if ( datn_get_changed( peekn_report, ii, i )) {
						/* Изменилось состояние теста */
						uint8_t val = datn_get_byte( ii, i );

// printf("\nreport testing change ST=%d, TEST=%d => %X\n", i, ii, val);

                        report_test( i, ii, val );
                        deduce_test_state( i, ii, val );
					}
				}
				/* Сброс признаков изменения */
				datn_reset_all( peekn_report, i );
			}
			/* Операции */
			if ( data_get_changed_any( peek_report )) {
				/* Что-то изменилось */
				if ( data_get_changed( peek_report, var_td_operate )) {
					/* Изменилось состояние операции ТД */
					uint8_t op = data_get_byte( var_td_operate );
					ST_MASK mask = data_get_dword( var_td_mask );

					if ( op == TD_NONE ) op = ( mask ) ? (( mask == _MAXDWORD ) ? TD_ABORT : TD_ERROR ) : TD_OK;
					if ( mask == _MAXDWORD ) mask = 0;
                    report_td_operate( op, mask );
                    deduce_td_operate( op, mask );
				}
				if ( data_get_changed( peek_report, var_td_percent ) && data_get_int( var_td_percent ) > 0 ) {
					/* Изменился процент выполнения ТД */
					refresh_summary( ID_TD_OPERATE, false );
				}

                if ( data_get_changed( peek_report, var_mcp_operate )) {
                    /* Изменилось состояние операции МCP */
                    uint8_t op = data_get_byte( var_mcp_operate );
                    MI_MASK mcp_mask = data_get_dword( var_mcp_mask );
                    ST_MASK mask = mcp_mask;
                    if ( op == MCP_NONE ) {
                        switch ( mcp_mask ) {
                        /* Успешная операция */
                        case 0:
                            op = MCP_OK;
                            break;
                        /* Ошибка чтения файла */
                        case ( _MAXDWORD ):
                            op = MCP_ERROR;
                            mask = BIT( st_tst );
                            break;
                        /* Ошибка записи таблицы */
                        default:
                            op = MCP_ERROR;
                        }
                    }
                    report_mi_operate( op, mask );
                    deduce_mi_operate( op, mask );
				}
                // new
//                if ( data_get_changed( peek_report, var_mi_operate )) {
//					/* Изменилось состояние операции МИ */
//					uint8_t op = data_get_byte( var_mi_operate );
//					MI_MASK mi_mask = data_get_dword( var_mi_mask );
//					ST_MASK mask1 = mi_mask << MI_ID_MIN;
//					if ( op == MI_NONE ) {
//						switch ( mi_mask ) {
//						/* Успешная операция */
//						case 0:
//							op = MI_OK;
//							break;
//						/* Ошибка чтения файла */
//						case ( _MAXDWORD ):
//							op = MI_ERROR;
//							mask = BIT( st_tst );
//							break;
//						/* Ошибка записи таблицы */
//						default:
//							op = MI_ERROR;
//						}
//					}
//					report_mi_operate( op, mask );
//					deduce_mi_operate( op, mask );
//				}
                // new

                if ( data_get_changed( peek_report, var_mcp_percent ) && data_get_int( var_mcp_percent ) > 0 ) {
					/* Изменился процент выполнения МИ */
					refresh_summary( ID_MI_OPERATE, false );
				}
				/* Сброс признаков изменения */
				data_reset_all( peek_report );
			}
		}
        /* Проверка изменения состояния МИ/mcp */
        for ( i = 0; i < mcp_count; i++ ) {
            if ( datmcp_get_changed_any( peekmcp_report, i )) {
                /* Что-то изменилось */
                if ( datmcp_get_changed( peekmcp_report, varmcp_stateM, i )) {
					/* Изменилось состояние модуля */
                    report_mcp_state( i, datmcp_get_byte( varmcp_stateM, i ));
				}

                for ( ii = varmcp_test_1; ii <= varmcp_test_net; ii++ ) {
                    if ( datmcp_get_changed( peekmcp_report, ii, i )) {
                        /* Изменилось состояние теста */
                        uint8_t val = datmcp_get_byte( ii, i );

                        report_mcp_test( i, ii, val );
                        deduce_mcp_test_state( i, ii, val );
					}
				}
				/* Проверка изменения серийного номера */
                if ( datmcp_get_changed( peekmcp_report, varmcp_col_sn, i )) {
                    report_mcp_sn( i, datmcp_get_dword( varmcp_col_sn, i ), datmcp_get_dword( varmcp_sn, i ), mcp_list[ i ].snmcp );
				}
				/* Проверка изменения корректности верификатора коррект. кодов */
                if ( datmcp_get_changed( peekmcp_report, varmcp_col_verify_c, i )) {
                    report_mi_verify_c( i, datmcp_get_dword( varmcp_col_verify_c, i ));
				}
				/* Сброс признаков изменения */
                datmcp_reset_all( peekmcp_report, i );
                datmcp_reset_all( peekmcp_report, i );

			}
        }

        for ( uint k = 0; k<mi_count; k++){
            if ( datm_get_changed_any(peekm_report, k)){

            if(datm_get_changed(peekm_report, varm_test_2, k)){

                uint8_t val2 = datm_get_byte(varm_test_2, k);
                report_mi_test(k,2,val2);
                deduce_mi_test_state( k, 2, val2 );
            }
            }
            datm_reset_all( peekm_report, k );
            datm_reset_all( peekm_report, k );
        }

        // вот тут сделать тест ми чтобы выдавало
		CORO_YIELD();
	}
	CORO_END();
}

static void report_string( uint8_t st, int col, const char *s )
{
	if ( !__freezed ) {
		journal_freeze_time();
		__freezed = true;
	}
	if ( st != st_tst ) {
        report_message( REPORT_JOURNAL, st, col, s );
		journal_print( st_list_ex[ st ].name_ch, s );
	} else {
		journal_print( "", s );
	}
}

static void report_mi_string( uint8_t st, int col, const char *s )
{
	if ( !__freezed ) {
		journal_freeze_time();
		__freezed = true;
	}
    st /= 6;
    report_message( REPORT_JOURNAL, st+4, col, s );
    journal_print( mcp_list[ st ].name_ch, s );
}

static void report_mcp_string( uint8_t st, int col, const char *s )
{
    if ( !__freezed ) {
        journal_freeze_time();
        __freezed = true;
    }
    report_message( REPORT_JOURNAL, st, col, s ); // тут делает в журнал станции мцп
    journal_print( mcp_list[ st ].name_ch, s );
}

static void report_state( uint8_t st, uint8_t val )
{
	int col = COLOR_DEFAULT;
	report_string( st, col, state_template_jr( val, &col ));
}

static void report_mi_state( uint8_t st, uint8_t val )
{
	int col = COLOR_DEFAULT;
	report_mi_string( st, col, state_mi_template_jr( val, &col ));
}

static void report_mcp_state( uint8_t st, uint8_t val )
{
    int col = COLOR_DEFAULT;
    //st+=4;
    report_string( st+4, col, state_mcp_template_jr( val, &col ));
}


static void report_link( uint8_t st, uint8_t net, uint8_t val )
{
	int col = COLOR_DEFAULT;
	char s[ REPORT_STR_LIMIT ];

    snprintf( s, REPORT_STR_LIMIT, link_template_jr( val, &col ), net + 1 );
    report_string( st, col, s );
}

static void report_test( uint8_t st, varn_t var, uint8_t val )
{
	int col = COLOR_DEFAULT;
	char s[ REPORT_STR_LIMIT ];

	if ( val == NO_TEST ) return;

	snprintf( s, REPORT_STR_LIMIT, test_template_jr( val, &col ), datn_get_title( var ));
    report_string( st, col, s );
}

static void report_mi_test( uint8_t st, varm_t var, uint8_t val )
{
	int col = COLOR_DEFAULT;
    char s[ REPORT_STR_LIMIT ];
    if ( val == NO_TEST ) return;

    snprintf( s, REPORT_STR_LIMIT, test_template_jr( val, &col ),datm_get_title( var)); // zavtra mi_list[st].name_ch

    //report_mi_string( st, col, s ); // тут

}

static void report_mcp_test( uint8_t st, int var, uint8_t val )
{
    int col = COLOR_DEFAULT;
    char s[ REPORT_STR_LIMIT ];

    if ( val == NO_TEST ) return;

    snprintf( s, REPORT_STR_LIMIT, test_template_jr( val, &col ), datmcp_get_title( var ));
    report_string( st+4, col, s );
}

#define REPORT_TD_STR_LIMIT  ( REPORT_STR_LIMIT * 2 )

static void report_td_operate( uint8_t op, ST_MASK mask )
{
	int col = COLOR_DEFAULT;
	char s[ REPORT_TD_STR_LIMIT ];

	strcpy( s, td_operate_template_jr( op, &col ));
	if ( mask ) st_mask2string( mask, s, strlen( s ), REPORT_TD_STR_LIMIT - 16 );
	report_string( st_tst, col, s );
}

static void report_mi_operate( uint8_t op, ST_MASK mask )
{
	int col = COLOR_DEFAULT;
	char s[ REPORT_TD_STR_LIMIT ];

    strcpy( s, mi_operate_template_jr( op, &col ));
    if ( mask ) mcp_mask2string( mask, s, strlen( s ), REPORT_TD_STR_LIMIT - 16 );
    report_mcp_string( st_tst, col, s );
}

static void report_mi_sn( uint8_t st, uint32_t col, uint32_t sn, uint32_t sn_ok )
{
	char s[ REPORT_TD_STR_LIMIT ];

	snprintf( s, REPORT_STR_LIMIT, mi_sn_template_jr( col ), sn, sn_ok );
	report_mi_string( st, col, s );
}

static void report_mcp_sn( uint8_t st, uint32_t col, uint32_t sn, uint32_t sn_ok )
{
    char s[ REPORT_TD_STR_LIMIT ];

    snprintf( s, REPORT_STR_LIMIT, mi_sn_template_jr( col ), sn, sn_ok );
    report_string( st+4, col, s );
}

static void report_mi_verify_c( uint8_t st, uint32_t col )
{
	switch ( col ) {

	case COLOR_FAIL:
        report_string( st+4, col, STR_MI_VERIFY_C_ERROR_JR );
		return;

	case COLOR_OK:
        report_string( st+4, col, STR_MI_VERIFY_C_OK_JR );
		return;
	}
}

static void deduce_test_state( uint8_t st, varn_t var, uint8_t state )
{
	int i;
	uint8_t val;
	__TEST *t = &__test[ var - varn_test_ ];

	if ( t->state == TEST_PERFORM ) {
		/* Тест выполняется */
		if ( state == TEST_PERFORM ) {
			/* Добавление рабочей станции */
			t->perform |= BIT( st );
			t->mask |= BIT( st );
			refresh_summary( var );
		} else {
			if ( t->perform & BIT( st )) {
				/* Вывод обобщенного состояния */
				t->state = TEST_NONE;
				t->mask = 0;
				for ( i = 0; i < st_count; i++ ) {
					if ( t->perform & BIT( i )) {
						if (( val = datn_get_byte( var, i )) > t->state ) {
							t->state = val;
							t->mask = BIT( i );
						} else {
							if ( val == t->state ) t->mask |= BIT( i );
						}
					}
				}
				refresh_summary( var );
				if ( t->state != TEST_PERFORM ) {
					/* Тест завершен */
					free_summary( var );
				}
			}
		}
	} else {
		if ( state == TEST_PERFORM ) {
			/* Тест запущен */
			t->state = TEST_PERFORM;
			t->perform = BIT( st );
			t->mask = t->perform;
			/* Поиск свободной строки */
			alloc_summary( var );
			refresh_summary( var );
		}
	}
}

static void deduce_mi_test_state( uint8_t st, varm_t var, uint8_t state )
{
    int i;
    uint8_t val;
    __TEST *t = &__test_mi[ var - varm_test_2 ];

    if ( t->state == TEST_PERFORM ) {
        /* Тест выполняется */
        if ( state == TEST_PERFORM ) {
            /* Добавление рабочей станции */
            t->perform |= BIT( st );
            t->mask |= BIT( st ) << MI_ID_MIN;
            refresh_summary2( -var );
        } else {
            if ( t->perform & BIT( st )) {
                /* Вывод обобщенного состояния */
                t->state = TEST_NONE;
                t->mask = 0;
                for ( i = 0; i < mi_count; i++ ) {
                    if ( t->perform & BIT( i )) {
                        if (( val = datm_get_byte( var, i )) > t->state ) {
                            t->state = val;
                            t->mask = BIT( i ) << MI_ID_MIN;
                        } else {
                            if ( val == t->state ) t->mask |= BIT( i ) << MI_ID_MIN;
                        }
                    }
                }
                refresh_summary2( -var );
                if ( t->state != TEST_PERFORM ) {
                    /* Тест завершен */
                    free_summary( -var );
                }
            }
        }
    } else {
        if ( state == TEST_PERFORM ) {
            /* Тест запущен */
            t->state = TEST_PERFORM;
            t->perform = BIT( st );
            t->mask = t->perform << MI_ID_MIN;
            /* Поиск свободной строки */
            alloc_summary( -var );
            refresh_summary2( -var );
        }
    }
}

static void deduce_mcp_test_state( uint8_t st, uint var, uint8_t state )
{
    int i;
    uint8_t val;
    __TEST *t = &__test_mcp[ var - varmcp_test_1 ];

    if ( t->state == TEST_PERFORM ) {
        /* Тест выполняется */
        if ( state == TEST_PERFORM ) {
            /* Добавление рабочей станции */
            t->perform |= BIT( st );
            t->mask |= BIT( st );
            refresh_summary3( -var );
        } else {
            if ( t->perform & BIT( st )) {
                /* Вывод обобщенного состояния */
                t->state = TEST_NONE;
                t->mask = 0;
                for ( i = 0; i < mcp_count; i++ ) {
                    if ( t->perform & BIT( i )) {
                        if (( val = datmcp_get_byte( var, i )) > t->state ) {
                            t->state = val;
                            t->mask = BIT( i );
                        } else {
                            if ( val == t->state ) t->mask |= BIT( i );
                        }
                    }
                }
                refresh_summary3( -var );
                if ( t->state != TEST_PERFORM ) {
                    /* Тест завершен */
                    free_summary( -var );
                }
            }
        }
    } else {
        if ( state == TEST_PERFORM ) {
            /* Тест запущен */
            t->state = TEST_PERFORM;
            t->perform = BIT( st );
            t->mask = t->perform << MI_ID_MIN; // тут
            /* Поиск свободной строки */
            alloc_summary( -var );
            refresh_summary3( -var );
        }
    }
}

static void deduce_td_operate( uint8_t op, ST_MASK mask )
{
	switch ( op ) {

	case TD_CHECK:
	case TD_WRITE:
		if ( __td_operate != TD_NONE ) break;

		/* Операция запущена */
		__td_operate = op;
		__td_mask = mask;
		/* Поиск свободной строки */
		alloc_summary( ID_TD_OPERATE );
		refresh_summary( ID_TD_OPERATE );
		break;

	case TD_ERROR:
	case TD_ABORT:
		__td_mask = mask;
		/* fallthrough */

	case TD_OK:
		/* Результат операции */
		__td_operate_last = __td_operate;
		__td_operate = op;
		refresh_summary( ID_TD_OPERATE );
		/* Операция завершена */
		__td_operate_last = __td_operate = TD_NONE;
		free_summary( ID_TD_OPERATE );
		break;
	}
}

static void deduce_mi_operate( uint8_t op, ST_MASK mask )
{

	switch ( op ) {

	case MI_WRITE_ID:
	case MI_WRITE_GRAD:
        if ( __mcp_operate != MCP_NONE ) break;

		/* Операция запущена */
        __mcp_operate = op;
        __mcp_mask = mask;
		/* Поиск свободной строки */
		alloc_summary( ID_MI_OPERATE );
        refresh_summary( ID_MI_OPERATE );
		break;

	case MI_ERROR:
        __mcp_mask = mask;
		/* fallthrough */

	case MI_OK:
		/* Результат операции */
        __mcp_operate_last = __mcp_operate;
        __mcp_operate = op;
        refresh_summary( ID_MI_OPERATE );
		/* Операция завершена */
        __mcp_operate_last = __mcp_operate = MCP_NONE;
		free_summary( ID_MI_OPERATE );
		break;
	}
}

static uint8_t search_summary( int id )
{
	for ( int i = 1; i < SUMMARY_COUNT + 1; i++ ) if ( __lines[ i ] == id ) return ( i );

	return ( LINE_NOT_FOUND );
}

static void alloc_summary( int id )
{
	int i;
	uint8_t line;

	if ( __showed < SUMMARY_COUNT ) {
		/* Имеются свободные строки */
		line = ++__showed;
	} else {
		uint8_t prey = LINE_NOT_FOUND;

		/* Поиск завершенных тестов */
		for ( i = 1; i < SUMMARY_COUNT + 1; i++ ) {
			if ( __lines[ i ] == ID_EMPTY ) {
				prey = i;
				break;
			}
		}
		if ( prey == LINE_NOT_FOUND ) {
			/* Жертвуем первой строкой */
			prey = 1;
			__lines[ prey ] = ID_EMPTY;
		}
		/* Прокрутка строк */
		for ( i = prey; i < SUMMARY_COUNT; i++ ) {
			__lines[ i ] = __lines[ i + 1 ];

			report_message( REPORT_SCROLL, i );
		}
		line = SUMMARY_COUNT;
	}
	/* Занять строку */
	__lines[ line ] = id;
}

static void free_summary( int id )
{
	__lines[ search_summary( id )] = ID_EMPTY;
}

#define REPORT_STR_END  " ..."

void st_mask2string( ST_MASK mask, char *s, int pos, int limit )
{
	int i;

    for ( i = 0; i < st_count; i++ ) {
		if ( mask & BIT( i )) {
			if ( pos > limit ) {
				memcpy( s + pos, REPORT_STR_END, sizeof( REPORT_STR_END ));
				break;
            }
            pos += sprintf( s + pos, " %s", st_list_ex[ i ].name_ch );
		}
	}
}

void mi_mask2string( ST_MASK mask, char *s, int pos, int limit )
{
    int i;

    for ( i = 0; i < mi_count; i++ ) {
        if ( mask & BIT( i )) {
            if ( pos > limit ) {
                memcpy( s + pos, REPORT_STR_END, sizeof( REPORT_STR_END ));
                break;
            }
            pos += sprintf( s + pos, " %s", mi_list[ i ].name_ch );
        }
    }
}
void mcp_mask2string( MI_MASK mask, char *s, int pos, int limit )
{
    int i;

    for ( i = 0; i < mcp_count; i++ ) {
        if ( mask & BIT( i )) {
            if ( pos > limit ) {
                memcpy( s + pos, REPORT_STR_END, sizeof( REPORT_STR_END ));
                break;
            }
            pos += sprintf( s + pos, " %s", mcp_list[ i ].name_ch );
        }
    }
}

#define REPORT_STR_STATIC_LIMIT ( REPORT_STR_LIMIT - 16 )

static void refresh_summary( int id, bool focus )
{   int marker = 0;
	int col = COLOR_DEFAULT;
	char s[ REPORT_STR_LIMIT ];
	int pos;
	uint32_t mask;
	uint8_t line = search_summary( id );

	if ( line == LINE_NOT_FOUND ) return;

	/* Состояние */
	switch ( id ) {

	case ID_TD_OPERATE:
		mask = __td_mask;
		strcpy( s, td_operate_template_sum( __td_operate, __td_operate_last, &col ));
		pos = strlen( s );
		switch ( __td_operate ) {
		case TD_WRITE:
		case TD_CHECK:
			/* Процент выполнения */
			pos += sprintf( s + pos, " %2u%%", data_get_int( var_td_percent ));
			break;

		case TD_OK:
			/* Время выполнения */
			pos += sprintf( s + pos, " %d сек", -data_get_int( var_td_percent ));
			break;
		}
		break;

    case ID_MI_OPERATE:
        mask = __mcp_mask;
        marker=mask;
        strcpy( s, mi_operate_template_sum( __mcp_operate, __mcp_operate_last, &col ));
        pos = strlen( s );
        switch ( __mcp_operate ) {
        case MCP_WRITE_ID:
        case MCP_WRITE_GRAD:
            /* Процент выполнения */
            pos += sprintf( s + pos, " %2u%%", data_get_int( var_mcp_percent ));
            break;

        case MCP_OK:
            /* Время выполнения */
            pos += sprintf( s + pos, " %d сек", -data_get_int( var_mcp_percent ));
            break;
        }
        break;

	default:
		if ( id > 0 ) {
			int t = id - varn_test_;

			mask = __test[ t ].mask;
			pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test[ t ].state, &col )
			, datn_get_title( id ));
        } else {
            int t = -id - varmcp_test_1;
            mask = __test_mcp[ t ].mask;
            pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test_mcp[ t ].state, &col )
            , datmcp_get_title( -id ));
        }
    }
	if ( mask ) {
		/* Кол-во модулей */
		pos += sprintf( s + pos, "  (%d)", popcount( mask ));
		/* Список модулей */
        if(marker){
        mcp_mask2string( mask, s, pos, REPORT_STR_STATIC_LIMIT );
        }
        else   st_mask2string( mask, s, pos, REPORT_STR_STATIC_LIMIT );

	}
	report_message(( focus ) ? REPORT_SUMMARY_FOCUS : REPORT_SUMMARY, line - 1, col, s );
}

static void refresh_summary2( int id, bool focus )
{
    int col = COLOR_DEFAULT;
    char s[ REPORT_STR_LIMIT ];
    int pos;
    uint32_t mask;
    uint8_t line = search_summary( id );

    if ( line == LINE_NOT_FOUND ) return;

    /* Состояние */
    switch ( id ) {
    case ID_MI_OPERATE:
        mask = __mi_mask;
        strcpy( s, mi_operate_template_sum( __mcp_operate, __mcp_operate_last, &col ));
        pos = strlen( s );
        switch ( __mcp_operate ) {
        case MCP_WRITE_ID:
        case MCP_WRITE_GRAD:
            /* Процент выполнения */
            pos += sprintf( s + pos, " %2u%%", data_get_int( var_mcp_percent ));
            break;

        case MCP_OK:
            /* Время выполнения */
            pos += sprintf( s + pos, " %d сек", -data_get_int( var_mcp_percent ));
            break;
        }
        break;

    default:
        if ( id > 0 ) {
            int t = id - varn_test_;

            mask = __test[ t ].mask;
            pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test[ t ].state, &col )
            , datn_get_title( id ));
        } else {
            int t = -id - varm_test_2;
            mask = __test_mi[ t ].mask;
            pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test_mi[ t ].state, &col )
            , datm_get_title( -id ));
        }
    }
    if ( mask ) {
        /* Кол-во модулей */
        pos += sprintf( s + pos, "  (%d)", popcount( mask ));
        /* Список модулей */
        mi_mask2string( mask, s, pos, REPORT_STR_STATIC_LIMIT );
    }
    report_message(( focus ) ? REPORT_SUMMARY_FOCUS : REPORT_SUMMARY, line - 1, col, s );
}


static void refresh_summary3( int id, bool focus )
{
    int col = COLOR_DEFAULT;
    char s[ REPORT_STR_LIMIT ];
    int pos;
    uint32_t mask;
    uint8_t line = search_summary( id );

    if ( line == LINE_NOT_FOUND ) return;

    /* Состояние */
    switch ( id ) {
    case ID_MI_OPERATE:
        mask = __mcp_mask;
        strcpy( s, mi_operate_template_sum( __mcp_operate, __mcp_operate_last, &col ));
        pos = strlen( s );
        switch ( __mcp_operate ) {
        case MCP_WRITE_ID:
        case MCP_WRITE_GRAD:
            /* Процент выполнения */
            pos += sprintf( s + pos, " %2u%%", data_get_int( var_mcp_percent ));
            break;

        case MCP_OK:
            /* Время выполнения */
            pos += sprintf( s + pos, " %d сек", -data_get_int( var_mcp_percent ));
            break;
        }
        break;

    default:
        if ( id > 0 ) {
            int t = id - varn_test_;

            mask = __test[ t ].mask;
            pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test[ t ].state, &col )
            , datn_get_title( id ));
        } else {
            int t = -id - varmcp_test_1;
            mask = __test_mcp[ t ].mask;
            pos = snprintf( s, REPORT_STR_STATIC_LIMIT, test_template_sum( __test_mcp[ t ].state, &col )
            , datmcp_get_title( -id ));
        }
    }
    if ( mask ) {
        /* Кол-во модулей */
        pos += sprintf( s + pos, "  (%d)", popcount( mask ));
        /* Список модулей */
        mcp_mask2string( mask, s, pos, REPORT_STR_STATIC_LIMIT );
    }
    report_message(( focus ) ? REPORT_SUMMARY_FOCUS : REPORT_SUMMARY, line - 1, col, s );
}


void report_message( uint8_t kind, uint8_t ind, int col, const char *s )
{
	REPORT_MESSAGE mg;

	mg.kind = kind;
	mg.ind = ind;
	mg.col = col;
	if ( s ) {
		strcpy( mg.s, s );
	} else {
		mg.s[ 0 ] = '\0';
	}
    pthread_mutex_lock( &report_mutex );
    report_queue.push( mg );
    pthread_mutex_unlock( &report_mutex );
	gui_send_event( EVENT_REPORT );
}

void report_test_result( PENDPROGRESS res, int8_t st )
{
	/* Записать в журнал */
	journal_freeze_time();

	switch ( res->wTestNum ) {

	case TEST_HDD:
	case TEST_HDD_EXP:
		journal_printf( st_list_ex[ st ].name_ch, STR_RESULT_TEST_HDD, res->State[ 0 ], res->State[ 1 ]);
		break;
	}
}

/** \} */
