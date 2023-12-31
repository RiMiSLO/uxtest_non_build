/*                 _ _     _
     ___ ___ ___ _| |_|___| |_
    |   |  _   _|   | |  _   _|
    |  _|_| |___ ___|_|___  |
    |_|                   |_|
 *//**
 *  \file  console-dict.h
 *  \brief  Dictionary of console commands.
 *  \author  Vitaly Kravtsov
 *  \copyright  See the LICENSE file.
 */

/**
 *  \page console Console commands
 *  <br><i>Common console commands</i><br>
 *
 *  \section c__delay _delay
 *  Interval of cyclic command execution.
 *
 *  \section c_data data
 *  Application state-space representation.
 *  \code
 *  data s -s
 *  s     - filter: substring (s) is contained in variable name
 *  -s    - filter: substring (s) is absent in variable name
 *  \endcode
 *
 *  \section c_d_flt d_flt
 *  Filter of state changing protocol.
 *  \code
 *  d_flt s -s * -
 *  s     - substring (s) is contained in variable name
 *  -s    - substring (s) is absent in variable name
 *  *     - reset "substring is contained" filter
 *  -     - reset "substring is absent" filter
 *  \endcode
 *
 *  \section c_d_get d_get
 *  Get value of state variable.
 *
 *  \section c_d_list d_list
 *  List of state variables, see \ref c_data "data".
 *  \code
 *  d_list s -s
 *  \endcode
 *
 *  \section c_d_prot d_prot
 *  Display state changing protocol.
 *
 *  \section c_d_set d_set
 *  Set value of state variable.
 *
 *  \section c_help help
 *  List of console commands.
 *
 *  \section c_ini ini
 *  Save program settings.
 *
 *  \section c_quit quit
 *  Quit program.
 *
 *  \section c_stop stop
 *  Stop cyclic command execution.
 *
 *  \section c_time time
 *  Current time.
 *  \code
 *  time 1
 *  1     - display once;
 *  \endcode
 *
 *  \section c_ut ut
 *  Restart unit testing of 2nd stage.
 *
 *  \section c_u_result u_result
 *  Unit testing result.
 *
 *  \section c_version version
 *  Program version.
 *
 *  \section c_commandNew commandNew
 *  New command
 */

/*
 *  CONSOLE_CMD( cmd, parn )
 *  CONSOLE_JOB( cmd, parn )
 */

CONSOLE_CMD( _delay,    1 )

CONSOLE_JOB( data,      0 )
CONSOLE_CMD( d_flt,     1 )
CONSOLE_CMD( d_get,     1 )
CONSOLE_CMD( d_list,    0 )
CONSOLE_CMD( d_prot,    1 )
CONSOLE_CMD( d_set,     2 )

CONSOLE_CMD( help,      0 )
CONSOLE_CMD( ini,       0 )
CONSOLE_CMD( quit,      1 )
CONSOLE_CMD( stop,      0 )
CONSOLE_JOB( time,      0 )


#ifdef LINK_UT
CONSOLE_CMD( ut,        0 )
CONSOLE_CMD( u_result,  0 )

#endif
CONSOLE_CMD( version,   0 )

