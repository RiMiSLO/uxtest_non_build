/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  data-error.h
 *  \brief  Error variable.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef DATA_ERROR_H
#define DATA_ERROR_H

/* All headers except standard and dictionaries
   must be included before the following line.
 */
#define _IMPLEMENT defined( DATA_ERROR_IMPLEMENT )
#include "h-begin.h"

/**
 *  \addtogroup data
 *  \{
 */

#define DATA_GET_ERROR()  data_get_word( var_error )

#define DATA_SET_ERROR( mask ) do { \
	data_set_word( var_error, SET_VALUE( DATA_GET_ERROR(), mask )); \
} while ( 0 )

#define DATA_CLEAR_ERROR( mask ) do { \
	data_set_word( var_error, CLEAR_VALUE( DATA_GET_ERROR(), mask )); \
} while ( 0 )

#define DATA_EQUAL_ERROR( mask, value ) do { \
	data_set_word( var_error, EQUAL_VALUE( DATA_GET_ERROR(), mask, value )); \
} while ( 0 )

#define DATA_RESET_ERROR() do { \
	data_set_word( var_error, 0 ); \
} while ( 0 )

/** \} */

#include "h-end.h"

#endif /* DATA_ERROR_H */
