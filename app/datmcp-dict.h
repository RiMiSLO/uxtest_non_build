/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datmcp-dict.h
 *  \brief  Массив состояния МЦП (словарь).
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page datn Элементы массива состояния МЦП
 *
 *  \section n_mode Переменная mode
 *  Режим работы МЦП.
 *  ...
 */

/*
 *  DATMCP_THREAD( thread )
 *  DATMCP_TYPE( T, format, strto )
 *  DATMCP_VAR( var, type, title, string )
 *  DATMCP_ARRAY( len, var, type, title, string )
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *        W A T C H I N G   T H R E A D S        *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATMCP_THREAD( gui )
DATMCP_THREAD( report )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      T Y P E S   O F   V A R I A B L E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATMCP_TYPE( byte,   "%u",    __strtoul )
DATMCP_TYPE( word,   "%u",    __strtoul )
DATMCP_TYPE( dword,  "%u",    __strtoll )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *   S T A T E  V E C T O R  V A R I A B L E S   *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATMCP_VAR( stateM                , byte  , "Режим"                                       , strm_state   )
DATMCP_VAR( variant               , word  , "Вариант"                                     , strm_word    )
DATMCP_VAR( failure_hard          , byte  , "Диагностика аппаратуры"                      , strn_failure )
DATMCP_VAR( failure_soft          , byte  , "Диагностика ВПО и ИД"                        , strn_failure )
DATMCP_VAR( link0                 , byte  , "Прием по сети 1"                             , strn_failure )
DATMCP_VAR( link1                 , byte  , "Прием по сети 2"                             , strn_failure )
DATMCP_VAR( net0                  , byte  , "Сеть 1"                                      , strb_error   )
DATMCP_VAR( net1                  , byte  , "Сеть 2"                                      , strb_error   )
DATMCP_VAR( eeprom                , byte  , "Запись ППЗУ"                                 , strb_error   )
DATMCP_VAR( cicle                 , byte  , "Ошибка цикла измерений"                      , strn_failure )
DATMCP_VAR( differ_1              , byte  , "Команды ВУ1"                                 , strb_differ  )
DATMCP_VAR( differ_2              , byte  , "Команды ВУ2"                                 , strb_differ  )
DATMCP_VAR( differ_3              , byte  , "Команды ВУ3"                                 , strb_differ  )
DATMCP_VAR( mcp_have_err          , byte  , "Есть ошибки МИ"                              , strn_failure )
DATMCP_VAR( mcp_num_err           , byte  , "Номер первого МИ с ошибкой"                  , strm_have    )
DATMCP_VAR( mcp_err_st            , byte  , "Номера МИ с ошибкой"                         , strm_mi_error_st )

DATMCP_VAR( test_1                , byte  , "Тест МЦП"                                    , strn_test )
//DATMCP_VAR( test_2                , byte  , "Тест МИ"                                   , strn_test )
DATMCP_VAR( test_net              , byte  , "Тест сетей Ethernet"                         , strn_test )
DATMCP_VAR( test_percent          , byte  , "Процент выполнения теста"                    , strn_percent )
DATMCP_VAR( sn                    , dword , "Заводской номер МЦП"                         , strm_sn )
//DATMCP_VAR( sn_mcp                , dword , "Заводской номер МЦП"                       , strm_sn )

DATMCP_VAR( verify_p_mcp          , word  , "Верификатор ВПО МЦП"                         , strm_crc16 )
DATMCP_VAR( verify_d              , word  , "Верификатор ИД"                              , strm_crc16 )
DATMCP_VAR( verify_g              , word  , "Верификатор таблицы градуировок"             , strm_crc16 )
DATMCP_VAR( verify_n              , word  , "Верификатор таблицы сетевых настроек"        , strm_crc16 )
DATMCP_VAR( verify_c              , word  , "Верификатор таблицы коррект. кодов"          , strm_crc16 )
//DATMCP_VAR( date_c              , dword , "Дата прог. коррект. кодов"                   , strm_date )
//DATMCP_VAR( time_c              , dword , "Время прог. коррект. кодов"                  , strm_time )
DATMCP_VAR( red                   , byte  , ""                                            , strm_byte )
DATMCP_VAR( col_sn                , dword , ""                                            , strm_dword )
DATMCP_VAR( col_verify_c          , dword , ""                                            , strm_dword )
DATMCP_VAR( onelink               , byte  , ""                                            , strm_byte )
