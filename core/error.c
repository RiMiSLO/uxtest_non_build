/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  error.c
 *  \brief  Error messages.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define ERROR_IMPLEMENT
#include "include.h"

/** \cond */

#define DATA_ERROR( name, bit, format, repeat ) \
	format,

const char *const ERROR_format[] = {
#include "error-dict.h"
};

#undef DATA_ERROR

/** \endcond */
