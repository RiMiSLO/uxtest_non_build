/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datm-mi.h
 *  \brief  Разбор состояния МИ в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATM_MI_H
#define DATM_MI_H

#ifdef  DATM_MI_IMPLEMENT
#define DATM_MI_EXT
#define DATM_MI_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATM_MI_EXT extern
#define DATM_MI_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datm
 *  \{
 */

#define MI_OFF         0x00
#define MI_ON          0x01
#define MI_ON_RESET    0x05
#define MI_WORK        0x08
#define MI_WORK_RESET  0x0D

/**
 *  \defgroup varm_status Битовые поля состояния МИ
 *  \{
 */

#define DATMO_FAILURE_HARD  0
#define DATMT_FAILURE_HARD  1
#define DATMO_FAILURE_SOFT  1
#define DATMT_FAILURE_SOFT  1
#define DATMO_LINK_0        2
#define DATMT_LINK_0        1
#define DATMO_LINK_1        3
#define DATMT_LINK_1        1
#define DATMO_NET           4
#define DATMT_NET           1
#define DATMO_EEPROM        6
#define DATMT_EEPROM        1
#define DATMO_DIFFER_1      8
#define DATMT_DIFFER_1      1
#define DATMO_DIFFER_2      9
#define DATMT_DIFFER_2      1
#define DATMO_DIFFER_3      10
#define DATMT_DIFFER_3      1

/**
 *  \brief Маска битового поля.
 *  \hideinitializer
 */
#define _DATM_MASK( F )  _F_MASK( word, DATM, F )

/**
 *  \brief Получить битовое поле.
 *  \hideinitializer
 */
#define DATM_GET( pool, F )  _F_GET( word, DATM, pool, F )

#define DATM_RED ( \
  _DATM_MASK( FAILURE_HARD ) | _DATM_MASK( FAILURE_SOFT ) \
| _DATM_MASK( LINK_0 ) | _DATM_MASK( LINK_1 ) \
| _DATM_MASK( NET ) \
| _DATM_MASK( EEPROM ))

/** \} */

/**
 *  \defgroup varm_date Дата ( varm_date )
 *  \{
 */

#define DATE_MO_C  8       /**< Смещение поля века. \hideinitializer */
#define DATE_MT_C  0xFF    /**< Шаблон поля века. \hideinitializer */
#define DATE_MO_Y  0       /**< Смещение поля года. \hideinitializer */
#define DATE_MT_Y  0xFF    /**< Шаблон поля года. \hideinitializer */
#define DATE_MO_M  16      /**< Смещение поля месяца. \hideinitializer */
#define DATE_MT_M  0xFF    /**< Шаблон поля месяца. \hideinitializer */
#define DATE_MO_D  24      /**< Смещение поля дня. \hideinitializer */
#define DATE_MT_D  0xFF    /**< Шаблон поля дня. \hideinitializer */

/**
 *  \brief Маска даты.
 *  \hideinitializer
 */
#define DATE_M_MASK( F )  _F_MASK( dword, DATE_M, F )

/**
 *  \brief Получить поле даты.
 *  \hideinitializer
 */
#define DATE_M_GET( pool, F )  _F_GET( dword, DATE_M, pool, F )

/**
 *  \brief Установить поле даты.
 *  \hideinitializer
 */
#define DATE_M_SET( F, value )  _F_SET( dword, DATE_M, F, value )

/** \} */

/**
 *  \defgroup varm_time Время ( varm_time )
 *  \{
 */

#define TIME_MO_H  0     /**< Смещение поля часов. \hideinitializer */
#define TIME_MT_H  0xFF  /**< Шаблон поля часов. \hideinitializer */
#define TIME_MO_M  8     /**< Смещение поля минут. \hideinitializer */
#define TIME_MT_M  0xFF  /**< Шаблон поля минут. \hideinitializer */
#define TIME_MO_S  16    /**< Смещение поля секунд. \hideinitializer */
#define TIME_MT_S  0xFF  /**< Шаблон поля секунд. \hideinitializer */

/**
 *  \brief Маска времени.
 *  \hideinitializer
 */
#define TIME_M_MASK( F )  _F_MASK( dword, TIME_M, F )

/**
 *  \brief Получить поле времени.
 *  \hideinitializer
 */
#define TIME_M_GET( pool, F )  _F_GET( dword, TIME_M, pool, F )

/**
 *  \brief Установить поле времени.
 *  \hideinitializer
 */
#define TIME_M_SET( F, value )  _F_SET( dword, TIME_M, F, value )

/** \} */

#define TEST_MI_COUNT  2  /* Без теста сети */

#define EX_START_TEST_MI  1  /**< Запуск теста. \hideinitializer */
#define EX_STOP_TEST_MI   0  /**< Завершение теста. \hideinitializer */

#pragma pack( 1 )

typedef struct {
	WORD   wVarSig;
	WORD   wCSProg;
    WORD   wCSProgMI[8];
	WORD   wCSData;
	WORD   wCSGrad;
    WORD   wCSComp[8];
	WORD   wCSNet;
	BYTE   bTest1;
    BYTE   bTest2[8];
	BYTE   bTestNet;
	BYTE   bTestPercent;
	_DWORD dwSn;
    _DWORD dwSn1[8];
    _DWORD dwDateComp[8];
    _DWORD dwTimeComp[8];
    BYTE byMIerrors;
} DATM;

#pragma pack()

DATM_MI_EXT void datm_head2vector( uint16_t pool, uint8_t abonent );
DATM_MI_EXT void datm_msg2vector( DATM *d, uint8_t abonent );
DATM_MI_EXT void datm_msg2vector_work( struct SIGNALS *d, uint8_t abonent );
DATM_MI_EXT void datm_clean_vector( uint8_t abonent );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATM_MI_H */
