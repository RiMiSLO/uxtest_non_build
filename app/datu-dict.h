/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datups-dict.h
 *  \brief  Массив состояния ups (словарь).
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page datn Элементы массива состояния станций
 *
 *  \section n_state Переменная state
 *  Текущее состояние станции.
 *
 *  \section n_failure Переменная failure
 *  Флаги неисправностей.
 *
 *  \section n_link Переменная link
 *  Состояние сетевых подключений.
 *
 *  \section n_date Переменная date
 *  Дата.
 *
 *  \section n_time Переменная time
 *  Время.
 *
 *  \section n_test_perform Переменная test_perform
 *  Выполняемая операция.
 *
 *  \section n_test_ Переменная test_*
 *  Результаты выполнения тестов.
 *
 *  \section n_test_percent Переменная test_percent
 *  Процент выполнения операции.
 *
 *  \section n_test_hdd_bad Переменная test_hdd_bad
 *  Количество плохих секторов на диске.
 *
 *  \section n_verify Переменная verify
 *  Верификатор РЗУ.
 *
 *  \section n__ut_ Переменные _ut_*
 *  Используются \ref test "модульными тестами".
 */

/*
 *  DATU_THREAD( thread )
 *  DATU_TYPE( T, format, strto )
 *  DATU_VAR( var, type, title, string )
 *  DATU_ARRAY( len, var, type, title, string )
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *        W A T C H I N G   T H R E A D S        *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATU_THREAD( gui )
DATU_THREAD( report )

#ifdef LINK_UT_DATU
DATU_THREAD( _ut_datu_1 )
DATU_THREAD( _ut_datu_2 )

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      T Y P E S   O F   V A R I A B L E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATU_TYPE( byte,   "%u",    __strtoul )
DATU_TYPE( word,   "%u",    __strtoul )
DATU_TYPE( dword,  "%u",    __strtoll )
DATU_TYPE( qword,  FMT_QWORD,  __strtoull )
DATU_TYPE( float,  "%.2f",  __strtof )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *   S T A T E  V E C T O R  V A R I A B L E S   *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATU_VAR( state                 , byte  , "Состояние"                                   , strn_state ) // sostoianie
DATU_VAR( failure               , word  , "Ёмкость в %"                                 , strn_failure )    // \_(-_-)_/
/*DATU_VAR( date                  , dword , "Дата"                                        , strn_date )
DATU_VAR( time                  , dword , "Время"                                       , strn_time )

DATU_VAR( test_perform         , word  , "Операция (тест)"                            , strn_perform )*/
DATU_VAR( param1               , word  , "Номинальная мощность (Вт)"                                 , strn_snmp1 )  //stru_room
DATU_VAR( param2               , word  , "Входное линейное напряжение "                              , strn_failure )  // stru_name
DATU_VAR( param3               , word  , "Статус"                                               , strn_snmp1 )
//DATU_VAR( param3               , word  , "Состояние устройства"                                 , strn_failure )  // stru_netcart
//DATU_VAR( param4               , word  , "Num of mess"                                 , strn_failure )  // stru_num_of_mess
//DATU_VAR( param5               , word  , "num of"                                 , strn_failure )  // stru_num_of

/*DATU_ARRAY( 2, link             , byte  , "Сеть"                                        , strn_link )*/



#ifdef LINK_UT_DATU
DATU_VAR( _ut_byte              , byte  , "_ut_byte"                                    , strn_byte )
DATU_VAR( _ut_word              , word  , "_ut_word"                                    , strn_word_h )
DATU_ARRAY( 4, _ut_array        , dword , "_ut_array"                                   , strn_dword_h )
DATU_VAR( _ut_qword             , qword , "_ut_qword"                                   , strn_qword )
DATU_VAR( _ut_float             , float , "_ut_float"                                   , strn_float )

#endif
