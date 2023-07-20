/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-dict.h
 *  \brief  Массив состояния станций (словарь).
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
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
 *  DATN_THREAD( thread )
 *  DATN_TYPE( T, format, strto )
 *  DATN_VAR( var, type, title, string )
 *  DATN_ARRAY( len, var, type, title, string )
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *        W A T C H I N G   T H R E A D S        *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATN_THREAD( gui )
DATN_THREAD( report )

#ifdef LINK_UT_DATN
DATN_THREAD( _ut_datn_1 )
DATN_THREAD( _ut_datn_2 )

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      T Y P E S   O F   V A R I A B L E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATN_TYPE( byte,   "%u",    __strtoul )
DATN_TYPE( word,   "%u",    __strtoul )
DATN_TYPE( dword,  "%u",   __strtoll )
DATN_TYPE( qword,  FMT_QWORD,  __strtoull )
DATN_TYPE( float,  "%.2f",  __strtof )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *   S T A T E  V E C T O R  V A R I A B L E S   *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATN_VAR( state                 , byte  , "Состояние"                                   , strn_state )
DATN_VAR( failure               , word  , "Диагностика"                                 , strn_failure )
DATN_ARRAY( 2, link             , byte  , "Сеть"                                        , strn_link )
DATN_VAR( date                  , dword , "Дата"                                        , strn_date )
DATN_VAR( time                  , dword , "Время"                                       , strn_time )

DATN_VAR( test_perform          , word  , "Операция (тест)"                             , strn_perform )

/* XXX
    Порядок тестов как в NM_TEST!
    Первый test_net!
 */
DATN_VAR( test_net              , byte  , "Тест сетей Ethernet"                         , strn_test )
DATN_VAR( test_keyb             , byte  , "Тест клавиатуры"                             , strn_test )
DATN_VAR( test_ram_exp          , byte  , "Тест ОЗУ (экспресс)"                         , strn_test )
DATN_VAR( test_ram              , byte  , "Тест ОЗУ"                                    , strn_test )
DATN_VAR( test_hdd              , byte  , "Тест диска"                                  , strn_test )
DATN_VAR( test_hdd_exp          , byte  , "Тест диска (экспресс)"                       , strn_test )
DATN_VAR( test_timer            , byte  , "Тест таймеров"                               , strn_test )
DATN_VAR( test_ic               , byte  , "Тест контроллера прерываний"                 , strn_test )
DATN_VAR( test_mouse            , byte  , "Тест манипулятора \"мышь\""                  , strn_test )
DATN_VAR( test_sound            , byte  , "Тест динамика"                               , strn_test )
DATN_VAR( test_video_exp        , byte  , "Тест видеопамяти (экспресс)"                 , strn_test )
DATN_VAR( test_video            , byte  , "Тест видеопамяти"                            , strn_test )
DATN_VAR( test_percent          , byte  , "Процент выполнения теста"                    , strn_percent )
DATN_VAR( test_hdd_bad          , dword , "Количество плохих секторов диска"            , strn_dword )
DATN_VAR( verify                , dword , "Верификатор"                                 , strn_crc32 )

#ifdef LINK_UT_DATN
DATN_VAR( _ut_byte              , byte  , "_ut_byte"                                    , strn_byte )
DATN_VAR( _ut_word              , word  , "_ut_word"                                    , strn_word_h )
DATN_ARRAY( 4, _ut_array        , dword , "_ut_array"                                   , strn_dword_h )
DATN_VAR( _ut_qword             , qword , "_ut_qword"                                   , strn_qword )
DATN_VAR( _ut_float             , float , "_ut_float"                                   , strn_float )

#endif
