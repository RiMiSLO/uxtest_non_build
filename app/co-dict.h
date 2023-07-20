/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  co-dict.h
 *  \brief  Dictionary of coroutines.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page coro List of coroutines
 *
 *  \section t_receive_char receive_char
 *  Coroutine of message receiving on debug interface.
 *
 *  \section t_console console
 *  Coroutine of debug console.
 *
 *  \section t_console_cyclic console_cyclic
 *  Coroutine of cyclic console commands.
 *
 *  \section t_ut_2 ut_2
 *  Coroutine of unit testing of 2nd stage.
 */

/*
 *  CORO( name )
 */

#ifdef LINK_CONSOLE
CORO( receive_char )
CORO( console )
CORO( console_cyclic )

#endif

CORO( report )
CORO( test_net )
CORO( td )
CORO( bl )
CORO( mi )
//CORO( snmpmess )

#ifdef LINK_UT
CORO( ut_2 )

#endif
