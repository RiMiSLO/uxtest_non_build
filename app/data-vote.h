/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data-vote.h
 *  \brief  Data voting.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATA_VOTE_H
#define DATA_VOTE_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATA_VOTE_IMPLEMENT )
#include "h-begin.h"

/**
 *  \addtogroup data
 *  \{
 */

/**
 *  \brief Voting of dword values (on exact matching).
 *  \param result Voted value pointer.
 *  \param valid_1 Validity flag of 1st channel.
 *  \param value_1 Value of 1st channel.
 *  \param valid_2 Validity flag of 2nd channel.
 *  \param value_2 Value of 2nd channel.
 *  \param valid_3 Validity flag of 3th channel.
 *  \param value_3 Value of 3th channel.
 *  \return Validity flag of majority opinion.
 */
_EXT_C bool vote_dword( dword *result, bool valid_1, dword value_1, bool valid_2, dword value_2, bool valid_3, dword value_3 );

/**
 *  \brief Voting of word values (on exact matching).
 *  \param result Voted value pointer.
 *  \param valid_1 Validity flag of 1st channel.
 *  \param value_1 Value of 1st channel.
 *  \param valid_2 Validity flag of 2nd channel.
 *  \param value_2 Value of 2nd channel.
 *  \param valid_3 Validity flag of 3th channel.
 *  \param value_3 Value of 3th channel.
 *  \return Validity flag of majority opinion.
 */
_EXT_C bool vote_word( word *result, bool valid_1, word value_1, bool valid_2, word value_2, bool valid_3, word value_3 );

/**
 *  \brief Voting of byte values (on exact matching).
 *  \param result Voted value pointer.
 *  \param valid_1 Validity flag of 1st channel.
 *  \param value_1 Value of 1st channel.
 *  \param valid_2 Validity flag of 2nd channel.
 *  \param value_2 Value of 2nd channel.
 *  \param valid_3 Validity flag of 3th channel.
 *  \param value_3 Value of 3th channel.
 *  \return Validity flag of majority opinion.
 */
_EXT_C bool vote_byte( byte *result, bool valid_1, byte value_1, bool valid_2, byte value_2, bool valid_3, byte value_3 );

/**
 *  \brief Voting of dword values (bit-by-bit).
 *  \param result Voted value pointer.
 *  \param _1 (unused)
 *  \param value_1 Value of 1st channel.
 *  \param _2 (unused)
 *  \param value_2 Value of 2nd channel.
 *  \param _3 (unused)
 *  \param value_3 Value of 3th channel.
 *  \return true.
 */
_EXT_C bool vote_bit_dword( dword *result, bool _1, dword value_1, bool _2, dword value_2, bool _3, dword value_3 );

/** \} */

#include "h-end.h"

#endif /* DATA_VOTE_H */

