/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-vote.h
 *  \brief  Функции голосование состояния блоков.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATB_VOTE_H
#define DATB_VOTE_H

#ifdef  DATB_VOTE_IMPLEMENT
#define DATB_VOTE_EXT
#define DATB_VOTE_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATB_VOTE_EXT extern
#define DATB_VOTE_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \addtogroup datb
 *  \{
 */
/* Голосование по каналам */
DATB_VOTE_EXT valb_t voteb_state( valb_t value_1, valb_t value_2, valb_t value_3 );
DATB_VOTE_EXT valb_t voteb_failure( valb_t value_1, valb_t value_2, valb_t value_3 );
/* Голосование по ВУ */
DATB_VOTE_EXT uint8_t voteh_state( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 );
DATB_VOTE_EXT uint8_t voteh_error( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 );
DATB_VOTE_EXT uint8_t voteh_h_link( valb_t *value, valb_t value_1, valb_t value_2, valb_t value_3 );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATB_VOTE_H */
