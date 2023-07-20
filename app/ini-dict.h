/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  ini-dict.h
 *  \brief  Dictionary of configuration options.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page ini Configuration options
 *
 *  \section i_cyclic_delay cyclic_delay
 *  Interval of cyclic command execution (seconds).
 *
 *  \section i_data_prot data_prot
 *  Display protocol of state changing (flag).
 *
 *  \section i_data_prot_filter data_prot_filter
 *  Protocol displaying filter (substring is contained in variable name).
 *
 *  \section i_data_prot_filter_skip data_prot_filter_skip
 *  Protocol displaying filter (substring is absent in variable name).
 *
 *  \section i_log_path log_path
 *  Log-file name.
 *
 *  \section i_log_level log_level
 *  Logging level.
 */

/*
 *  INI_INT( name, defval )
 *  INI_BOOL( name, defval )
 *  INI_STR( name, len, defval )
 */

INI_INT( cyclic_delay, 5 )
INI_BOOL( data_prot, false )
INI_BOOL( datn_prot, false )
INI_BOOL( datm_prot, false )
INI_BOOL( datmcp_prot, false )


INI_STR( data_prot_filter,       128, "" )
INI_STR( data_prot_filter_skip,  128, "_ut_" )
INI_STR( datn_prot_filter,       128, "" )
INI_STR( datn_prot_filter_skip,  128, "_ut_" )
INI_STR( datm_prot_filter,       128, "" )
INI_STR( datm_prot_filter_skip,  128, "_ut_" )
INI_STR( datmcp_prot_filter,       128, "" )
INI_STR( datmcp_prot_filter_skip,  128, "_ut_" )

INI_QSTR( file_map, "map.ini" )
INI_QSTR( file_upo_prog, "upo_prog.ini" )
INI_QSTR( file_mi, "mi.ini" )
INI_QSTR( file_pack, "pack.ini" )

INI_STR( log_path, 64,__PROGRAM__ ".log" )
INI_INT( log_level, LOG_LEVEL_ERROR )
INI_QSTR( journal_path, "journal.log" )
INI_QSTR( journal_mcp, "journalmcp.txt")

INI_QSTR( dir_rzu, "/КОФ/РЗУ/" )
INI_QSTR( disk_td, "/КОФ/disk.td" )

INI_QSTR( dir_mi_grd, "/ГРАД" )
INI_QSTR( dir_mi_id, "/КОФ/КМСИП/" )
INI_QSTR( mask_mi_grd, "*.grd" )
INI_QSTR( mask_mi_id, "*.id" )
INI_QSTR( file_mi_comp, "/МИ/CRC16_KK.dat" )


// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
INI_QSTR( font_family, "Tahoma" )
INI_INT( font_size, 13 )
INI_INT( cell_font_dec, 2 )
INI_STR( console_coding, 16, "cp866" )
INI_STR( system_coding, 16, "cp1251" )
INI_QSTR( clock, "rundll32 shell32,Control_RunDLL timedate.cpl" )
INI_QSTR( shell, "cmd" )
INI_QSTR( shell_key, "/C" )
INI_QSTR( set_time, "time %1:%2:%3" )

// UNIX //////////////////////////////////////////
#else
INI_QSTR( font_family, "microsoft-tahoma" )
INI_INT( font_size, 12 )
INI_INT( cell_font_dec, 2 )
INI_STR( console_coding, 16, "koi8-r" )
INI_STR( system_coding, 16, "koi8-r" )
INI_QSTR( clock, "elk-esa-date" )
INI_QSTR( shell, "sh" )
INI_QSTR( shell_key, "-c" )
INI_QSTR( set_time, "sudo date +%T -s %1:%2:%3; sudo hwclock --systohc --localtime" ) // тут sudo новое
INI_STR( decor_hotkey, 16, "CTRL+F11" )

//////////////////////////////////////////////////
#endif

INI_INT( gui_bl_init, 1 )
INI_INT( gui_bl_prog_all, 0 )
INI_INT( gui_exit_confirm, 0 )

INI_INT( udp_delay_usec, udp_delay_usec_DEFAULT )
INI_INT( udp_start_delay, udp_start_delay_DEFAULT )

INI_INT( udp_online, udp_online_DEFAULT )
INI_INT( udp_poll, udp_poll_DEFAULT )
INI_INT( udp_poll_first, udp_poll_first_DEFAULT )
INI_INT( udp_poll_mi, udp_poll_mi_DEFAULT )
INI_INT( udp_poll_operate, udp_poll_operate_DEFAULT )
INI_INT( udp_stat_request_sec, udp_stat_request_sec_DEFAULT )

INI_INT( udp_port_in, udp_port_in_DEFAULT )
INI_INT( udp_port_out, udp_port_out_DEFAULT )
INI_INT( udp_port_prot, udp_port_prot_DEFAULT )

INI_INT( net_count, NET_COUNT )
INI_INT( udp_net1, udp_net1_DEFAULT )
INI_INT( udp_net2, udp_net2_DEFAULT )
INI_INT( udp_host, udp_host_DEFAULT )
INI_INT( snmp_host, snmp_host_DEFAULT )
INI_STR( udp_ip_prot, 16, udp_ip_prot_DEFAULT )

INI_BOOL( net_mi, net_mi_DEFAULT )
INI_INT( udp_net1_mi, udp_net1_mi_DEFAULT )
INI_INT( udp_net2_mi, udp_net2_mi_DEFAULT )

INI_INT( test_timer_sec, 10 )
INI_INT( test_hdd_mb, 0 )
INI_INT( test_net_rate, 10 )
INI_INT( test_net_miss, 1 )
INI_INT( time_diff, 2 )

INI_INT( report_start_delay, 2000 )
INI_INT( report_delay, 500 )

INI_INT( td_confirm, 3000 )
INI_INT( bl_confirm, 3000 )
INI_INT( mi_confirm, 10000 )

INI_INT( bl_label_width, 37 )
INI_INT( map_indent_1, 0 )
INI_INT( map_indent_2, 0 )

INI_BOOL( prot_check_new, true )
INI_QSTR( prot_reader, "/opt/preader/preader" )
INI_QSTR( prot_reader_check, "/opt/preader/preader -s" )
