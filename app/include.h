/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  include.h
 *  \brief  All header files.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      S T A N D A R D   L I B R A R I E S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// WIN32 /////////////////////////////////////////
#if defined( __WIN32__ )
#include <timer.h>

//////////////////////////////////////////////////
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#ifdef __cplusplus
#include <queue>
#include <QString>

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *          C O N F I G U R A T I O N            *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "config.h"

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *      C O M M O N   D E F I N I T I O N S      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "unfold.h"
#include "binary.h"
#include "coroutine.h"
#include "define.h"
#include "version.h"
#include "app.h"
#include "co.h"
#include "data.h"
#include "datn.h"
#include "datmcp.h"
#include "datm.h"
#include "datu.h"
#include "error.h"
#include "log.h"
#include "crc.h"

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *          Р Е А Л И З А Ц И Я   З У            *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "sysnet.h"
#include "basenet.h"
#include "upo.h"
#include "crcn.h"
#include "cfile.h"

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *               H A R D W A R E                 *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "clock.h"
#include "udp-desc.h"

#ifdef __cplusplus
#include "message.h"
#include "udp.h"
#include "snmpmess.h"


#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *               D E B U G G I N G               *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ut.h"
#include "getch.h"
#include "char.h"
#include "console.h"

#ifdef __cplusplus

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *            A P P L I C A T I O N              *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "data-error.h"
#include "data-str.h"
#include "data-vote.h"
#include "datn-st.h"
#include "datn-str.h"
#include "datn-desc.h"
#include "datb.h"
#include "datb-vote.h"
#include "datb-str.h"
#include "datb-bl.h"
#include "datm-str.h"
#include "datm-mi.h"
#include "datmcp-mcp.h"
#include "ini.h"
#include "coding.h"
#include "const.h"
#include "gui-event.h"


#ifdef __cplusplus
#include "st.h"
#include "td.h"
#include "bl.h"
#include "mi.h"
#include "mcp.h"
#include "test-net.h"
#include "report.h"
#include "journal.h"



#endif

