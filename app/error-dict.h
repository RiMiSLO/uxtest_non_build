/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  error-dict.h
 *  \brief  Dictionary of error messages.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/*
 *  DATA_ERROR( name, bit, format, repeat )
 */

DATA_ERROR( _RZU_LIMIT      ,  0,  "rzu_read(): RZU_LIMIT"                             ,  LOG_EVER )
DATA_ERROR( _BL_LIMIT       ,  0,  "map_read(): BL_LIMIT"                              ,  LOG_EVER )

DATA_ERROR( ut              ,  1,  "ut_stage_%d is failed"                             ,  LOG_EVER )
DATA_ERROR( unreachable     ,  1,  "unreachable code O_o"                              ,  LOG_EVER )

DATA_ERROR( udp_open        ,  2, "udp_open(%s)"                                       ,  LOG_EVER )
DATA_ERROR( udp_r_len       ,  2, "udp_receive(): %s: ab = %d msg_l = %d (must be %d)" ,  LOG_EVER )
DATA_ERROR( udp_r_crc       ,  2, "udp_receive(): wrong crc net = %d msg_l = %d"       ,  LOG_EVER )
DATA_ERROR( udp_r_abonent   ,  2, "udp_receive(): abonent = %d"                        ,  LOG_EVER )
DATA_ERROR( udp_t_sendto    ,  2, "udp_transmit(): sendto = %d"                        ,  LOG_ONCE )

DATA_ERROR( udp_open_p      ,  2, "udp_open_p() = %d"                                  ,  LOG_EVER )
DATA_ERROR( udp_r_sendto_p  ,  2, "udp_receive(): sendto = %d"                         ,  LOG_EVER )
DATA_ERROR( udp_r_len_p     ,  2, "udp_relay_p(): %s: msg_l = %d (must be %d)"         ,  LOG_EVER )

DATA_ERROR( st_list_len     ,  3,  "st_list_check(): length"                           ,  LOG_EVER )
DATA_ERROR( st_list_read    ,  3,  "st_list_check(): read"                             ,  LOG_EVER )
DATA_ERROR( st_list_data    ,  3,  "st_list_check(): data"                             ,  LOG_EVER )

DATA_ERROR( bl_list_addr    ,  3,  "bl_list_read(): addr = %d gr = %d"                 ,  LOG_EVER )
DATA_ERROR( bl_list_gr      ,  3,  "bl_list_read(): group"                             ,  LOG_EVER )
DATA_ERROR( bl_list_name    ,  3,  "bl_list_read(): block with addr %d must called %s" ,  LOG_EVER )

DATA_ERROR( map_addr        ,  3,  "map_read(): addr = %d"                             ,  LOG_EVER )
DATA_ERROR( map_kind        ,  3,  "map_read(): map[%d].kind is wrong"                 ,  LOG_EVER )
DATA_ERROR( map_st_list     ,  3,  "map_read(): st_list[%d] is empty"                  ,  LOG_EVER )
DATA_ERROR( map_st_tst      ,  3,  "map_read(): st_tst is undefined"                   ,  LOG_EVER )

DATA_ERROR( file_open       ,  4,  "fopen(%s): %s"                                     ,  LOG_EVER )
DATA_ERROR( rzu_read        ,  4,  "rzu_read(%s): can't read directory"                ,  LOG_EVER )
DATA_ERROR( file_is_empty   ,  4,  "%s is empty"                                       ,  LOG_EVER )
DATA_ERROR( file_read       ,  4,  "fread(%s) is failed"                               ,  LOG_EVER )
DATA_ERROR( file_not_found  ,  4,  "%s not found in %s"                                ,  LOG_EVER )
DATA_ERROR( file_more_1     ,  4,  "more than one file %s in %s"                       ,  LOG_EVER )

DATA_ERROR( disk_td_chsum   ,  5, "disk.td checksum is wrong"                          ,  LOG_EVER )
DATA_ERROR( ihex            ,  5, "ihex error: %s"                                     ,  LOG_EVER )
DATA_ERROR( no_hex_name     ,  5, "HEX filename for %d is undefined"                   ,  LOG_EVER )

DATA_ERROR( test_net        ,  6, "test_net: wrong net = %d"                           ,  LOG_EVER )
