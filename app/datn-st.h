/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-st.h
 *  \brief  Структуры данных для сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATN_ST_H
#define DATN_ST_H

#ifdef  DATN_ST_IMPLEMENT
#define DATN_ST_EXT
#define DATN_ST_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATN_ST_EXT extern
#define DATN_ST_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datn
 *  \{
 */

/**
 *  \brief Число тестов ВУ.
 */
#define TEST_COUNT  12 //13 тут может надо добавить

/**
 *  \defgroup pack Пакеты тестов
 *  \{
 */
enum {
    PACK_NONE,     /**< Нет пакета. \hideinitializer */
    PACK_1,
    PACK_2,
    PACK_3,
    PACK_AUTORUN,  /**< Тесты начального включения. \hideinitializer */
    PACK__LIMIT_
};
/** \} */

#pragma pack( 1 )

/**
 *  \brief Пакет тестов.
 */
typedef struct {
    word id;
    TEST test[ TEST_COUNT ];
} PACK_TEST;

#pragma pack()

/**
 *  \brief Число пакетов тестов.
 */
#define PACK_COUNT  ( PACK__LIMIT_ - 2 )  /* Без пакета начального включения */

extern char *pack_title[ PACK__LIMIT_ ];

/**
 *  \defgroup cmd_ex Расширеннные команды (ПУНВД)
 *  \{
 */

#define EX_NONE           0             /**< Нет команды. \hideinitializer */
#define EX_START_TEST     COM_RUN_TEST  /**< Запуск теста. \hideinitializer */
#define EX_STOP_TEST      101           /**< Завершение теста. \hideinitializer */
#define EX_ABORT_TESTING  102           /**< Прервать диагностику. \hideinitializer */
#define EX_START_PACK     103           /**< Запуск пакета тестов. \hideinitializer */

/** \} */

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

#pragma pack( 1 )

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) type var;
#define DATN_ARRAY( len, var, type, title, string ) type var[ len ];

/* Структура вектора состояния. */
typedef struct {
#include "datn-dict.h"
} DATN_ST;

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY

/** \endcond */

#pragma pack()

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif

#define DATN_ST_SIZE sizeof( DATN_ST )

DATN_ST_EXT DATN_ST datn_st;

#ifdef __GNUC__
DATN_ST_EXT void datn_msg2vector( DATN_VECTOR *datn, void *sou );

#endif

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATN_ST_H */
