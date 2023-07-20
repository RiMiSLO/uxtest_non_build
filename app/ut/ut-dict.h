/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ut-dict.h
 *  \brief  Dictionary of unit tests.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/*
 *  UT_1( test )
 *  UT_2( test )
 */

#ifdef LINK_UT_UT
UT_1( ut )

#endif

#ifdef LINK_UT_DATA
UT_1( data )

#endif

#ifdef LINK_UT_CORO
UT_2( coro )

#endif

#ifdef LINK_UT_DATA
UT_2( data )

#endif

#ifdef LINK_UT_DATN
UT_2( datn )

#endif
