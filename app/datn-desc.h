/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-desc.h
 *  \brief  Форматы данных массива состояния станций.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATN_DESC_H
#define DATN_DESC_H

#ifdef  DATN_DESC_IMPLEMENT
#define DATN_DESC_EXT
#define DATN_DESC_EXT_INIT( dec, init ) \
    dec = init
#else
#define DATN_DESC_EXT extern
#define DATN_DESC_EXT_INIT( dec, init ) \
    extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datn
 *  \{
 */

/* XXX
    Первый test_net!
 */
#define varn_test_  varn_test_net

#define TEST__FIRST_  TEST_ETHERNET
#define TEST__LAST_  ( TEST__FIRST_ + TEST_COUNT - 1 )

/**
 *  \defgroup varn_test_ Состояние теста
 *  \{
 */

#define TEST_NONE     0  /**< Тест не выполнялся. \hideinitializer */
#define TEST_ERROR    1  /**< Ошибка запуска теста. \hideinitializer */
#define TEST_PASSED   2  /**< Тест пройден успешно. \hideinitializer */
#define TEST_ABORTED  3  /**< Тест прерван. \hideinitializer */
#define TEST_FAILED   4  /**< Выявлена неисправность. \hideinitializer */
#define TEST_PERFORM  5  /**< Тест в процессе выполнения. \hideinitializer */

/** \} */

/**
 *  \defgroup varn_link Сетевое подключение ( varn_link )
 *  \{
 */

#define LINK_NONE          0  /**< Нет сетевого подключения. \hideinitializer */
#define LINK_CONNECTED     1  /**< Сетевое подключения функционирует. \hideinitializer */
#define LINK_DISCONNECTED  2  /**< Сетевое подключения расстыковано. \hideinitializer */
#define LINK_FAILED        3  /**< Ошибка сетевого подключения. \hideinitializer */

/** \} */

/**
 *  \defgroup varn_test_perform Выполняемая операция ( varn_test_perform )
 *  \{
 */

#define PERFORMO_TEST  0     /**< Смещение поля тест. \hideinitializer */
#define PERFORMT_TEST  0xFF  /**< Шаблон поля тест. \hideinitializer */
#define PERFORMO_PACK  8     /**< Смещение поля пакет. \hideinitializer */
#define PERFORMT_PACK  0xFF  /**< Шаблон поля пакет. \hideinitializer */

/**
 *  \brief Маска выполняемой операции.
 *  \hideinitializer
 */
#define PERFORM_MASK( F )  _F_MASK( word, PERFORM, F )

/**
 *  \brief Получить поле выполняемой операции.
 *  \hideinitializer
 */
#define PERFORM_GET( pool, F )  _F_GET( word, PERFORM, pool, F )

/**
 *  \brief Установить поле даты.
 *  \hideinitializer
 */
#define PERFORM_SET( F, value )  _F_SET( word, PERFORM, F, value )

/** \} */

/**
 *  \defgroup varn_date Дата ( varn_date )
 *  \{
 */

#define DATEO_Y  16      /**< Смещение поля года. \hideinitializer */
#define DATET_Y  0xFFFF  /**< Шаблон поля года. \hideinitializer */
#define DATEO_M  8       /**< Смещение поля месяца. \hideinitializer */
#define DATET_M  0xFF    /**< Шаблон поля месяца. \hideinitializer */
#define DATEO_D  0       /**< Смещение поля дня. \hideinitializer */
#define DATET_D  0xFF    /**< Шаблон поля дня. \hideinitializer */

/**
 *  \brief Маска даты.
 *  \hideinitializer
 */
#define DATE_MASK( F )  _F_MASK( dword, DATE, F )

/**
 *  \brief Получить поле даты.
 *  \hideinitializer
 */
#define DATE_GET( pool, F )  _F_GET( dword, DATE, pool, F )

/**
 *  \brief Установить поле даты.
 *  \hideinitializer
 */
#define DATE_SET( F, value )  _F_SET( dword, DATE, F, value )

/**
 *  \brief Формировать дату.
 *  \hideinitializer
 */
#define DATE_SET_VALUE( y, m, d ) (( dword ) DATE_SET( Y, y ) | DATE_SET( M, m ) | DATE_SET( D, d ))

/** \} */

/**
 *  \defgroup varn_time Время ( varn_time )
 *  \{
 */

#define TIMEO_H  16    /**< Смещение поля часов. \hideinitializer */
#define TIMET_H  0xFF  /**< Шаблон поля часов. \hideinitializer */
#define TIMEO_M  8     /**< Смещение поля минут. \hideinitializer */
#define TIMET_M  0xFF  /**< Шаблон поля минут. \hideinitializer */
#define TIMEO_S  0     /**< Смещение поля секунд. \hideinitializer */
#define TIMET_S  0xFF  /**< Шаблон поля секунд. \hideinitializer */

/**
 *  \brief Маска времени.
 *  \hideinitializer
 */
#define TIME_MASK( F )  _F_MASK( dword, TIME, F )

/**
 *  \brief Получить поле времени.
 *  \hideinitializer
 */
#define TIME_GET( pool, F )  _F_GET( dword, TIME, pool, F )

/**
 *  \brief Установить поле времени.
 *  \hideinitializer
 */
#define TIME_SET( F, value )  _F_SET( dword, TIME, F, value )

/**
 *  \brief Формировать время.
 *  \hideinitializer
 */
#define TIME_SET_VALUE( h, m, s ) (( dword ) TIME_SET( H, h ) | TIME_SET( M, m ) | TIME_SET( S, s ))

/** \} */

/**
 *  \defgroup varn_failure Флаги неисправностей ( varn_failure )
 *  \{
 */

/**
 *  \brief Маска неисправности.
 *  \hideinitializer
 */
#define FAILURE_MASK( F )  (( word ) 1 << TEST_##F )

/**
 *  \brief Получить флаг неисправности.
 *  \hideinitializer
 */
#define FAILURE_GET( pool, F )  ((( word )( pool ) >> TEST_##F ) & 1 )

/**
 *  \brief Установить флаг неисправности.
 *  \hideinitializer
 */
#define FAILURE_SET( F, value ) ((( word )( value ) & 1 ) << TEST_##F )

/** \} */

DATN_DESC_EXT byte bcd2byte( byte bcd );
DATN_DESC_EXT word bcd2word( word bcd );
DATN_DESC_EXT byte byte2bcd( byte bin );
DATN_DESC_EXT word word2bcd( word bin );
DATN_DESC_EXT dword bcd2sec( dword bcd );
DATN_DESC_EXT dword sec2bcd( dword bin );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATN_DESC_H */
