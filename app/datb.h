/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb.h
 *  \brief  Массив состояния ячеек УПО.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATB_H
#define DATB_H

#ifdef  DATB_IMPLEMENT
#define DATB_EXT
#define DATB_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATB_EXT extern
#define DATB_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \defgroup datb Массив состояния ячеек УПО
 *  \ingroup app
 *
 *  <b>DATB_VAR( var, pos, mask, title, vote, red, string )</b> - Элемент массива состояния ячеек.
 *  \param var Имя элемента.
 *  \param pos Позиция в пуле сообщения.
 *  \param mask Маска значения в пуле сообщения.
 *  \param title Описание элемента.
 *  \param vote Функция голосования.
 *  \param red Признак ошибки.
 *  \param string Функция преобразования к строковому виду.
 *
 *  <b>DATB_VAR3( var, pos, mask, offset, vote )</b> - Элемент массива состояния ячеек (неотображаемый).
 *  \param var Имя элемента.
 *  \param pos Позиция в пуле сообщения.
 *  \param mask Маска значения в пуле сообщения (0 - НЕ передается ячейкой).
 *  \param offset Смещение по номеру канала.
 *  \param vote Функция голосования.
 *
 *  \{
 */

typedef int varb_t;
typedef uint8_t valb_t;

/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
	varb_##var,
#define DATB_VAR3( var, pos, mask, offset, vote ) \
	varb_##var,

/**
 *  \brief Идентификаторы элементов.
 */
enum {
#include "datb-dict.h"
	varb_COUNT
};

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */

/**
 *  \brief Проверить изменение значения элемента.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 */
DATB_EXT int datb_get_changed( const varb_t item, const int bl, const int ch );

/**
 *  \brief Проверить изменение значения элемента хотя бы по одному из каналов.
 *  \param item Элемент.
 *  \param bl Номер блока.
 */
DATB_EXT int datb_get_changed_bl( const varb_t item, const int bl );

/**
 *  \brief Проверить изменение значения хотя бы одного из элементов.
 *  \param bl Номер блока.
 */
DATB_EXT int datb_get_changed_any( const int bl );

/**
 *  \brief Установить признак изменения значения всех элемента.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 */
DATB_EXT void datb_set_all_changed( const int bl, const int ch );

/**
 *  \brief Сбросить признак изменения значения элемента.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 */
DATB_EXT void datb_reset( const varb_t item, const int bl, const int ch );

/**
 *  \brief Сбросить признак изменения значений всех элементов блока.
 *  \param bl Номер блока.
 */
DATB_EXT void datb_reset_all_bl( const int bl );

/**
 *  \brief Установить значение элемента.
 *  \param item Элемент.
 *  \param value Значение.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 *  \param host Номер хоста.
 */
DATB_EXT void datb_set( const varb_t item, valb_t value, const int bl, const int ch, const uint8_t host );

/**
 *  \brief Получить "актуальное" значение (выбранное из значений полученных от трех ВУ) элемента.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 *  \param host Номер хоста.
 *  \return Значение;
 */
DATB_EXT valb_t datb_get( const varb_t item, const int bl, const int ch, const uint8_t host );

/**
 *  \brief Получить "актуальное" значение элемента в строковом виде.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 *  \param [out]col Буфер для цвета.
 */
DATB_EXT char *datb_get_string( const varb_t item, const int bl, const int ch, int *col );

/**
 *  \brief Получить "голосованное" значение элемента.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param vote Функция голосования - varb_t vote( value_1, value_2, value_3 ),
 *  вычисляющая "голосованное" значение по канальным значениям.
 */
DATB_EXT valb_t datb_get_bl( const varb_t item, const int bl, valb_t ( *vote )( valb_t, valb_t, valb_t ));

/**
 *  \brief Получить номер ВУ "актуального" значения элемента.\n
 *  0: Несколько ВУ
 *  1: ВУ1\n
 *  2: ВУ2\n
 *  3: ВУ3\n
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 */
DATB_EXT uint8_t datb_get_host( const varb_t item, const int bl, const int ch );

/**
 *  \brief Получить номер ВУ "актуального" значения элемента в строковом виде.
 *  \param item Элемент.
 *  \param bl Номер блока.
 *  \param ch Номер канала.
 */
DATB_EXT char *datb_get_host_string( const varb_t item, const int bl, const int ch );

/**
 *  \brief Получить описание элемента.
 *  \param item Элемент.
 */
DATB_EXT char *datb_get_title( const varb_t item );

DATB_EXT void datb_clear_updated( const int bl );
DATB_EXT void datb_set_updated( const int bl );
DATB_EXT uint8_t datb_check_updated( int bl );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATB_H */
