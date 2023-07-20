/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datm-dict.h
 *  \brief  Массив состояния МИ (словарь).
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page datn Элементы массива состояния МИ
 *
 *  \section n_mode Переменная mode
 *  Режим работы МИ.
 *  ...
 */

/*
 *  DATM_THREAD( thread )
 *  DATM_TYPE( T, format, strto )
 *  DATM_VAR( var, type, title, string )
 *  DATM_ARRAY( len, var, type, title, string )
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *        W A T C H I N G   T H R E A D S        *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATM_THREAD( gui )
DATM_THREAD( report )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      T Y P E S   O F   V A R I A B L E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATM_TYPE( byte,   "%u",    __strtoul )
DATM_TYPE( word,   "%u",    __strtoul )
DATM_TYPE( dword,  "%u",    __strtoll )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *   S T A T E  V E C T O R  V A R I A B L E S   *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATM_VAR( state                 , byte  , "Режим"                                         , strm_state )
DATM_VAR( variant               , word  , "Вариант"                                       , strm_word )
//DATM_VAR( failure_hard          , byte  , "Диагностика аппаратуры"                      , strn_failure )
//DATM_VAR( failure_soft          , byte  , "Диагностика ВПО и ИД"                        , strn_failure )
//DATM_ARRAY( 2, link             , byte  , "Прием по сети"                               , strn_failure )
//DATM_VAR( net                   , byte  , "Сеть"                                        , strb_error )
//DATM_VAR( differ_1              , byte  , "Команды ВУ1"                                 , strb_differ )
//DATM_VAR( differ_2              , byte  , "Команды ВУ2"                                 , strb_differ )
//DATM_VAR( differ_3              , byte  , "Команды ВУ3"                                 , strb_differ )
//DATM_VAR( eeprom                , byte  , "Запись ППЗУ"                                 , strb_error )
//DATM_VAR( test_1                , byte  , "Тест групповой части МИ"                     , strn_test )
DATM_VAR( test_2                , byte  , "Тест измерительной части МИ"                   , strn_test )
//DATM_VAR( test_net              , byte  , "Тест сетей Ethernet"                         , strn_test )
//DATM_VAR( test_percent          , byte  , "Выполнение теста"                    , strn_percent )
DATM_VAR( sn_mi                    , dword , "Заводской номер"                            , strm_sn )
DATM_VAR( verify_p_mi              , word  , "Верификатор ВПО МИ"                         , strm_crc16 )
//DATM_VAR( verify_d              , word  , "Верификатор ИД"                              , strm_crc16 )
//DATM_VAR( verify_g              , word  , "Верификатор таблицы градуировок"             , strm_crc16 )
//DATM_VAR( verify_n              , word  , "Верификатор таблицы сетевых настроек"        , strm_crc16 )
DATM_VAR( verify_c              , word  , "Верификатор таблицы коррект. кодов"            , strm_crc16 )
DATM_VAR( date_c                , dword , "Дата прог. коррект. кодов"                     , strm_date )
DATM_VAR( time_c                , dword , "Время прог. коррект. кодов"                    , strm_time )
DATM_VAR( red                   , byte  , ""                                              , strm_byte )
DATM_VAR( col_sn                , dword , ""                                              , strm_dword )
DATM_VAR( col_verify_c          , dword , ""                                              , strm_dword )
//DATM_VAR( onelink               , byte  , ""                                            , strm_byte )
