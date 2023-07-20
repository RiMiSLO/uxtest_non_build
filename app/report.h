/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  report.h
 *  \brief  Задача формирования отчета.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef REPORT_H
#define REPORT_H

#ifdef  REPORT_IMPLEMENT
#define REPORT_EXT
#define REPORT_EXT_INIT( dec, init ) \
	dec = init
#else
#define REPORT_EXT extern
#define REPORT_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup report Формирование отчета
 *  \ingroup app
 *  \{
 */

#define REPORT_STR_LIMIT        300 // было 256

#define REPORT_JOURNAL          0                 /**< Добавить строку в журнал станции. \hideinitializer */
#define REPORT_SCROLL           1                 /**< Переместить строку сообщения. \hideinitializer */
#define REPORT_SUMMARY          2                 /**< Добавить строку сообщения. \hideinitializer */
#define REPORT_SUMMARY_FOCUS    3                 /**< Добавить и показать строку сообщения. \hideinitializer */
#define REPORT_JOURNAL2         4                 /**< Добавить строку в журнал МИ/MCP. \hideinitializer */

/**
 *  \brief Сообщение со строкой отчета для отображения
 */
typedef struct {
	uint8_t kind;                                 /**< Вид сообщения. */
	uint8_t ind;                                  /**< Номер станции или индекс строки. */
	int col;                                      /**< Цвет сообщения. */
	char s[ REPORT_STR_LIMIT ];                   /**< Строка. */
} REPORT_MESSAGE;

REPORT_EXT void st_mask2string( ST_MASK mask, char *s, int pos, int limit );
REPORT_EXT void mi_mask2string( MI_MASK mask, char *s, int pos, int limit );
REPORT_EXT void mcp_mask2string( MI_MASK mask, char *s, int pos, int limit );


/**
 *  \brief Передача сообщения со строкой отчета для отображения.
 *  \param kind Вид сообщения.
 *  \param ind Номер станции или индекс строки.
 *  \param col Цвет сообщения.
 *  \param s Строка.
 */
REPORT_EXT void report_message( uint8_t kind, uint8_t ind, int col = 0, const char *s = NULL );

/**
 *  \brief Запись в журнал сообщения о результате теста.
 *  \param res Результат.
 *  \param st Номер станции.
 */
REPORT_EXT void report_test_result( PENDPROGRESS res, int8_t st );

/**
 *  \brief Очередь входящих сообщений.
 */
REPORT_EXT std::queue<REPORT_MESSAGE> report_queue;

/**
 *  \brief Мьютекс очереди входящих сообщений.
 */
REPORT_EXT_INIT( pthread_mutex_t report_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#endif /* REPORT_H */
