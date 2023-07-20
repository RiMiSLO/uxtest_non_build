/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-bl.h
 *  \brief  Разбор состояния блоков в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATB_BL_H
#define DATB_BL_H

#ifdef  DATB_BL_IMPLEMENT
#define DATB_BL_EXT
#define DATB_BL_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATB_BL_EXT extern
#define DATB_BL_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \addtogroup datb
 *  \{
 */

enum {
	REGWORK_VU_OFF,             // Выключено
	REGWORK_VU_ON,              // Включено
	REGWORK_VU_CHECK,           // Проверки
	REGWORK_VU_WORK             // Штатная работа
};

typedef struct
{
	BYTE byState[ 4 ];
} TSTATEUPO_VSSU;

DATB_BL_EXT void datb_msg2vector( uint32_t *sou, int gap, uint8_t abonent );
DATB_BL_EXT void datb_clean_vector( uint8_t abonent );
DATB_BL_EXT void datb_msg2chsum( void *sou, uint8_t abonent );

/** \} */

#endif /* DATB_BL_H */
