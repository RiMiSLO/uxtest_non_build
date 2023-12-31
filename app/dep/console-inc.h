/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  console-inc.h
 *  \brief  Console command handles.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/**
 *  \addtogroup console
 *  \{
 */

/**
 *  \brief Interval of cyclic command execution.
 */
handle_cmd( _delay )
{
    printf(
    __CRLF__"  cyclic_delay = %d"
    __CRLF__"  value (" __stringify( cyclic_delay_MIN )
                   ".." __stringify( cyclic_delay_MAX ) "): "
    , ini_cyclic_delay
    );
    wait_par( par__delay );
}

handle_par( _delay, 0 )
{
	if ( msg_to__uint( cyclic_delay_MIN, cyclic_delay_MAX )) {
		ini_cyclic_delay = __uint;
		//TODO: save
	} else {
		error = "value";
	}
}

#define __D_PER_CYCLE  10

static uint32_t __d_from;
static bool __d_value;
static bool __d_exclude;
static char __d_filter[ RX_SIZE ];
static uint16_t __cyclic_delay;

/**
 *  \brief Application state-space representation.
 */
handle_cmd( data )
{
	__d_value = true;
	START_CYCLIC( data );
}

start_job( data )
{
	__cyclic_delay = ini_cyclic_delay;
	ini_cyclic_delay = 1;
	__d_filter[ 0 ] = '\0';
	__d_exclude = false;
	__d_from = 0;
	if ( arg_count == 1 ) {
		if ( arg[ 0 ][ 0 ] == '-' ) {
			__d_exclude = true;
			if ( arg[ 0 ][ 1 ] != '\0' ) {
				strcpy( __d_filter, arg[ 0 ] + 1 );
			}
		} else {
			strcpy( __d_filter, arg[ 0 ]);
		}
	}
	return cyclic_stt_EXEC;
}

exec_job( data )
{
	char *var;
	int i;
	int j = 0;

	for ( i = __d_from; i < var_COUNT; i++ ) {
		var = data_vartoa( i );

		if (( __d_filter[ 0 ] == '\0' ) || ( !strstr( var, __d_filter ) == __d_exclude )) {
			printf( __CRLF__"        %-22s  %s"
			, var
			, ( __d_value ) ? data_get_string( __str, i ) : data_get_type( i )
			);
			if ( ++j == __D_PER_CYCLE ) {
				__d_from = i + 1;
				return cyclic_stt_EXEC;
			}
		}
	}
	ini_cyclic_delay = __cyclic_delay;
	printf( __CRLF__ CONSOLE_PROMPT );
	flush();
	return cyclic_stt_STOP;
}

stop_job( data )
{

}

/**
 *  \brief Filter of state changing protocol.
 */
handle_cmd( d_flt )
{
	printf( __CRLF__"  filter = (%s) -(%s)"
	, ( *ini_data_prot_filter ) ? ini_data_prot_filter : "*"
	, ( *ini_data_prot_filter_skip ) ? ini_data_prot_filter_skip : ""
	);
	printf( __CRLF__"  value (substr -substr * -): " );
	wait_par( par_d_flt );
}

handle_par( d_flt, 0 )
{
	if ( *msg == '-' ) {
		tmp3_ptr = ini_data_prot_filter_skip;
		++msg;
	} else {
		tmp3_ptr = ini_data_prot_filter;
	}
	if (( *msg == '\0' ) || ( strchr( msg, '*' ) != 0 )) {
		*( char *) tmp3_ptr = '\0';
	} else {
		strcpy( tmp3_ptr, msg );
	}
	printf( __CRLF__"  filter = (%s) -(%s)"
	, ( *ini_data_prot_filter ) ? ini_data_prot_filter : "*"
	, ( *ini_data_prot_filter_skip ) ? ini_data_prot_filter_skip : ""
	);
	//TODO: save
}

/**
 *  \brief Get value of state variable.
 */
handle_cmd( d_get )
{
	printf( __CRLF__"  name: " );
	wait_par( par_d_get );
}

handle_par( d_get, 0 )
{
	tmp1_int = data_atovar( msg );
	if ( tmp1_int < var_COUNT ) {
		printf( __CRLF__"  %s = %s", msg, data_get_string( __str, tmp1_int ));
	} else {
		error = "name";
	}
}

/**
 *  \brief List of state variables.
 */
handle_cmd( d_list )
{
	__d_value = false;
	START_CYCLIC( data );
}

/**
 *  \brief Display protocol of state changing.
 */
handle_cmd( d_prot )
{
	printf(
	__CRLF__"  data_prot = %d"
	__CRLF__"  value (0..1): "
	, ini_data_prot
	);
	wait_par( par_d_prot );
}

handle_par( d_prot, 0 )
{
	if ( msg_to__uint_0( 1 )) {
		ini_data_prot = __uint;
		printf( __CRLF__"  data_prot = %d", ini_data_prot );
	} else {
		error = "value";
	}
}

/**
 *  \brief Set value of state variable.
 */
handle_cmd( d_set )
{
	printf( __CRLF__"  name: " );
	wait_par( par_d_set );
}

handle_par( d_set, 0 )
{
	tmp1_int = data_atovar( msg );
	if ( tmp1_int < var_COUNT ) {
		printf( __CRLF__"  %s = %s", msg, data_get_string( __str, tmp1_int ));
		printf( __CRLF__"  value: " );
		wait_par( par_d_set_1 );
	} else {
		error = "name";
	}
}

handle_par( d_set, 1 )
{
	data_set_string( tmp1_int, msg );
	printf( __CRLF__"  %s = %s", data_vartoa( tmp1_int ), data_get_string( __str, tmp1_int ));
}

/**
 *  \brief List of console commands.
 */
handle_cmd( help )
{
	int i;
	char last = ' ';

	for ( i = 0; i < CONSOLE_COMMAND_COUNT; i++ ) {
		if ( last != *console_command[ i ]) {
			printf( __CRLF__"  " );
			last = *console_command[ i ];
		}
		printf( "%s ", console_command[ i ]);
	}
}

/**
 *  \brief Save program settings.
 */
handle_cmd( ini )
{
	ini_save();
	printf( __CRLF__"*** %s saved ***", ini_path );
}

/**
 *  \brief Quit program.
 */
handle_cmd( quit )
{
	printf( msg_CONFIRM, "quit" );
	arg_count = 0;
	wait_par( par_quit );
}

handle_par( quit, 0 )
{
	if ( msg_to__uint( 1, 1 )) {
		char_echo = 0;
		app_alive = 0;
	}
}

/**
 *  \brief Stop cyclic command execution.
 */
handle_cmd( stop )
{
	if ( cyclic_stt == cyclic_stt_EXEC ) cyclic_stt = cyclic_stt_STOP;
}

/**
 *  \brief Current time.
 */
handle_cmd( time )
{
	if (( arg_count == 1 ) && ( strcmp( arg[ 0 ], "1" ) == 0 )) {
		printf( __CRLF__"  %s", clock_stamp_short( __str ));
	} else {
		START_CYCLIC( time );
	}
}

start_job( time )
{
	return cyclic_stt_EXEC;
}

exec_job( time )
{
	if ( char_echo ) {
		return cyclic_stt_STOP;
	}
	printf( "\r%s    \r", clock_stamp_short( __str ));
	return cyclic_stt_EXEC;
}

stop_job( time )
{

}

#ifdef LINK_UT

/**
 *  \brief Restart unit testing of 2nd stage.
 */
handle_cmd( ut )
{
	ut_2_restart();
}

/**
 *  \brief Unit testing result.
 */
handle_cmd( u_result )
{
	ut_print_statistics();
}

#endif

/**
 *  \brief Program version.
 */
handle_cmd( version )
{
	print_version();
}
handle_cmd( commandNew )
{

        printf("Let's do it");
    }

/** \} */
