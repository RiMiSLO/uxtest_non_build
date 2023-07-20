/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datmcp-mcp.h
 *  \brief  Разбор состояния МCPИ в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATMCP_MCP_H
#define DATMCP_MCP_H

#ifdef  DATMCP_MCP_IMPLEMENT
#define DATMCP_MCP_EXT
#define DATMCP_MCP_EXT_INIT( dec, init ) \
    dec = init
#else
#define DATMCP_MCP_EXT extern
#define DATMCP_MCP_EXT_INIT( dec, init ) \
    extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datmcp
 *  \{
 */

#define MCP_OFF         0x00
#define MCP_STOP        0x01
#define MCP_IGNORE      0x02
#define MCP_RESET       0x04

#define MCP_OFF         0x00
#define MCP_ON          0x01
#define MCP_ON_RESET    0x05
#define MCP_WORK        0x08
#define MCP_WORK_RESET  0x0D

/**
 *  \defgroup varm_status Битовые поля состояния МИ
 *  \{
 */

#define DATMCPO_FAILURE_HARD  0
#define DATMCPT_FAILURE_HARD  1
#define DATMCPO_FAILURE_SOFT  1
#define DATMCPT_FAILURE_SOFT  1
#define DATMCPO_LINK_0        2
#define DATMCPT_LINK_0        1
#define DATMCPO_LINK_1        3
#define DATMCPT_LINK_1        1
#define DATMCPO_NET_0         4
#define DATMCPT_NET_0         1
#define DATMCPO_NET_1         5
#define DATMCPT_NET_1         1
#define DATMCPO_EEPROM        6
#define DATMCPT_EEPROM        1
#define DATMCPO_ERR_CICLE     7
#define DATMCPT_ERR_CICLE     1
#define DATMCPO_DIFFER_1      8
#define DATMCPT_DIFFER_1      1
#define DATMCPO_DIFFER_2      9
#define DATMCPT_DIFFER_2      1
#define DATMCPO_DIFFER_3      10
#define DATMCPT_DIFFER_3      1
#define DATMCPO_HAVE_MI_ERR   11
#define DATMCPT_HAVE_MI_ERR   1
#define DATMCPO_NUM_MI_ERR    12
#define DATMCPT_NUM_MI_ERR    1
#define DATMCPO_ERROR_ST      13
#define DATMCPT_ERROR_ST      1


/**
 *  \brief Маска битового поля.
 *  \hideinitializer
 */
#define _DATMCP_MASK( F )  _F_MASK( word, DATMCP, F )

/**
 *  \brief Получить битовое поле.
 *  \hideinitializer
 */
#define DATMCP_GET( pool, F )  _F_GET( word, DATMCP, pool, F )

#define DATMCP_RED ( \
  _DATMCP_MASK( FAILURE_HARD ) | _DATMCP_MASK( FAILURE_SOFT ) \
| _DATMCP_MASK( LINK_0 ) | _DATMCP_MASK( LINK_1 ) \
| _DATMCP_MASK( NET_0 )  | _DATMCP_MASK( NET_1 ) \
| _DATMCP_MASK( EEPROM ))

/** \} */

/**
 *  \defgroup varmcp_date Дата ( varmcp_date )
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
 *  \defgroup varmcp_time Время ( varmcp_time )
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

#define TEST_MCP_COUNT  2  /**< Без теста сети. \hideinitializer */
#define EX_START_TEST_MCP  1  /**< Запуск теста. \hideinitializer */
#define EX_STOP_TEST_MCP   0  /**< Завершение теста. \hideinitializer */

#pragma pack( 1 )

typedef struct {
    WORD   wVarSig;
    WORD   wCSProg;
    WORD   wCSProgMI[8];
    WORD   wCSData;
    WORD   wCSGrad;
    WORD   wCSComp[8];
    WORD   wCSNet;
    BYTE   bTest1;  // состояние МЦП ШИ
    BYTE   bTest2[8];  // состояние МИ
    BYTE   bTestNet;
    BYTE   bTestPercent;
    _DWORD dwSn; // мцп заводской номер
    _DWORD dwSn1[8]; // MI заводсвкой номер
    _DWORD dwDateComp[8];
    _DWORD dwTimeComp[8];
    BYTE   byMIerrors;
} DATMCP;

#pragma pack()

DATMCP_MCP_EXT void datmcp_head2vector( uint16_t pool, uint8_t abonent );
DATMCP_MCP_EXT void datmcp_msg2vector( DATMCP *d, uint8_t abonent );
DATMCP_MCP_EXT void datmcp_msg2vector_work( struct SIGNALS *d, uint8_t abonent );
DATMCP_MCP_EXT void datmcp_clean_vector( uint8_t abonent );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATMCP_MI_H */
