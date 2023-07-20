/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data-dict.h
 *  \brief  Dictionary of state-space representation.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page data List of state variables
 *
 *  \section d_error Variable error
 *  Pool of error bits. See errors dictionary \ref error-dict.yu .
 *
 *  \section d__ut_ Variables _ut_*
 *  Used for \ref ut "unit testing".
 */

/*
 *  DATA_THREAD( thread )
 *  DATA_TYPE( T, format, strto )
 *  DATA_VAR( var, type, string )
 *  DATA_ARRAY( len, var, type, string )
 *  DATA_VOTED( var, type, string )
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *        W A T C H I N G   T H R E A D S        *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATA_THREAD( gui )
DATA_THREAD( report )
DATA_THREAD( test_net )
DATA_THREAD( td )
DATA_THREAD( bl )
DATA_THREAD( mi )
DATA_THREAD( mcp )

#ifdef LINK_UT_DATA
DATA_THREAD( _ut_data_1 )
DATA_THREAD( _ut_data_2 )

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      T Y P E S   O F   V A R I A B L E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATA_TYPE( byte,   "%u",       __strtoul )
DATA_TYPE( word,   FMT_WORD,   __strtoul )
DATA_TYPE( dword,  FMT_DWORD,  __strtoll )
DATA_TYPE( float,  "%.2f",     __strtof  )
DATA_TYPE( int,    "%d",       __strtol  )

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *   S T A T E  V E C T O R  V A R I A B L E S   *
 *                ( C O M M O N )                *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

DATA_VAR( error, word, string_word_h )

DATA_VAR( online_mask, dword, string_dword_h )
DATA_VAR( operate_mask, dword, string_dword_h )
DATA_VAR( onelink_mask, dword, string_dword_h )
DATA_VOTED( operate, byte, string_byte )

DATA_VAR( select_rzu, byte, string_byte )

/* test_net */
DATA_VAR( test_net_command, byte, string_byte )
DATA_VAR( test_net_rate, byte, string_byte )

/* td */
DATA_VAR( td_check, byte, string_byte )
DATA_VAR( td_operate, byte, string_byte )
DATA_VAR( td_mask, dword, string_dword_h )
DATA_VAR( td_error, dword, string_dword_h )
DATA_VAR( td_percent, int, string_percent )

/* mi */
DATA_VAR( mi_operate, byte, string_byte )
DATA_VAR( mi_mask, dword, string_dword_h )
DATA_VAR( mi_error, dword, string_dword_h )
DATA_VAR( mi_percent, int, string_percent )

/* mcp */
DATA_VAR( mcp_operate, byte, string_byte )
DATA_VAR( mcp_mask, dword, string_dword_h )
DATA_VAR( mcp_error, dword, string_dword_h )
DATA_VAR( mcp_percent, int, string_percent )

/* bl */
DATA_VAR( bl_addr, byte, string_byte )

#ifdef LINK_UT_DATA
DATA_VAR(       _ut_byte,   byte,   string_byte    )
DATA_VAR(       _ut_word,   word,   string_word_h  )
DATA_ARRAY( 4,  _ut_array,  dword,  string_dword_h )
DATA_VAR(       _ut_float,  float,  string_float   )
DATA_VOTED(     _ut_vote,   byte,   string_byte    )

#endif
