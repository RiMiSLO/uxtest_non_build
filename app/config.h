/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  config.h
 *  \brief  Configuration.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 *  \defgroup config Configuration
 *  \ingroup app
 *  \{
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *             A P P L I C A T I O N             *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*OPT*/
#define TITLE_RU
/**/

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *             D E B U G   T O O L S             *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*OPT*/
#define LOGGING
/*OPT*/
#define LINK_CONSOLE

/**/

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *              U N I T   T E S T S              *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*OPT*/
#define LINK_UT
/**/
#ifdef LINK_UT
/*OPT*/
#define LINK_UT_UT
/*OPT*/
#define LINK_UT_CORO
/*OPT*/
#define LINK_UT_DATA
/*OPT*/
#define LINK_UT_DATN
/**/
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *               F R A M E W O R K               *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *  \brief Don't finish the program even in case all coroutines were completed.
 */
#define PREDICT_ALIVE_CORO_COMPLETED  1

/*
 *  \brief Pause before unit testing of 2nd stage (seconds).
 */
#define PREDICT_UT_2_PAUSE  0

/*
 *  \brief Use ftoa() instead of string format for floating numbers (%f).
 */
#undef PREDICT_USE_FTOA

/** \} */

#endif /* CONFIG_H */
