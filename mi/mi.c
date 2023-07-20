/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  mi.c
 *  \brief  Имитатор МИ
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
#define socklen_t int32_t
#include <winsock.h>
#include <wininet.h>

#define bzero( ptr, sz ) memset( ptr, 0, sz )

// UNIX //////////////////////////////////////////
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif

#include "define.h"
#include "crcn.h"
#include "sysnet.h"
#include "basenet.h"
#include "../app/datm-mi.h"

#define IP_MASK_FORMAT  "192.168.%d.%d"
#define MI_SUBNET  110
#define ST_SUBNET  100
#define UDP_PORT_IN  4950
#define UDP_PORT_OUT  4949

#define UDP_MESSAGE_LIMIT  1500

#define HEADER_ID__TST  0x7F

#define ST_ID_TST  3
#define MI_ID_MIN  4
#define MI_ID_MAX  15

int mi_number = MI_ID_MIN;

#define UDP_SERVER_RX_DELAY  10  /* ms */
#define UDP_SERVER_TX_DELAY  1000

uint32_t verify = 0;

enum {
	CMD_STATE = 3,
	CMD_TEST_NET = 10,
	CMD_GET_STAT = 12,
	CMD_STAT = 13,
	CMD_TEST = 30,
};

#pragma pack( 1 )

typedef struct {
	uint16_t wCheckSum;
	char     IdDst;
	char     IdSrc;
	uint8_t  byCommand;
	uint8_t  byState[ 3 ];
	uint32_t dwVeryfi;
	uint32_t dwMessNum;
} MESSAGE_HEADER, *PMESSAGE_HEADER;

typedef struct {
	MESSAGE_HEADER head;
	uint8_t body[ UDP_MESSAGE_LIMIT - sizeof( MESSAGE_HEADER )];
} MESSAGE, *PMESSAGE;

#pragma pack()

#define NET_COUNT  2

typedef struct {
	_DWORD dwDisconnect;
	_DWORD dwLostCarrier;

	_DWORD dwRxGoodFrame;
	_DWORD dwRxOk;
	_DWORD dwRxDisable;
	_DWORD dwRxError;
	_DWORD dwRxErrorPacket;
	_DWORD dwRxMinPacketLength;
	_DWORD dwRxMaxPacketLength;
	_DWORD dwRxErrorCRC;
	_DWORD dwRxFrameAlign;
	_DWORD dwRxOverrun;
	_DWORD dwRxErrResource;

	_DWORD dwTxGoodFrame;
	_DWORD dwTxOk;
	_DWORD dwTxWriteOk;
	_DWORD dwTxDisable;
	_DWORD dwTxLostCarrier;
	_DWORD dwTxAbort;
	_DWORD dwTxMaxCollision;
	_DWORD dwTxUnderrun;
	_DWORD dwTxJabber;
	_DWORD dwTxBusy;

} TNETSTATISTIC;

typedef struct {
	_DWORD num [ NET_COUNT ];
	_DWORD miss[ NET_COUNT ];
} TEST_NET_DATA_ITEM;

typedef struct {
	WORD rate;
	WORD __;
	TNETSTATISTIC stat[ NET_COUNT ];
	TEST_NET_DATA_ITEM st[ 40 ];
} TEST_NET_PACKET;

typedef struct {
	TNETSTATISTIC stat[ NET_COUNT ];
} NET_STAT_PACKET;

typedef struct {
	_DWORD dwMask;
	_DWORD __;
	WORD  wTestCmd;
	WORD  wTestNum;
	WORD  wParam0;
	WORD  wParam1;
} TEST_PACKET;

#define HEADER_SIZE  sizeof( MESSAGE_HEADER )

static int sockfd;
static MESSAGE __r_msg;
static MESSAGE __t_msg;
static struct sockaddr_in __r_addr, __t_addr;

static DATM datm = {
	wVarSig: 1,
	wCSProg: 2,
	wCSData: 3,
	wCSGrad: 4,
	wCSComp: 0xF47D,
	wCSNet: 6,
	bTest1: 0,
	bTest2: 0,
	bTestNet: 0,
	bTestPercent: 0,
	dwSn: 2,
	dwDateComp: 0x10061520,
	dwTimeComp: 0x392613
};

static TNETSTATISTIC stat;
static TEST_NET_PACKET test_net_packet;

#define TEST_NONE     0
#define TEST_ERROR    1
#define TEST_PASSED   2
#define TEST_ABORTED  3
#define TEST_FAILED   4
#define TEST_PERFORM  5

#define __R_ERR_LIMIT  7
static dword __r_next = 0;
static uint8_t __r_error = 0;
static dword __t_last = 0;

static void udp_transmit( uint8_t cmd, void *buf, uint16_t sz );

static int udp_open_sock( char *addr, int port )
{
	int sockfd;
	u_long optval = 1;
	int res;
	char *ip = ( addr ) ? addr : ( char * ) "INADDR_ANY";

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
	WSADATA wsa;
	res = WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	if ( res ) {
		printf( "*** ERROR *** udp_open(%s)\n", ip );
		perror( "    WSAStartup()" );
		printf( "\n" );
		return ( EOF );
	}

//////////////////////////////////////////////////
#endif
	sockfd = socket( PF_INET, SOCK_DGRAM, 0 );
	if ( sockfd < 0 ) {
		printf( "*** ERROR *** udp_open(%s)\n", ip );
		perror( "    socket()" );
		printf( "\n" );

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
		WSACleanup();

//////////////////////////////////////////////////
#endif
		return ( EOF );
	}

	bzero( &__r_addr, sizeof( struct sockaddr_in ));
	__r_addr.sin_family = AF_INET;
	__r_addr.sin_port = htons( port );
	__r_addr.sin_addr.s_addr = ( addr ) ? inet_addr( addr ) : htonl( INADDR_ANY );

	res = bind( sockfd, ( struct sockaddr * ) &__r_addr, sizeof( __r_addr ));
	if ( res < 0 ) {
		printf( "*** ERROR *** udp_open(%s)\n", ip );
		perror( "    bind()" );
		printf( "\n" );
		close( sockfd );

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
		WSACleanup();

//////////////////////////////////////////////////
#endif
		return ( EOF );
	}

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
	setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, ( const char* ) &optval, sizeof( optval ));
	ioctlsocket( sockfd, FIONBIO, &optval );

// UNIX //////////////////////////////////////////
#else
	setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof( optval ));
	fcntl( sockfd, F_SETFL, O_NONBLOCK );

//////////////////////////////////////////////////
#endif

	return ( sockfd );
}

int udp_open( void )
{
	char s[ 32 ];

	sprintf( s, IP_MASK_FORMAT, MI_SUBNET, mi_number + 1 );
	sockfd = udp_open_sock( s, UDP_PORT_IN );

	bzero( &__t_addr, sizeof( struct sockaddr_in ));
	__t_addr.sin_family = AF_INET;
	__t_addr.sin_port = htons( UDP_PORT_IN );
	sprintf( s, IP_MASK_FORMAT, ST_SUBNET, 255 );
	__t_addr.sin_addr.s_addr = inet_addr( s );

	memset( &test_net_packet, 0, sizeof( test_net_packet ));
	memset( &stat, 0xFF, sizeof( stat ));
	stat.dwRxGoodFrame = 0;
	stat.dwRxOk = 0;
	stat.dwRxError = 0;
	stat.dwRxErrorPacket = 0;
	stat.dwRxErrorCRC = 0;
	stat.dwTxGoodFrame = 0;
	stat.dwTxOk = 0;
	stat.dwTxBusy = 0;

	return ( 0 );
}

int udp_close( void )
{
	if ( sockfd ) close( sockfd );

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
	WSACleanup();

//////////////////////////////////////////////////
#endif
	return ( 0 );
}

static int udp_chrono( uint32_t num )
{
	if ( num == __r_next - 1 ) return ( 0 );

	if ( num == 0 ) {
		__r_next = 1;
		__r_error = 0;
		return ( 1 );
	}

	if ( num < __r_next ) {
		if ( ++__r_error == __R_ERR_LIMIT ) {
			__r_next = num + 1;
			__r_error = 0;
			printf( "*** WARNING *** receive: wrong message number\n" );
			return ( 1 );
		}
		return ( 0 );
	}
	__r_next = num + 1;
	__r_error = 0;
	return ( 1 );
}

static int udp_receive( void )
{
	socklen_t addr_l = sizeof( struct sockaddr_in );
	int msg_l = recvfrom( sockfd, ( char* ) &__r_msg, sizeof( __r_msg ), 0, ( struct sockaddr* ) &__r_addr, &addr_l );

	if ( msg_l > 0 ) {
		PMESSAGE_HEADER h = &__r_msg.head;

		stat.dwRxGoodFrame++;

		uint8_t subnet = _AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ];
		 // Bad ADDR (0.0.0.0:0) in ST test msgs workaround...
		if (( subnet != ST_SUBNET ) && ( subnet != 0 )) return ( EOF );

		if ( msg_l < ( int ) sizeof( MESSAGE_HEADER )) {
			printf( "*** ERROR *** receive: HEADER: msg_l = %d\n", msg_l );
			stat.dwRxErrorPacket++;
			return ( EOF );
		}
		if ( h->IdDst != SEND_BI ) return ( EOF );

		uint16_t crc = h->wCheckSum;
		h->wCheckSum = 0;
		if ( crc != CRC16(( uint8_t* ) h, msg_l )) {
			printf( "*** ERROR *** receive: wrong message CRC16\n" );
			stat.dwRxErrorCRC++;
			return ( EOF );
		}
		uint8_t abonent = ( h->IdSrc == HEADER_ID__TST ) ? ST_ID_TST : h->IdSrc;
		uint8_t cmd = h->byCommand;
		if (( cmd != CMD_TEST_NET ) && ( !udp_chrono( h->dwMessNum ))) return ( EOF );
		verify = h->dwVeryfi;
		stat.dwRxOk++;

		msg_l -= HEADER_SIZE;
		switch ( cmd ) {

		case CMD_STATE:
			if (( msg_l == 0 ) || ( msg_l >= ( int ) sizeof( DATM ))) {
/*				printf( "-> CMD_STATE\n" );
 */
/*				udp_transmit( CMD_STATE, &datm, sizeof( DATM ));
 */
			} else {
				printf( "*** ERROR *** receive: CMD_STATE len = %d (%d)\n", msg_l, sizeof( DATM ));
				stat.dwRxErrorPacket++;
			}
			break;

		case CMD_TEST:
			if ( msg_l >= ( int ) sizeof( TEST_PACKET )) {
				TEST_PACKET *d = ( TEST_PACKET *) &__r_msg.body;
				printf( "-> CMD_TEST\n" );
				printf( "   dwMask = %X wTestCmd = %u wTestNum = %u wParam0 = %u wParam1 = %u\n"
				, d->dwMask, d->wTestCmd, d->wTestNum, d->wParam0, d->wParam1 );
				switch ( d->wTestNum ) {
				case TEST_MI_1:
					datm.bTest1 = ( d->wTestCmd ) ? TEST_PERFORM : TEST_PASSED;
					datm.bTestPercent = ( d->wTestCmd ) ? 2 : 0;
					break;

				case TEST_MI_2:
					datm.bTest2 = ( d->wTestCmd ) ? TEST_PERFORM : TEST_ABORTED;
					datm.bTestPercent = ( d->wTestCmd ) ? 50 : 0;
					break;

				case TEST_ETHERNET:
					memset( &test_net_packet, 0, sizeof( test_net_packet ));
					datm.bTestNet = ( d->wTestCmd ) ? TEST_PERFORM : TEST_PASSED;
					datm.bTestPercent = ( d->wTestCmd ) ? 100 : 0;
					break;

				default:
					printf( "*** ERROR *** receive: wrong TEST = %d\n", d->wTestNum );
					stat.dwRxError++;
					break;
				}
			} else {
				printf( "*** ERROR *** receive: CMD_TEST len = %d (%d)\n", msg_l, sizeof( TEST_PACKET ));
				stat.dwRxErrorPacket++;
			}
			break;

		case CMD_GET_STAT:
			printf( "<> CMD_NET_STAT\n" );
			udp_transmit( CMD_STAT, &stat, sizeof( stat ));
			break;

		case CMD_TEST_NET:
			if ( msg_l == ( sizeof( TEST_NET_PACKET ))) {
				printf( "-> CMD_TEST_NET\n" );
				TEST_NET_PACKET *d = ( TEST_NET_PACKET *) &__r_msg.body;
				uint32_t num = d->st[ abonent ].num[ 0 ];
				uint32_t last = test_net_packet.st[ abonent ].num[ 0 ];
				test_net_packet.st[ abonent ].num[ 0 ] = num;
				test_net_packet.st[ abonent ].num[ 1 ] = num;
				if ( num > last ) {
					test_net_packet.st[ abonent ].miss[ 0 ] += num - last - 1;
					test_net_packet.st[ abonent ].miss[ 1 ] = test_net_packet.st[ abonent ].miss[ 0 ];
				}
				if ( abonent == ST_ID_TST ) {
					test_net_packet.st[ mi_number ].num[ 0 ]++;
					test_net_packet.st[ mi_number ].num[ 1 ] = test_net_packet.st[ mi_number ].num[ 0 ];
					udp_transmit( CMD_TEST_NET, &test_net_packet, sizeof( TEST_NET_PACKET ));
				}
			} else {
				printf( "*** ERROR *** receive: CMD_TEST_NET len = %d (%d)\n", msg_l, sizeof( TEST_NET_PACKET ));
				stat.dwRxErrorPacket++;
			}
			break;

		default:
			printf( "*** ERROR *** unknown command = %d\n", cmd );
			stat.dwRxError++;
			break;
		}
	}
	return ( 0 );
}

static void udp_transmit( uint8_t cmd, void *buf, uint16_t sz )
{
	PMESSAGE_HEADER h = &__t_msg.head;
	uint8_t *d = ( uint8_t *) &__t_msg.body;
	int res, l;

	h->IdDst = HEADER_ID__TST;
	h->IdSrc = mi_number;
	h->byCommand = cmd;
	h->byState[ 0 ] = MI_ON;
	h->dwVeryfi = verify;
	if ( cmd != CMD_TEST_NET ) {
		h->dwMessNum = __t_last;
	}
	memcpy( d, buf, sz );
	l = sizeof( MESSAGE_HEADER ) + sz;
	h->wCheckSum = 0;
	h->wCheckSum = CRC16(( PBYTE ) h, l );

	res = sendto( sockfd, ( const char* ) &__t_msg, l, 0, ( struct sockaddr* ) &__t_addr, sizeof( __t_addr ));
	if ( res < 0 ) {
		printf( "*** ERROR *** transmit: sendto = %d\n", errno );
		perror( "    sendto()" );
		printf( "\n" );
		stat.dwTxBusy++;
	} else {
		++__t_last;
		stat.dwTxGoodFrame++;
		stat.dwTxOk++;
	}
}

static int alive = 1;

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

// UNIX //////////////////////////////////////////
#else
static void __sig_handler( int sig )
{
	alive = 0;
}

//////////////////////////////////////////////////
#endif

int main( int argc, char **argv )
{
// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

// UNIX //////////////////////////////////////////
#else
	struct sigaction sigint;

//////////////////////////////////////////////////
#endif
	uint32_t d = 0;

	if ( argc > 1 ) {
		char *end;
		int val = strtol( argv[ 1 ], &end, 0 );
		if (( *end == '\x00' ) && ( val >= MI_ID_MIN ) && ( val <= MI_ID_MAX )) {
			mi_number = val;
		} else {
			printf( "Usage: mi [%d..%d]\n", MI_ID_MIN, MI_ID_MAX );
			return ( 2 );
		}
	}
	printf( "*** Hello! ***\n" );

	if ( udp_open() == EOF ) return ( 1 );

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

// UNIX //////////////////////////////////////////
#else
	sigint.sa_handler = __sig_handler;
	sigemptyset( &sigint.sa_mask );
	sigint.sa_flags = 0;
	sigaction( SIGINT, &sigint, NULL );

//////////////////////////////////////////////////
#endif

	while ( alive ) {
		Sleep( UDP_SERVER_RX_DELAY );
		udp_receive();
		if ( ++d == UDP_SERVER_TX_DELAY / UDP_SERVER_RX_DELAY ) {
/*
			printf( "<- CMD_STATE\n" );
 */
			udp_transmit( CMD_STATE, &datm, sizeof( DATM ));
			d = 0;
		}
	}
	udp_close();
	printf( "*** Bye! ***\n" );
	return ( 0 );
}
