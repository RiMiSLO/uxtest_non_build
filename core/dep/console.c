/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  console.c
 *  \brief  Debug console.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

#define CONSOLE_IMPLEMENT
#include "include.h"

#ifdef LINK_CONSOLE

/**
 *  \addtogroup console
 *  \{
 */

/** \cond */

#define CONSOLE_CMD( cmd, parn ) \
	__stringify( cmd ),
#define CONSOLE_JOB( cmd, parn ) \
	CONSOLE_CMD( cmd, parn )

const char *const console_command[] = {
	"",
#include "console-dict.h"
};

#undef CONSOLE_CMD
#undef CONSOLE_JOB

const int CONSOLE_COMMAND_COUNT = array_length( console_command );


#define CONSOLE_CMD( cmd, parn ) \
	id_##cmd,
#define CONSOLE_JOB( cmd, parn ) \
	CONSOLE_CMD( cmd, parn )

/**
 *  \brief Console commands identifiers.
 */
enum {
	id_NONE = 0,
#include "console-dict.h"
	id_WRONG
};

#undef CONSOLE_CMD
#undef CONSOLE_JOB


#define T0( N, cmd ) \
	par_##cmd, \
	par_##cmd##_0 = par_##cmd,

#define T( N, cmd ) \
	par_##cmd##_##N,

#define CONSOLE_CMD( cmd, parn ) \
	UNFOLD( parn, T0, T, cmd )
#define CONSOLE_JOB( cmd, parn ) \
	CONSOLE_CMD( cmd, parn )

/**
 *  \brief Command parameters identifiers.
 */
enum {
	par_none = 0,
#include "console-dict.h"
};

#undef T
#undef T0
#undef CONSOLE_CMD
#undef CONSOLE_JOB


#define CONSOLE_CMD( cmd, parn )
#define CONSOLE_JOB( cmd, parn ) \
	cyclic_job_##cmd,

enum {
	cyclic_job_none = 0,
#include "console-dict.h"
};

#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */

/**
 *  \brief Console state flags.
 */
enum {
	stt_CMD_WAIT,  /**< Wait for command. */
	stt_PAR_WAIT,  /**< Wait for command parameter. */
	stt_PAR_PARSE  /**< Handle parameter. */
};

/**
 *  \brief Console state.
 */
static int stt;

static int arg_wanted;
static char *arg_current;
static char arg[ 6 ][ RX_SIZE ];
static int arg_count;
static int arg_i;

enum {
	cyclic_stt_IDLE,
	cyclic_stt_START,
	cyclic_stt_EXEC,
	cyclic_stt_STOP
};

/**
 *  \brief Cyclic command state.
 */
static int cyclic_stt;

/**
 *  \brief Current cyclic command.
 */
static int cyclic_job;

/* Temporary variables */
static int tmp1_int __attribute__(( unused ));
static int tmp2_int __attribute__(( unused ));
static void *tmp3_ptr __attribute__(( unused ));
static uint32_t __uint __attribute__(( unused ));
static char __str[ 100 ] __attribute__(( unused ));

static char *error;

#define msg_ERROR    __CRLF__"*** ERROR - wrong %s ***"
#define msg_CONFIRM  __CRLF__"  confirm %s (1): "

static void parse_cmd( const char *msg );
static void parse_par( const char *msg );
static int search_cmd( const char *msg );
static void wait_par( int par );
static int cyclic_start( void );
static int cyclic_exec( void );
static void cyclic_stop( void );
static bool __partoul( const char *s, uint32_t *result );
static bool __partoull( const char *s, uint64_t *result ) __attribute__(( unused ));

#define START_CYCLIC( job ) do { \
	if ( cyclic_stt == cyclic_stt_IDLE ) { \
		cyclic_job = cyclic_job_##job; \
		cyclic_stt = cyclic_stt_START; \
	} else { \
		if ( cyclic_job == cyclic_job_##job ) { \
			cyclic_stt = cyclic_stt_STOP; \
		} \
	} \
} while ( 0 )

/**
 *  \brief Convert and check unsigned integer parameter.
 *  \hideinitializer
 */
#define msg_to__uint( MIN, MAX ) \
	( __partoul( msg, &__uint ) && ( __uint >= ( MIN )) && ( __uint <= ( MAX )))

/**
 *  \brief Convert and check unsigned integer parameter (MIN is 0).
 *  \hideinitializer
 */
#define msg_to__uint_0( MAX ) \
	( __partoul( msg, &__uint ) && ( __uint <= ( MAX )))

#define handle_cmd( cmd )  static void handle_cmd_##cmd( void )

#define handle_par( cmd, N )  static void handle_par_##cmd##_##N( const char *msg )

#define start_job( cmd )  static int cyclic_start_##cmd( void )

#define exec_job( cmd )  static int cyclic_exec_##cmd( void )

#define stop_job( cmd )  static void cyclic_stop_##cmd( void )

/* Command handles */
#include "console-inc.h"

int console_init( void )
{
	console_index = 0;
	stt = stt_CMD_WAIT;
	arg_count = 0;
	arg_i = 0;

	return ( CO_READY ); /* CO_SKIP */
}

void console_uninit( void )
{
	printf( __CRLF__"*** ( "__PROGRAM__" ) Bye! ***"__CRLF__ );
}

/**
 *  \brief Coroutine of debug console.
 */
CORO_DEFINE( console )
{
	CORO_BEGIN();

	/* Greeting */
	printf(
    __CRLF__"*** ( " __PROGRAM__ " ) Hello! ***"
	__CRLF__ CONSOLE_PROMPT
	);
	flush();

	for ( ; ; ) {
		switch ( stt ) {

		case stt_CMD_WAIT:

			CORO_WAIT( char_received );     /* Wait for received message */
			parse_cmd( char_received );     /* Handle command */
			break;

		case stt_PAR_WAIT:

			CORO_WAIT( char_received );     /* Wait for received message */
			arg_current = char_received;
			/* fallthrough */

		case stt_PAR_PARSE:

			parse_par( arg_current );       /* Handle parameter */
			break;
		}
		char_received = NULL;               /* Reset received message */

		if ( stt == stt_CMD_WAIT ) {
			if ( char_echo ) {
				printf( __CRLF__ CONSOLE_PROMPT );
				flush();
			} else {
				printf( __CRLF__ );
			}
		}
	}
	CORO_END();
}

int console_cyclic_init( void )
{
	cyclic_stt = cyclic_stt_IDLE;

	return ( CO_READY ); /* CO_SKIP */
}

void console_cyclic_uninit( void )
{
}

/**
 *  \brief Coroutine of cyclic console commands.
 */
CORO_DEFINE( console_cyclic )
{
	CORO_LOCAL struct timeval t_cycle = { 0, 0 };

	CORO_BEGIN();

	for ( ; ; ) {
		switch ( cyclic_stt ) {

		case cyclic_stt_START:  /* Initialize cyclic command */

			cyclic_stt = cyclic_start();
			if ( cyclic_stt != cyclic_stt_EXEC ) break;

			char_echo = 0;
			/* fallthrough */

		case cyclic_stt_EXEC:  /* Execute cyclic command */

			if ( clock_time_before( t_cycle )) break;

			cyclic_stt = cyclic_exec();
			if ( cyclic_stt != cyclic_stt_EXEC ) break;

			t_cycle = clock_t_set( ini_cyclic_delay * TICK_PER_SEC );
			break;

		case cyclic_stt_STOP:  /* Stop cyclic command */

			cyclic_stop();
			cyclic_stt = cyclic_stt_IDLE;
			char_echo = 1;
			break;
		}
		CORO_YIELD();
	}
	CORO_END();
}

void console_prompt( void )
{
	if ( char_echo ) {
		if ( stt == stt_CMD_WAIT ) {
			char_clear_input();
			console_index = 0;
			printf( __CRLF__ CONSOLE_PROMPT );
			flush();
		} else {
			printf( __CRLF__ );
		}
	}
}

bool console_command_waited( void )
{
	return (( stt == stt_CMD_WAIT ) && ( cyclic_stt == cyclic_stt_IDLE ));
}

/**
 *  \brief Handle command.
 *  \param Received message.
 */
static void parse_cmd( const char *msg )
{
	char_echo = 1;

	switch( search_cmd( msg )) {

/** \cond */

#define CONSOLE_CMD( cmd, parn ) \
	case id_##cmd: \
		handle_cmd_##cmd(); \
		break;
#define CONSOLE_JOB( cmd, parn ) \
	CONSOLE_CMD( cmd, parn )

#include "console-dict.h"

#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */

	/* Empty command */
	case id_NONE:
		break;

	default:
		printf( __CRLF__"( unknown )" );
	}
}

/**
 *  \brief Handle command parameter.
 *  \param Received message.
 */
static void parse_par( const char *msg )
{
	stt = stt_CMD_WAIT;
	if ( strlen( msg ) == 0 ) return;

	switch ( arg_wanted ) {

/** \cond */

#define T( N, cmd ) \
	case par_##cmd##_##N: \
		error = NULL; \
		handle_par_##cmd##_##N( msg ); \
		if ( error ) goto l_error; \
		break;

#define CONSOLE_CMD( cmd, parn ) \
	UNFOLD( parn, T, T, cmd )
#define CONSOLE_JOB( cmd, parn ) \
	CONSOLE_CMD( cmd, parn )

#include "console-dict.h"

#undef T
#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */

	/* Print error message */
	l_error:
		printf( msg_ERROR, error );
		break;

	default:
		printf( __CRLF__"( unknown )" );
	}
}

/**
 *  \brief Search command which starts with specified string.
 *  \param Received message.
 *  \return Command identifier.
 */
static int search_cmd( const char *msg )
{
	int len;
	int i;
	int result = id_NONE;
	int count = 0;
	char cmd[ RX_SIZE ];

	if ( strlen( msg ) == 0 ) {
		if (( console_index > 0 ) && ( console_index < CONSOLE_COMMAND_COUNT )) {
			/* Command has been selected from list */
			msg = console_command[ console_index ];
			console_index = 0;
		} else {
			/* Empty command */
			return ( id_NONE );
		}
	}
	/* Split command line */
	arg_count = sscanf( msg, "%s %s %s %s %s %s %s", cmd
	, arg[ 0 ], arg[ 1 ], arg[ 2 ], arg[ 3 ], arg[ 4 ], arg[ 5 ]) - 1;
	arg_i = 0;
	if (( len = strlen( cmd )) == 0 ) return ( id_NONE );

	/* Check full match */
	for ( i = 1; i < CONSOLE_COMMAND_COUNT; i++ ) {
		if ( strcmp( cmd, console_command[ i ]) == 0 ) {
			printf( __CRLF__"( %s )", cmd );

			return ( i );  /* Command found */
		}
	}
	/* Checking partial match */
	for ( i = 1; i < CONSOLE_COMMAND_COUNT; i++ ) {
		if ( strncmp( cmd, console_command[ i ], len ) == 0 ) {
			if ( ++count == 1 ) {
				/* First match */
				result = i;
				printf( __CRLF__"( " );
			}
			printf( "%s ", console_command[ i ]);
		}
	}
	if ( count ) {
		printf( ")" );

		return (( count == 1 ) ? result : id_NONE );
	}

	return ( id_WRONG );  /* Command not found */
}

/**
 *  \brief Wait for command parameter.
 *  \param par Parameter identifier.
 */
static void wait_par( int par )
{
	arg_wanted = par;

	if ( arg_i < arg_count ) {
		stt = stt_PAR_PARSE;
		arg_current = arg[ arg_i ];
		printf( arg_current );
		++arg_i;
	} else {
		stt = stt_PAR_WAIT;
		arg_count = 0;
	}
}

/**
 *  \brief Initialize cyclic command.
 *  \return Cyclic command state.
 */
static int cyclic_start( void )
{
	switch ( cyclic_job ) {

/** \cond */

#define CONSOLE_CMD( cmd, parn )
#define CONSOLE_JOB( cmd, parn ) \
	case cyclic_job_##cmd: \
		return cyclic_start_##cmd();

#include "console-dict.h"

#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */

	default:
		return cyclic_stt_IDLE;
	}
}

/**
 *  \brief Execute cyclic command.
 *  \return Cyclic command state.
 */
static int cyclic_exec( void )
{
	switch ( cyclic_job ) {

/** \cond */

#define CONSOLE_CMD( cmd, parn )
#define CONSOLE_JOB( cmd, parn ) \
	case cyclic_job_##cmd: \
		return cyclic_exec_##cmd();

#include "console-dict.h"

#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */

	default:
		return cyclic_stt_IDLE;
	}
}

/**
 *  \brief Stop cyclic command.
 */
static void cyclic_stop( void )
{
	switch ( cyclic_job ) {

/** \cond */

#define CONSOLE_CMD( cmd, parn )
#define CONSOLE_JOB( cmd, parn ) \
	case cyclic_job_##cmd: \
		cyclic_stop_##cmd(); \
		return;

#include "console-dict.h"

#undef CONSOLE_CMD
#undef CONSOLE_JOB

/** \endcond */
	}
}

/**
 *  \brief Convert parameter string to unsigned integer.
 *  \param s String buffer.
 *  \param result Result pointer.
 *  \return Successful converting flag.
 */
static bool __partoul( const char *s, uint32_t *result )
{
	char *end;

	*result = strtoul( s, &end, 0 );
	return ( *end == '\0' );
}

static bool __partoull( const char *s, uint64_t *result )
{
	char *end;

	*result = strtoull( s, &end, 0 );
	return ( *end == '\0' );
}

/** \} */

#endif /* LINK_CONSOLE */
