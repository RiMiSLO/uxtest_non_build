/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-event.h
 *  \brief  События обрабатываемые задачей отображения.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_EVENT_H
#define GUI_EVENT_H

#ifdef  GUI_EVENT_IMPLEMENT
#define GUI_EVENT_EXT
#define GUI_EVENT_EXT_INIT( dec, init ) \
	dec = init
#else
#define GUI_EVENT_EXT extern
#define GUI_EVENT_EXT_INIT( dec, init ) \
	extern dec
#endif

#define EVENT_SELECT_RZU      701
#define EVENT_RZU_CHANGED     702
#define EVENT_REPORT          703
#define EVENT_TD_VERIFY       704
#define EVENT_TD_OPERATE      705
#define EVENT_MI_OPERATE      706
#define EVENT_MI_FILE_ERROR   707
#define EVENT_BL_PROG_OK      708
#define EVENT_BL_PROG_ERROR   709
#define EVENT_DATB_RECEIVED   790
#define EVENT_DATN_RECEIVED   800  /* .. 800 + DATN_DEPTH - 1 */
#define EVENT_STAT_RECEIVED   850  /* .. 850 + DATN_DEPTH - 1 */
#define EVENT_DATM_RECEIVED   900  /* .. 900 + DATM_DEPTH - 1 */
#define EVENT_STAT2_RECEIVED  950  /* .. 950 + DATM_DEPTH - 1 */
#define EVENT_SNMP_MESS       5600
/**
 *  \brief Количество строк обобщенного состояния.
 *  \hideinitializer
 */
#define SUMMARY_COUNT  8

#ifdef __cplusplus
extern "C" {
#endif

GUI_EVENT_EXT void gui_event_init( void );
GUI_EVENT_EXT void gui_send_event( int type );

#ifdef __cplusplus
}
#endif

#endif /* GUI_EVENT_H */
