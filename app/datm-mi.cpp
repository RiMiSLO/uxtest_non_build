/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datm-mi.c
 *  \brief  Разбор состояния МИ в сетевом сообщении.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATM_MI_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup datm
 *  \{
 */
void datm_head2vector( uint16_t pool, uint8_t abonent )
{

//    datm_set_byte( varm_failure_hard, DATM_GET( pool, FAILURE_HARD ), abonent );
//    datm_set_byte( varm_failure_soft, DATM_GET( pool, FAILURE_SOFT ), abonent );
//    datm_set_byte( varm_link_0, DATM_GET( pool, LINK_0 ), abonent );
//	datm_set_byte( varm_link_1, DATM_GET( pool, LINK_1 ), abonent );
//    datm_set_byte( varm_net_0, DATM_GET( pool, NET_0 ), abonent );
//    datm_set_byte( varm_net_1, DATM_GET( pool, NET_1 ), abonent );
//    datm_set_byte( varm_eeprom, DATM_GET( pool, EEPROM ), abonent ); // вот это ошибка записи
//    datm_set_byte( varm_cicle, DATM_GET( pool, ERR_CICLE ), abonent );
//	datm_set_byte( varm_differ_1, DATM_GET( pool, DIFFER_1 ), abonent );
//	datm_set_byte( varm_differ_2, DATM_GET( pool, DIFFER_2 ), abonent );
//    datm_set_byte( varm_differ_3, DATM_GET( pool, DIFFER_3 ), abonent );
//     // где нибудь тут вставить Номер первого ми с ошибкой т.е. кинуть красный цвет в ми

//	datm_set_byte( varm_red, !!( pool & DATM_RED ), abonent );
//	if (( datm_get_byte( varm_onelink, abonent )) && ( datm_get_byte( varm_red, abonent ) == 0 )) {
//		datm_set_byte( varm_red, 1, abonent );

//	}
}

void datm_msg2vector( DATM *d, uint8_t abonent )
{
    uint8_t abonentmcp = abonent;
    abonent *= MI_MAX_COUNT;

    datmcp_set_word( varmcp_variant, d->wVarSig, abonentmcp );  // возможно всем мцп надо будет поставить абонентZ только с изначальными знач ф-ии
    datmcp_set_byte( varmcp_test_1, d->bTest1, abonentmcp );
    datmcp_set_byte( varmcp_test_net, d->bTestNet, abonentmcp );
    datmcp_set_byte( varmcp_test_percent, d->bTestPercent, abonentmcp );
    datmcp_set_word( varmcp_verify_p_mcp, d->wCSProg, abonentmcp );
    datmcp_set_word( varmcp_verify_d, d->wCSData, abonentmcp );
    datmcp_set_word( varmcp_verify_g, d->wCSGrad, abonentmcp );
    datmcp_set_word( varmcp_verify_n, d->wCSNet, abonentmcp );
    datmcp_set_dword( varmcp_sn, d->dwSn, abonentmcp );
    datmcp_set_byte( varmcp_mcp_err_st, d->byMIerrors,  abonentmcp); // выявлены ошибки ми

    if ( datmcp_get_byte( varmcp_red, abonentmcp ) == 0 ) {      // узнать нужен ли тут, при ошибке теста
        if (( d->bTest1 == TEST_FAILED ) || ( d->bTest1 == TEST_ERROR )         // выделять красным мцп
         || ( d->bTestNet == TEST_FAILED ) || ( d->bTestNet == TEST_ERROR )) {
            datmcp_set_byte( varm_red, 1, abonentmcp );
        }
    }

    for ( uint8_t i = 0; i < MI_MAX_COUNT; i++){

        if(d->wCSProgMI[i] == 0xffff){
            d->wCSProgMI[i] = 0;
        }
        if(d->wCSComp[i] == 0xffff){
            d->wCSComp[i] = 0;
        }
        if(d->dwSn1[i] == 0xffff){
            d->dwSn1[i] = 0;
        }

        bool bMI_Error = false;
        datmcp_set_word( varm_variant, d->wVarSig, abonent+i );
        datm_set_byte( varm_test_2, d->bTest2[i], abonent+i );
        datm_set_word( varm_verify_p_mi, d->wCSProgMI[i], abonent+i );
        datm_set_word( varm_verify_c, d->wCSComp[i], abonent+i );
        datm_set_dword( varm_sn_mi, d->dwSn1[i], abonent+i );
        datm_set_dword( varm_date_c, d->dwDateComp[i], abonent+i );
        datm_set_dword( varm_time_c, d->dwTimeComp[i], abonent+i );

        // Контроль серийного номера / контрольной суммы

        int index = -1;

        // Поиск серийного номера MI
        for (int ii = 0; ii < mi_count; ii++){
            if (d->dwSn1[i] == mi_comp[ii].wSn){
                // Найден серийный номер
                index = ii;
                break;
            }
        }

        if (index >= 0){
            // Найден серийный номер

            mi_list[abonent+i].sn = mi_comp[index].wSn;       // вот тут могут быть проблемы
            datm_set_dword(varm_col_sn, COLOR_OK, abonent+i);

            // Контроль CRC
            if (d->wCSComp[i] == mi_comp[index].wCSComp){       // тут могут быть пробелмы
                // CRC соответствует
                datm_set_dword(varm_col_verify_c, COLOR_OK, abonent+i);
            }else{
                // CRC некорректная
                datm_set_dword(varm_col_verify_c, COLOR_FAIL, abonent+i);
                bMI_Error = true;
                datm_set_byte(varm_red, 1,abonent+i);
            }
        }else{
            // Серийный номер не найден
            if (mi_comp_count){
                datm_set_dword(varm_col_sn, COLOR_FAIL, abonent+i);
                bMI_Error = true;
            }
        }

        if ( d->byMIerrors & BIT(i)
          || d->bTest2[i] == TEST_FAILED
          || d->bTest2[i] == TEST_ERROR )
            bMI_Error = true;

        datm_set_byte( varm_red, (bMI_Error) ? 1 : 0, abonent+i );
    }
}

void datm_msg2vector_work( struct SIGNALS *d, uint8_t abonent )
{
    uint8_t abonentmcp = abonent;
    abonent *= MI_MAX_COUNT;

    for ( uint8_t i = 0; i < MI_MAX_COUNT; i++){
        datm_set_byte( varm_red, ( d->byMIerrors & BIT(i) ? 1 : 0), abonent+i );

        datm_set_word( varm_verify_p_mi, d->wCSProgMI[i], abonent+i );
        datm_set_word( varm_verify_c, d->wCSComp[i], abonent+i );
        datm_set_byte( varm_test_2, TEST_NONE, abonent+i );
        datm_set_word( varm_variant, d->wVarSig, abonent+i );
    }

    datmcp_set_word( varmcp_variant, d->wVarSig, abonentmcp );
    datmcp_set_byte( varmcp_test_1, TEST_NONE, abonentmcp );
    datmcp_set_byte( varmcp_test_net, TEST_NONE, abonentmcp );
    datmcp_set_byte( varmcp_test_percent, TEST_NONE, abonentmcp );
    datmcp_set_word( varmcp_verify_p_mcp, d->wCSProg, abonentmcp );
    datmcp_set_word( varmcp_verify_d, d->wCSData, abonentmcp );
    datmcp_set_word( varmcp_verify_g, d->wCSGrad, abonentmcp );
    datmcp_set_word( varmcp_verify_n, d->wCSNet, abonentmcp );
    datmcp_set_byte( varmcp_mcp_err_st, d->byMIerrors,  abonentmcp);
}

/** \} */
