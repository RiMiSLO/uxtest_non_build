/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb-dict.h
 *  \brief  Вектор состояния ячейки (словарь).
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page datb Элементы вектора состояния ячейки
 *
 *  \section b_state Переменная datb_state
 *  Текущее состояние ячейки.
 *
 *  \section b_failure Переменная failure
 *  Признак неисправности.
 *
 *  \section b_chsum Переменная chsum
 *  Признак ошибки конторльной суммы ПО/ИД.
 *
 *  \section b_can_1 Переменная can_1
 *  Признак ошибки сети CAN 1.
 *
 *  \section b_can_2 Переменная can_2
 *  Признак ошибки сети CAN 2.
 *
 *  \section b_serial Переменная serial
 *  Признак неисправности последовательного интерфейса.
 *
 *  \section b_differ_1 Переменная differ_1
 *  Отклонение команды ВУ1.
 *
 *  \section b_differ_2 Переменная differ_2
 *  Отклонение команды ВУ2.
 *
 *  \section b_differ_3 Переменная differ_3
 *  Отклонение команды ВУ3.
 *
 *  \section b_h_link Переменная h_link
 *  Признаки наличия связи с ячейкой по сетям/картам CAN.
 *
 *  \section b_h_link_3 Переменная h_link_3
 *  Обобщенный признаки наличия связи с ячейкой по сетям/картам CAN.
 *
 *  \section b_red Переменная red
 *  Обобщенный признак неисправности (НЕ передается ячейкой).
 */

/*
 *  DATB_VAR( var, pos, mask, title, vote, red, string )
 *  DATB_VAR3( var, pos, mask, offset, vote )
 */

DATB_VAR( state         , 0 , 3 , "Состояние"           , voteh_state, 0, strb_state )

DATB_VAR( failure       , 8 , 1 , "Диагностика"         , voteh_error, 1, strn_failure )
DATB_VAR( chsum         , 9 , 1 , "Контрольная сумма"   , voteh_error, 1, strn_failure )
DATB_VAR( can_1         , 10, 1 , "Порт CAN 1"          , voteh_error, 1, strb_error )
DATB_VAR( can_2         , 11, 1 , "Порт CAN 2"          , voteh_error, 1, strb_error )
DATB_VAR( serial        , 12, 1 , "Послед. интерфейс"   , voteh_error, 2, strb_rserr )
DATB_VAR( differ_1      , 13, 1 , "Команды ВУ1"         , voteh_error, 0, strb_differ )
DATB_VAR( differ_2      , 14, 1 , "Команды ВУ2"         , voteh_error, 0, strb_differ )
DATB_VAR( differ_3      , 15, 1 , "Команды ВУ3"         , voteh_error, 0, strb_differ )
DATB_VAR3( h_link       , 19, 7 , 4                     , voteh_h_link )

DATB_VAR( h_link_3      , 0 , 0 , "Связь с ВУ"          , voteh_h_link, 0, strb_link )
DATB_VAR3( red          , 0 , 0 , 0                     , voteh_error )
