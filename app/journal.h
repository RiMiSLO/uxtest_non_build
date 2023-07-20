/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  journal.h
 *  \brief  Журнал результатов тестирования.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef JOURNAL_H
#define JOURNAL_H

#ifdef  JOURNAL_IMPLEMENT
#define JOURNAL_EXT
#define JOURNAL_EXT_INIT( dec, init ) \
	dec = init
#else
#define JOURNAL_EXT extern
#define JOURNAL_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup journal Журнал результатов тестирования
 *  \ingroup app
 *  \{
 */

/**
 *  \brief Открыть журнал.
 *  \param name - Имя файла.
 *  \param mode - Режим.
 */
JOURNAL_EXT void journal_open( const char *name, const char *mode = "a+" );
JOURNAL_EXT void journal_openmcp( const char *name, const char *mode = "a+" );
JOURNAL_EXT void journal_closemcp();
JOURNAL_EXT void journal_printmcp( const char *st, const char *s, bool istime = true );
JOURNAL_EXT void journal_printmcp_qstring( const char *st, QString str, bool istime = true );

JOURNAL_EXT void journal_printmcp_enter(  );



/**
 *  \brief Закрыть журнал.
 */
JOURNAL_EXT void journal_close();

/**
 *  \brief Очистить журнал.
 *  \param name - Имя файла.
 */
JOURNAL_EXT void journal_clear( const char *name );

/**
 *  \brief Записать сообщение в журнал.
 *  \param st - Имя рабочей станции.
 *  \param s - Строка.
 *  \param istime - Признак записи времени в журнал.
 */
JOURNAL_EXT void journal_print( const char *st, const char *s, bool istime = true );

/**
 *  \brief Записать сообщение в журнал.
 *  \param st - Имя рабочей станции.
 *  \param format - Форматная строка.
 *  \param ... - Список аргументов.
 */
JOURNAL_EXT void journal_printf( const char *st, const char *format, ... )
	__attribute(( format( printf, 2, 3 )));

/**
 *  \brief Зафиксировать текущее время.
 */
JOURNAL_EXT const char *journal_freeze_time();

/**
 *  \brief Текущее время в строковом формате.
 */
JOURNAL_EXT const char *journal_get_time();

/**
 *  \brief Мьютекс доступа к журналу.
 */
JOURNAL_EXT_INIT( pthread_mutex_t journal_mutex, PTHREAD_MUTEX_INITIALIZER );

/** \} */

#endif /* JOURNAL_H */
