/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-bl.cpp
 *  \brief  Разбор состояния блоков в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATB_BL_IMPLEMENT
#include "include.h"

                                                                    /* { 3, 2, 1 }, { 2, 6, 4 }, { 1, 4, 5 } */
static const uint8_t h_link_mask_3[ BL_HOST_COUNT ][ BL_CH_COUNT ] = { { 3, 2, 1 }, { 1, 3, 2 }, { 2, 1, 3 } };
static const uint8_t h_can_3[ BL_HOST_COUNT ][ BL_CH_COUNT ][ 3 ]  = { { { 1, 2, 0 }, { 0, 2, 0 }, { 1, 0, 0 } }
                                                                     , { { 2, 0, 0 }, { 2, 3, 0 }, { 0, 3, 0 } }
                                                                     , { { 0, 1, 0 }, { 3, 0, 0 }, { 3, 1, 0 } } };
static const uint8_t h_link_mask_1[ BL_CH_COUNT ] = { 3, 6, 5 };

/**
 *  \addtogroup datb
 *  \{
 */

#define M_APP_CHSUM   "Контрольная сумма ПО К%d = 0x%08X надо 0x%08X"
#define M_APP_WRONG_CHSUM   "Неверная контрольная сумма ПО К%d."
#define M_APP_DIFFER_CHSUM  "Контрольные суммы ПО каналов блока не совпадают (К%d)."

void datb_msg2vector( uint32_t *sou, int gap, uint8_t abonent )
{
	int i, ch, bl, ii;
	valb_t val, state, state_old, _red, h_mask, h_link;
	uint8_t rs;
	uint8_t gr = st_list_ex[ abonent ].gr;
	uint8_t cnt = gr_list[ gr ].bl_count;
	int host = st_list[ abonent ].byChannel;

/*
	uint8_t *b = ( uint8_t *) sou;
    printf( "\n-------- host = %d - %d cnt = %d --------\n", abonent, host, cnt );
    for ( i = 0; i < cnt; i++ ) {

		for ( ch = 0; ch < BL_CH_COUNT; ch++ ) {
            if (i == 5)
                printf( "%d: %02X %02X %02X %02X\n",ch, b[ 0 ], b[ 1 ], b[ 2 ], b[ 3 ]);
			b += gap * 4;
		}
        //if (i == 5)
          //printf( "\n" );
	}

/*
if (host == 0){
    uint8_t *b = ( uint8_t *) sou;
    printf( "\n-------- host = %d --------\n", host );
    for ( i = 0; i < cnt; i++ ) {

        if (i == 4){
        for ( ch = 0; ch < BL_CH_COUNT; ch++ ) {
            printf( "%d: %02X %02X %02X %02X\n",ch, b[ 0 ], b[ 1 ], b[ 2 ], b[ 3 ]);
            b += gap * 4;
        }
        printf( "\n" );
        }
    }
}
*/
	for ( i = 0; i < cnt; i++ ) {
		bl = gr_list[ gr ].bl[ i ];
		if ( bl == BL_LIMIT ) continue;

/*
if (i == 5){
    printf("==================\nvoted_state_old:%d (%d)  ->  ", datb_get_bl( varb_state, bl, &voteb_state), bl);
    printf("voted_red_old:%d (%d)\n", datb_get_bl( varb_red, bl, &voteb_state), bl);
}
*/
		datb_set_updated( bl );
		rs = bl_list[ bl ].rs;
		for ( ch = 0; ch < BL_CH_COUNT; ch++ ) {
			h_mask = ( ini_net_count > 1 ) ? h_link_mask_3[ host ][ ch ] : h_link_mask_1[ ch ];
			state_old = datb_get( varb_state, bl, ch, host + 1 );
/*
//if (host == 0){
//if (bl == 4){
if (i == 5){
    printf("state_old:%d (%d:%d)  ->  ", state_old, bl, ch);
    printf("red_old:%d (%d:%d)\n", datb_get( varb_red, bl, ch, host + 1 ), bl, ch);
}
//}
*/
			/* Читаем состояние ячейки и подключений... */
/** \cond */

//if (i == 5){
//    h_link = (( ~( *sou ) >> ( 19 )) & h_mask );
//    printf("hlink:%x %x - %x %x\n", h_link, ( ~( *sou ) >> ( 19 )), (*sou >> 19 ), h_mask);
//}

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
            if ( varb_##var == varb_state ) { \
                state = (( *sou >> pos ) & mask ); \
            }


#define DATB_VAR3( var, pos, mask, offset, vote )   \
            if ( varb_##var == varb_h_link ) {      \
                if (ini_net_count == 1)            \
                    h_link = (( ~( *sou ) >> ( pos )) & h_mask ); \
                else                              \
                    h_link = (( ~( *sou ) >> ( pos + offset * host )) & h_mask ); \
            }


#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

//h_link = (( ~( *sou ) >> ( pos + offset * host )) & h_mask ); \



			/* Если нет подключений - ячейка выключена */
            if ( h_link == 0 ) state  = REGWORK_VU_OFF;

//if (host == 0){
//if (bl == 1){
//if (i == 5){
//    printf("HLINK:%d state:%d\n", h_link, state);
//}
//}

			if ( state != REGWORK_VU_OFF ) {
				_red = 0;
				/* ... блок в сети - читаем данные */
#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
				if ( mask ) { \
					val = (( *sou >> pos ) & mask ); \
					datb_set( varb_##var, val, bl, ch, host + 1 ); \
					if ( red == 1 ) _red |= val; \
					if ( red == 2 ) if ( rs & BIT( ch )) _red |= val; \
				}
#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

				datb_set( varb_h_link, h_link, bl, ch, host + 1 );
				val = 0;
				if ( h_link != h_mask ) {
					for ( ii = 0; ii < BL_CH_COUNT; ii++ ) {
						uint8_t b = BIT( ii );
						if ( b & h_mask ) {
							uint8_t h_can = ( ini_net_count > 1 ) ? h_can_3[ host ][ ch ][ ii ] - 1 : ii;
							val |= !( b & h_link ) << h_can;
						}
					}
					_red |= 1;
					datb_set( varb_h_link_3, val, bl, ch, host + 1 );
				} else {
					datb_set( varb_h_link_3, 0, bl, ch, host + 1 );
				}
				datb_set( varb_red, _red, bl, ch, host + 1 );
			} else {
				/*  ... не в сети */
				datb_set( varb_state, REGWORK_VU_OFF, bl, ch, host + 1 ); \
				if ( state_old ) {
					/* Был в сети - обнулить данные */

////if (host == 0){
////if (bl == 4){
//if (i == 5){
//    printf("NEED CLEAR:%d\n", state);
//}
////}

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
					datb_set( varb_##var, 0, bl, ch, host + 1 );
#define DATB_VAR3( var, pos, mask, offset, vote ) \
					datb_set( varb_##var, 0, bl, ch, host + 1 );

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3
					/* Change flag will be reset by dlgMain */
				}
			}
			sou += gap;

////if (host == 0){
////if (bl == 4){
//if (i == 5){
//    printf("state_new:%d (%d:%d)  ->  ", datb_get( varb_state, bl, ch, host + 1 ), bl, ch);
//    printf("red_new:%d (%d:%d)\n", datb_get( varb_red, bl, ch, host + 1 ), bl, ch);
//}
////}
		}

////if (host == 0){
////if (bl == 4){
//if (i == 5){
//    printf("voted_state_new:%d (%d)  ->  ", datb_get_bl( varb_state, bl, &voteb_state), bl);
//    printf("voted_red_new:%d (%d)\n", datb_get_bl( varb_red, bl, &voteb_state), bl);

//    printf("state0:%d -> ", datb_get( varb_state, bl, 0, 0 ));
//    printf("state1:%d -> ", datb_get( varb_state, bl, 0, 1 ));
//    printf("state2:%d -> ", datb_get( varb_state, bl, 0, 2 ));
//    printf("state3:%d\n", datb_get( varb_state, bl, 0, 3 ));
//}
////}

/** \endcond */
 	}
}

void datb_clean_vector( uint8_t abonent )
{
	int i, ch, bl;
	uint8_t gr = st_list_ex[ abonent ].gr;
	uint8_t cnt = gr_list[ gr ].bl_count;
	int host = st_list[ abonent ].byChannel;

	for ( i = 0; i < cnt; i++ ) {
		bl = gr_list[ gr ].bl[ i ];
		if ( bl == BL_LIMIT ) continue;

		for ( ch = 0; ch < BL_CH_COUNT; ch++ ) {

/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
			datb_set( varb_##var, 0, bl, ch, host + 1 );
#define DATB_VAR3( var, pos, mask, offset, vote ) \
			datb_set( varb_##var, 0, bl, ch, host + 1 );

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3
			/* Change flag will be reset by dlgMain */

/** \endcond */
			datb_set_all_changed( bl, ch );
			bl_chsum[ bl ][ ch ].app = 0;
			bl_chsum[ bl ][ ch ].source = 0;
		}
	}
}

void datb_msg2chsum( void *sou, uint8_t abonent )
{
	int i, ch, bl;
    uint32_t expected, chsum;
	bool changed;
	uint8_t gr = st_list_ex[ abonent ].gr;
	uint8_t cnt = gr_list[ gr ].bl_count;
	int host = st_list[ abonent ].byChannel;
	PTSTATEUPO state = ( PTSTATEUPO ) sou;
    char s[ 256 ];


	for ( i = 0; i < cnt; i++ ) {
		bl = gr_list[ gr ].bl[ i ];
		if ( bl == BL_LIMIT ) continue;

		expected = bl_list[ bl ].app;
		for ( ch = 0; ch < BL_CH_COUNT; ch++ ) {
            chsum = ( uint32_t ) state->dwCheckSumPO;


//if (bl == 18)
//    printf("\nRx block %d CRC %08X = %08X %08X\n", bl, state->dwCheckSumPO, bl_list[ bl ].app, state->dwCheckSumID);

			changed = bl_chsum[ bl ][ ch ].app != chsum;
			if ( changed ) {
				journal_freeze_time();
                journal_printf( bl_list[ bl ].name_ch, M_APP_CHSUM, ch + 1, chsum, expected );
			}
			bl_chsum[ bl ][ ch ].app = chsum;
			if ( chsum ) {
				if ( expected ) {
					if ( expected != chsum ) {
						datb_set( varb_chsum, 1, bl, ch, host + 1 );
						datb_set( varb_red, 1, bl, ch, host + 1 );
						if ( changed ) {
							if ( bl_list[ bl ].app ) {
								journal_printf( bl_list[ bl ].name_ch, M_APP_WRONG_CHSUM, ch + 1 );
							} else {
								journal_printf( bl_list[ bl ].name_ch, M_APP_DIFFER_CHSUM, ch + 1 );
							}
						}
					}
				} else {
					expected = chsum;
				}
			}
			bl_chsum[ bl ][ ch ].source = state->dwCheckSumID;
			++state;
		}
	}
}

/** \} */
