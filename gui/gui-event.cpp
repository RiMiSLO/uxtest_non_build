/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-event.cpp
 *  \brief  События обрабатываемые задачей отображения.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define GUI_EVENT_IMPLEMENT
#include "include.h"

/**
 *  \addtogroup gui
 *  \{
 */

static const int gui_watch[] = {
	var_select_rzu,
	var_td_operate,
	var_td_check,
    var_mi_operate,
    var_mcp_operate,
	var_COUNT
};

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
	varn_##var,

#define T( N, len, var, type, title, string ) \
	varn_##var##_##N, \

#define DATN_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

static const int gui_watchn[] = {
#include "datn-dict.h"
	varn_COUNT
};

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */

/** \cond */

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
	varm_##var,

#define T( N, len, var, type, title, string ) \
	varm_##var##_##N, \

#define DATM_ARRAY( len, var, type, title, string ) \
	UNFOLD( len, T, T, len, var, type, title, string )

static const int gui_watchm[] = {
#include "datm-dict.h"
	varm_COUNT
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

/** \endcond */

/** \cond */

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
    varmcp_##var,

#define T( N, len, var, type, title, string ) \
    varmcp_##var##_##N, \

#define DATMCP_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, len, var, type, title, string )

static const int gui_watchmcp[] = {
#include "datmcp-dict.h"
    varmcp_COUNT
};

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

/** \endcond */

void gui_event_init( void )
{
	data_watch( peek_gui, gui_watch );
	datn_watch( peekn_gui, gui_watchn );
	datm_watch( peekm_gui, gui_watchm );
    datmcp_watch( peekmcp_gui, gui_watchmcp );
}

/** \} */
