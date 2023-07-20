/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  udp.cpp
 *  \brief  Задача сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define UDP_IMPLEMENT
#include "include.h"

#include <QDebug>
#include <fcntl.h>

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
#define socklen_t int32_t
#include <winsock.h>
#include <wininet.h>

#define bzero( ptr, sz ) memset( ptr, 0, sz );

// UNIX //////////////////////////////////////////
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////5////////
#endif

#include "syserror.h"
#include "crc.h"
#include "snmp.h"
#include <memory>

/**
 *  \addtogroup udp
 *  \{
 */

#define HEADER_SIZE         sizeof( struct MESSNETHEAD )
#define EX_HEADER_SIZE      sizeof( EX_HEADER )

#define CIOR_I_OFFSET       offsetof( struct CIOREQUEST, Inputs )

#define AB_MASK_ERROR       BIT( AB_ERROR )
#define AB_MASK_ERROR_ID    BIT( AB_ERROR_ID )
#define AB_MASK_ERROR_NET1  BIT( AB_ERROR_NET1 )
#define AB_MASK_ERROR_NET2  BIT( AB_ERROR_NET2 )
#define AB_MASK_ERGETNET_1  BIT( AB_ERGETNET_1 )
#define AB_MASK_ERGETNET_2  BIT( AB_ERGETNET_2 )

#define AB_ERROR_2_LINK( e, N ) ( \
	(( e ) & AB_MASK_ERROR_NET##N ) ? LINK_DISCONNECTED : \
	(( e ) & AB_MASK_ERGETNET_##N ) ? LINK_FAILED : \
	LINK_CONNECTED \
)
#define __R_ERR_LIMIT 12                         /**< Лимит ошибок нумерации сообщений. \hideinitializer */
#define __T_ERR_LIMIT 12                         /**< Лимит ошибок передачи сообщений. \hideinitializer */

//EOF=-1;
static int sockfd_in = EOF;                       /**< Дескриптор сокета на прием. */
static int sockfd_snmp_in = EOF;                  /**< Дескриптор сокета на прием snmp трафика от ибп. */
static int sockfd_out1 = EOF;                     /**< Дескриптор сокета на передачу в сеть 1. */
static int sockfd_out2 = EOF;                     /**< Дескриптор сокета на передачу в сеть 2. */
static struct MESSNET __r_msg;                    /**< Буфер приема данных. */
static char snmp_buf[MAX_NET_DATA_SIZE]{};        /**< Буфер приема snmp данных. */
static char snmp_buf1[MAX_NET_DATA_SIZE]{};        /**< Буфер приема snmp данных. */

static struct MESSNET __t_msg;                    /**< Буфер передачи данных. */
static struct sockaddr_in __r_addr, __t_addr_0, __t_addr_1, __t_addr_2, __t_addr_3, __t_addr_4, ups_addr;  /**< Адреса. */

static int sockfd_p = EOF;                        /**< Дескриптор сокета для обмена с программой чтения протокола. */
static struct sockaddr_in __t_addr_p;

static dword __r_next[ MAX_QMRMO ];               /**< Счетчики входящих сообщений. */
static uint8_t __r_error[ MAX_QMRMO ];            /**< Счетчики ошибок нумерации сообщений. */
static dword __t_messnum;                         /**< Счетчики исходящих сообщений. */
static uint8_t __t_error[ 5 ];                    /**< Счетчик ошибок передачи сообщений. */

static uint32_t __online[ NET_COUNT ];            /**< Флаги присутствия станций и МИ в сети. */
static uint32_t __operate;                        /**< Флаги - реализация ЗУ. */
static struct timeval __online_to;                /**< Таймер сброса состояния станций при отсутствии связи. */
static struct timeval __poll_to;                  /**< Таймер опроса новых станций в сети. */
static struct timeval __poll_to2;                 /**< Таймер опроса новых МИ в сети. */
static struct timeval __stat_request_to;          /**< Таймер запроса статистики. */
static DATN_VECTOR __datn;                        /**< Буфер вектора состояния. */

#define CHRONO_WRONG   -2
#define CHRONO_REPLAY  -1
#define CHRONO_OK       1
#define CHRONO_FIRST    2
#define CHRONO_MISSED   3
#define CHRONO_TIMEOUT  4

static int udp_open_sock( char *addr, int port, u_long broadcast=1);
static bool udp_open( void );
static void udp_close( void );
static void udp_receive( void );
static void snmp_receive( void );
static void udp_transmit( void );
static void udp_poll( void );
static void udp_stat_request( void );
static int udp_chrono( uint8_t st, dword num );
static MESSAGE_REMOTE *__queue_front( void );
static void __queue_pop( void );

static void udp_open_p( void );
static void udp_relay_p( void );
static void udp_sendto_p( void *msg, int len );
static void udp_close_p( void );

static __inline void __reset_online( void )
{
	__online[ 0 ] = 0;
	__online[ 1 ] = 0;
}

static __inline uint32_t __get_online( void )
{
	return ( __online[ 0 ] | __online[ 1 ]);
}

static __inline uint32_t __only_1_online( void )
{
	return ( __online[ 0 ] ^ __online[ 1 ]);
}

/**
 *  \brief Инициализация состояния задачи.
 */
static void udp_init_state( void )
{
	__t_msg.Head.IdSrc = HEADER_ID__TST;
	__t_msg.Head.bNodeState = PKF;
	__t_msg.Head.SE_VS[ 0 ] = 0;
	__t_msg.Head.SE_VS[ 1 ] = 0;

	__reset_online();
	__operate = 0;
	__online_to = clock_t_set( ini_udp_online );
	__poll_to = clock_t_set( ini_udp_poll_first );  /* Увеличенный таймаут при запуске */
	__poll_to2 = clock_t_set( ini_udp_poll_mi );
	__stat_request_to = clock_t_set( ini_udp_stat_request_sec * 1000 );
    __t_messnum = 0;
}

/**
 *  \brief Инициализация задачи.
 */
int udp_init( void )
{
	return ( 1 );
}

/**
 *  \brief Деинициализация задачи.
 */
void udp_uninit( void )
{
}

/**
 *  \brief Задача обмена сетевыми сообщениями по UDP.
 */
void udp_thread( void )
{
    /* Пауза */
    Sleep( ini_udp_start_delay );

    if ( !udp_open()) return;

    udp_open_p();

    while ( app_alive ) {
        /* Работаем по таймеру */
        usleep( ini_udp_delay_usec );
        //snmp_receive();
        udp_receive();                            /* Прием сообщений */
        udp_relay_p();                            /* Ретрансляция сообщений протокола */
        udp_transmit();                           /* Передача сообщений */
        udp_poll();                               /* Опрос станций */
        udp_stat_request();                       /* Запрос статистики */
    }
    udp_close();
    udp_close_p();
}

static int udp_open_sock( char *addr, int port, u_long broadcast)
{
    int sockfd; // sock descritor
//	u_long optval = 1;
    u_long optval = broadcast;
	int res;
    char *ip = ( addr ) ? addr : ( char * ) "INADDR_ANY";

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
	WSADATA wsa;
	res = WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	if ( res ) {
		ERROR_MESSAGE( udp_open, ip );
		perror( __CRLF__"    WSAStartup()" );
		return ( EOF );
	}

//////////////////////////////////////////////////
#endif
	sockfd = socket( PF_INET, SOCK_DGRAM, 0 );
	if ( sockfd < 0 ) {
		ERROR_MESSAGE( udp_open, ip );
		perror( __CRLF__"    socket()" );

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
    __r_addr.sin_addr.s_addr = ( addr ) ? inet_addr( addr ) :htonl( INADDR_ANY );
    setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &optval, sizeof( optval ));
	res = bind( sockfd, ( struct sockaddr *) &__r_addr, sizeof( __r_addr ));
	if ( res < 0 ) {
		ERROR_MESSAGE( udp_open, ip );
		perror( __CRLF__"    bind()" );
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
    u_long val = 1;
    ioctlsocket( sockfd, FIONBIO, &val);

// UNIX //////////////////////////////////////////
#else
	setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof( optval ));
	fcntl( sockfd, F_SETFL, O_NONBLOCK );

//////////////////////////////////////////////////
#endif

	return ( sockfd );
}

static bool udp_open( void ) //
{
	char s[ 32 ];
    //char snmp_addr[ 32 ];

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
    sprintf( s, IP_MASK_FORMAT_WIN, ini_udp_net1, ini_udp_host );
    //sprintf( snmp_addr, IP_MASK_FORMAT_SNMP, ini_udp_net1, ini_snmp_host );

// UNIX //////////////////////////////////////////
#else
	sprintf( s, IP_MASK_FORMAT, ini_udp_net1 );
    //sprintf( snmp_addr, IP_MASK_FORMAT_SNMP, ini_udp_net1, ini_snmp_host );

//////////////////////////////////////////////////
#endif

	if ( ini_net_count == 1 ) {
		sockfd_in = udp_open_sock( s, ini_udp_port_in );
	} else {
		sockfd_in = udp_open_sock( NULL, ini_udp_port_in );
	}

printf("Tx SOCKET :%s  PORT: %d\n", s, ini_udp_port_out);
    sockfd_out1 = udp_open_sock( s, ini_udp_port_out );
	if ( ini_net_count > 1 ) {

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
		sprintf( s, IP_MASK_FORMAT_WIN, ini_udp_net2, ini_udp_host );

// UNIX //////////////////////////////////////////
#else
        sprintf( s, IP_MASK_FORMAT, ini_udp_net2 );

//////////////////////////////////////////////////
#endif
        sockfd_out2 = udp_open_sock( s, ini_udp_port_out );
	}
	bzero( &__t_addr_0, sizeof( struct sockaddr_in ));
	__t_addr_0.sin_family = AF_INET;
	__t_addr_0.sin_port = htons( ini_udp_port_in );
	sprintf( s, IP_MASK_FORMAT, ini_udp_net1 );
	__t_addr_0.sin_addr.s_addr = inet_addr( s );
	bzero( &__t_addr_1, sizeof( struct sockaddr_in ));
	__t_addr_1.sin_family = AF_INET;
	__t_addr_1.sin_port = htons( ini_udp_port_in );
	sprintf( s, IP_MASK_FORMAT, ini_udp_net2 );
	__t_addr_1.sin_addr.s_addr = inet_addr( s );
//	bzero( &__t_addr_2, sizeof( struct sockaddr_in ));
//	__t_addr_2.sin_family = AF_INET;
//	__t_addr_2.sin_port = htons( ini_udp_port_in );
//	sprintf( s, IP_MASK_FORMAT, ini_udp_net1_mi );
//	__t_addr_2.sin_addr.s_addr = inet_addr( s );
//	bzero( &__t_addr_3, sizeof( struct sockaddr_in ));
//	__t_addr_3.sin_family = AF_INET;
//	__t_addr_3.sin_port = htons( ini_udp_port_in );
//	sprintf( s, IP_MASK_FORMAT, ini_udp_net2_mi );
//	__t_addr_3.sin_addr.s_addr = inet_addr( s );
	udp_init_state();
    // UPS address
//    bzero( &ups_addr, sizeof( struct sockaddr_in ));
//    ups_addr.sin_family = AF_INET;
//    ups_addr.sin_port = htons( 161 );
    sprintf( s, IP_MASK_FORMAT, ini_udp_net1_mi );
//    sprintf( s, "192.168.100.101");
//    ups_addr.sin_addr.s_addr = inet_addr( s ); // почему этот адресс

	return ( true );
}

void udp_close( void )
{
	if ( sockfd_in != EOF ) close( sockfd_in );
	if ( sockfd_out1 != EOF ) close( sockfd_out1 );
    if ( sockfd_snmp_in != EOF ) close( sockfd_snmp_in );
	if ( sockfd_out2 != EOF ) close( sockfd_out2 );

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
	WSACleanup();

//////////////////////////////////////////////////
#endif
}


//static void snmp_receive( void )  // получение snmp
//{
//    int msg_l;
//    socklen_t addr_l = sizeof( struct sockaddr_in );
//    UPS_RESPONSE* ups_r = 0;

//    while (( msg_l = recvfrom( sockfd_snmp_in, snmp_buf, MAX_NET_DATA_SIZE, 0,
//                               ( struct sockaddr *) &__r_addr, &addr_l )) > 0 )
//    {
//        printf("\nBefore snmp parse\n");
//        ups_r = parse_snmp(snmp_buf);
//        if ( ups_r != 0 )
//        {
//            std::string str = "1.3.6.1.2.1.25.2.2.0 " + std::to_string(ups_r->val1);
//            printf(str.c_str());
//            std::free(ups_r);
//            ups_r = 0;
//            continue;
//        }
//        printf("\nSnmp isn't parsed\n");
//    }

//}

static void udp_receive( void ) // получение udp
{
	MESSAGE_REMOTE mg;
	int msg_l, net;
	uint8_t gr;
	int chrono;
	socklen_t addr_l = sizeof( struct sockaddr_in );
	uint8_t *d = ( uint8_t *) &__r_msg.byDataMess;
    PMESSNETHEAD h = &__r_msg.Head;
    QString str0="";


	while (( msg_l = recvfrom( sockfd_in, ( char *) &__r_msg, sizeof( __r_msg ), 0
	, ( struct sockaddr *) &__r_addr, &addr_l )) > 0 ) {

        /* Номер сети */
        net = _AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ];
		/* Проверка размера сообщения */
		if ( msg_l < ( int ) HEADER_SIZE ) {
			ERROR_MESSAGE( udp_r_len, "MESSNETHEAD", 0, msg_l, HEADER_SIZE );
			continue;
		}
		/* Игнорируем собственные сообщения */
        if ( h->IdSrc == HEADER_ID__TST ) continue;
		/* Игнорируем сообщения другим станциям */
		if (( h->IdDst != st_tst )
		 && ( h->IdDst != HEADER_ID__TST )
		 && ( h->IdDst != SEND_MRMOY )  /* Для отображения в МРМО в режиме реализации */
		 && ( h->IdDst != SEND_ALL )) continue;
		/* Проверка контрольной суммы */
		WORD crc = h->wCheckSum;
		h->wCheckSum = 0;
		if ( crc != CRC16(( PBYTE ) h, msg_l )) {
			ERROR_MESSAGE( udp_r_crc, net, msg_l );
			continue;
		}
        msg_l -= HEADER_SIZE;

        if ( net == ini_udp_net1 ) net = 0;
        else
        if ( net == ini_udp_net2 ) net = 1;
        else
        continue;

        /* Станция отправитель *verify_p_mi*/
        mg.abonent = h->IdSrc;
		/* Номер сети */
		mg.net = net;

        /* Проверка номера абонента */
        if ( mg.abonent >= st_count ) {
            ERROR_MESSAGE( udp_r_abonent, mg.abonent );
            continue;
        }
        for ( int j = 0; j<mcp_count; j++){
            if ( (_AS_PCHAR( __r_addr.sin_addr.s_addr )[ 3 ])  == mcp_list[j].addr){
                net = 2;
                mg.abonent = mcp_list[j].addr;
            }
        }
        if ( net == 2 ) {
            /* МИ */
            if (( mg.abonent < MCP_ID_MIN ) || ( mg.abonent > MCP_ID_MAX )) continue;
            if ( mg.abonent - MCP_ID_MIN > mcp_count ) {
                ERROR_MESSAGE( udp_r_abonent, mg.abonent );
                continue;
            }
		}

		/* В сети */

        __online[( _AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ] == ini_udp_net1 ) ? 0 : 1 ] |= BIT( h->IdSrc ); //?
        /* Команда */
		mg.cmd = h->byCommand;
		/* Проверка номера сообщения, кроме тестовых */
        if ( mg.cmd != CMD_TEST_NET ) {
                    chrono = udp_chrono( h->IdSrc, h->dwMessNum );
                    if (( net == 2 ) && ( chrono == CHRONO_REPLAY )) {
                        __online[ 1 ] |= BIT( h->IdSrc );
                        __online[ 0 ] |= BIT( h->IdSrc );
                        continue;
                    }

                    if ( chrono < 0 ) continue;
                } else {
                    chrono = CHRONO_OK;
                }
        //-------------------------------------------------------------

        if ( net == 2 ) {

            /****  МИ - MCP ****/
            uint8_t mcp = h->IdSrc - SEND_MCP1;
            uint8_t mi  = mcp * MI_MAX_COUNT;

            datmcp_set_byte( varmcp_stateM, h->bNodeState, mcp );
            for(int i = 0; i< MI_MAX_COUNT; i++){
                datm_set_byte( varm_state, h->bNodeState, mi + i);
           }

            datmcp_head2vector(( h->SE_VS[ 1 ] << 8 ) + h->SE_VS[ 0 ], mcp );
            datmcp_set_updated( mcp );     // возможно это надо будет вставить для все МИ
            /* Обработка сообщений */
			switch ( mg.cmd ) {

			/* Состояние МИ */
			case CMD_STATE:
                if ( msg_l == ( int ) sizeof( DATM )) {
                    datm_msg2vector(( DATM* ) d, mcp );
                    gui_send_event( EVENT_DATM_RECEIVED + mcp ); //
                } else {
                    ERROR_MESSAGE( udp_r_len, "CMD_STATE", mcp, msg_l, sizeof( DATM ));
                }
				break;

			/* Результаты измерений */
			case CMD_VSSU:
				if ( msg_l == ( int ) sizeof( struct SIGNALS )) {
                    datm_msg2vector_work(( struct SIGNALS* ) d, mcp );
                    gui_send_event( EVENT_DATM_RECEIVED + mcp );
				} else {
                    ERROR_MESSAGE( udp_r_len, "CMD_VSSU", mcp, msg_l, sizeof( struct SIGNALS ));
				}
				break;

			/* Сетевая статистика */
            case CMD_STAT:
                if ( msg_l == ( sizeof( struct TNETSTATISTIC ))) {
					/* Статистика */
                    net_get_stat2(( struct TNETSTATISTIC* ) d, mcp, (_AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ])==ini_udp_net1 ? 0 : 1);
                    gui_send_event( EVENT_STAT2_RECEIVED + mg.abonent );
				} else {
					ERROR_MESSAGE( udp_r_len, "CMD_STAT", mg.abonent, msg_l, sizeof( struct TNETSTATISTIC ));
				}
				break;

			/* Тестовое сообщение */
            case CMD_TEST_NET:
//                char date[25];
//                struct timeval t;
//                time_t sec;

//                gettimeofday( &t, NULL );
//                sec = t.tv_sec;
//                strftime( date, sizeof( date ), "\n RX %H:%M:%S. \n", localtime( &sec ));
//                str0.append((char*)date);


//                memcpy( snmp_buf, &__r_msg, msg_l);
//                for ( int b = 0; b < msg_l; b++){
//                    sprintf(date, "%X.", snmp_buf[b]);
//                    str0.append(date);
//                    if( (b+1)%16==0){
//                        str0.append("\n");
//                    }
//                }
//                //str0.append(((char *)&snmp_buf));
//                journal_printmcp_qstring( "",str0);
//                journal_printmcp_enter(  );


                if ( msg_l <= ( sizeof( TEST_NET_PACKET ))) {
					/* Данные */
                    mg.net = ((_AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ])==ini_udp_net1 ? 0 : 1);
                    mg.abonent = h->IdSrc; /* Номер абонента с учетом ВУ! */
                    memcpy( mg.malloc_data( msg_l ), d, msg_l );
                    pthread_mutex_lock( &test_net_mutex );
                    test_net_queue.push( mg );
                    pthread_mutex_unlock( &test_net_mutex );
                } else {
                    ERROR_MESSAGE( udp_r_len, "CMD_TEST_NET", mg.abonent, msg_l, sizeof( TEST_NET_PACKET ));
				}
				break;

            case CMD_MI_FILE_REC:
//                                char date[25];
//                                struct timeval t;
//                                time_t sec;

//                                gettimeofday( &t, NULL );
//                                sec = t.tv_sec;
//                                strftime( date, sizeof( date ), "\n RX %H:%M:%S. \n", localtime( &sec ));
//                                str0.append((char*)date);


//                                memcpy( snmp_buf, &__r_msg, msg_l);
//                                for ( int b = 0; b < msg_l; b++){
//                                    sprintf(date, "%X.", snmp_buf[b]);
//                                    str0.append(date);
//                                    if( (b+1)%16==0){
//                                        str0.append("\n");
//                                    }
//                                }
//                                //str0.append(((char *)&snmp_buf));
//                                journal_printmcp_qstring( "",str0);
//                                journal_printmcp_enter(  );
                        if(msg_l == sizeof(FILEWRDAT)){
                            memcpy( mg.malloc_data( msg_l ), d, msg_l );

                            pthread_mutex_lock( &mi_mutex );
                            mi_queue.push( mg );
                            pthread_mutex_unlock( &mi_mutex );
                        }
                        else{
                            ERROR_MESSAGE( udp_r_len, "CMD_RESULT_READ_MESS", mg.abonent, msg_l, sizeof( WORD ));
                        }
                    break;


			/* Результат операции */
            case CMD_RESULT:

                if ( msg_l == sizeof( WORD )) {
                    switch ((( PENDPROGRESS ) d )->byCommand ) {
                    /* Результат передачи таблиц */
                    case CMD_MI_SEND_TABLE:
                    case CMD_MI_PROG_TABLE:
                        mg.abonent = h->IdSrc;
                        mg.set_data_short((( PENDPROGRESS ) d )->byEndCode, sizeof( byte ));
                        pthread_mutex_lock( &mi_mutex );
                        mi_queue.push( mg );
                        pthread_mutex_unlock( &mi_mutex );
                        break;
                    }


                } else {
                    ERROR_MESSAGE( udp_r_len, "CMD_RESULT", mg.abonent, msg_l, sizeof( WORD ));
                }
                break;
            }
			continue;
		}


        //--------------------------------------------

		/****  ВУ  ****/
		/* Режим станции */
		datn_set_byte( varn_state, h->bNodeState, mg.abonent );
		/* Верификатор ЗУ */
		datn_set_dword( varn_verify, h->Veryfi, mg.abonent );
		datn_set_updated( mg.abonent );

		/* Проверка на реализацию ЗУ */
		if ( h->bNodeState == WORK ) {
			uint16_t f = ( h->SE_VS[ 1 ] << 8 ) + h->SE_VS[ 0 ];
			datn_set_word( varn_failure, ( f & ( AB_MASK_ERROR | AB_MASK_ERROR_ID )), mg.abonent );
            datn_set_byte( varn_link_0, AB_ERROR_2_LINK( f, 1 ), mg.abonent );
            datn_set_byte( varn_link_1, AB_ERROR_2_LINK( f, 2 ), mg.abonent );
			__operate |= BIT( mg.abonent );
			/* Обработка сообщений */
			switch ( mg.cmd ) {

			/* Состояние блоков */
			case CMD_VSSU:
                /* Состояния блоков только в первом пакете */
                if ((( struct CIOREQUEST *) d )->dwOffsetData != 0 ) break;

                switch ( st_list[ mg.abonent ].TypeMRMO ) {
				/* Только для ЦОИ и КВВ */
				case IOCONT:
				case MRMOY:
				case MRMOY_KPA:

					gr = st_list_ex[ mg.abonent ].gr;
					if ( gr < GR_LIMIT ) {

						if ( msg_l >= ( int )( gr_list[ gr ].bl_count
							* sizeof( TSTATEUPO_VSSU ) * BL_CH_COUNT + CIOR_I_OFFSET )) {
							datb_msg2vector(( uint32_t *)( d + CIOR_I_OFFSET ), 1, mg.abonent );
							gui_send_event( EVENT_DATB_RECEIVED );
						} else {
							ERROR_MESSAGE( udp_r_len, "CMD_VSSU", mg.abonent, msg_l
							, gr_list[ gr ].bl_count * sizeof( TSTATEUPO_VSSU ) * BL_CH_COUNT );
						}
					}
					break;
				}
				break;

			/* Блок ТД */
			case COM_SENDBLOCKDATA:
				udp_sendto_p( &__r_msg, msg_l + HEADER_SIZE );
				break;
			}
			gui_send_event( EVENT_DATN_RECEIVED + mg.abonent );
			/* Игнорируем сообщения от станций в режиме работы */
			continue;
		}
		/* Обработка сообщений */
		switch ( mg.cmd ) {

		/* Состояние станции */
		case CMD_STATE:
			if ( msg_l >= ( int ) sizeof( DATN_ST )) {
				datn_msg2vector( &__datn, d );
				datn_set_vector( &__datn, mg.abonent );
				gui_send_event( EVENT_DATN_RECEIVED + mg.abonent );
				msg_l -= sizeof( DATN_ST );
				gr = st_list_ex[ mg.abonent ].gr;
				if (( gr < GR_LIMIT ) && ( h->Veryfi )) {
					/*imit
					int l = ( int )( gr_list[ gr ].bl_count * sizeof( TSTATEUPO ) * BL_CH_COUNT );
					if ( msg_l > l ) msg_l = l;
					*/

//printf("group=%d block count=%d\n", gr, gr_list[ gr ].bl_count);
					if ( msg_l == ( int )( gr_list[ gr ].bl_count * sizeof( TSTATEUPO ) * BL_CH_COUNT )) {
						uint32_t *p = ( uint32_t *)( d + sizeof( DATN_ST ));
						datb_msg2vector( p, sizeof( TSTATEUPO ) / sizeof( TSTATEUPO_VSSU ), mg.abonent );
						datb_msg2chsum( p, mg.abonent );
						gui_send_event( EVENT_DATB_RECEIVED );
					} else {
						if ( rzu_ind < rzu_count ) {
							__echo_warn( ERROR_format[ ERROR_udp_r_len ], "CMD_STATE", mg.abonent, msg_l
							, gr_list[ gr ].bl_count * sizeof( TSTATEUPO ) * BL_CH_COUNT );
						}
					}
				}
			} else {
				ERROR_MESSAGE( udp_r_len, "CMD_STATE", mg.abonent, msg_l, sizeof( DATN_ST ));
			}
			break;

		/* Сетевая статистика */
		case CMD_STAT:
			if ( msg_l == ( sizeof( NET_STAT_PACKET ))) {
				/* Статистика */
				net_get_stat(( NET_STAT_PACKET *) d, mg.abonent );
				gui_send_event( EVENT_STAT_RECEIVED + mg.abonent );
			} else {
				ERROR_MESSAGE( udp_r_len, "CMD_STAT", mg.abonent, msg_l, sizeof( NET_STAT_PACKET ));
			}
			break;

		/* Тестовое сообщение */
		case CMD_TEST_NET:
			if ( msg_l == ( sizeof( TEST_NET_PACKET ))) {
				/* Данные */
                memcpy( mg.malloc_data( msg_l ), d, msg_l );
                pthread_mutex_lock( &test_net_mutex );
                test_net_queue.push( mg );
                pthread_mutex_unlock( &test_net_mutex );
			} else {
                ERROR_MESSAGE( udp_r_len, "CMD_TEST_NET", mg.abonent, msg_l, sizeof( TEST_NET_PACKET ));
			}
			break;

		/* Результат операции */
		case CMD_RESULT:
			if ( msg_l == sizeof( struct ENDPROGRESS )) {

				switch ((( PENDPROGRESS ) d )->byCommand ) {
				/* Результат записи или проверки ТД */
				case CMD_TD_WRITE_BLOCK:
				case CMD_TD_CHECK_BLOCK:
					mg.set_data_short((( PENDPROGRESS ) d )->byEndCode, sizeof( byte ));
                    pthread_mutex_lock( &td_mutex );
                    td_queue.push( mg );
                    pthread_mutex_unlock( &td_mutex );
					break;
				/* Результат передачи ПО блока УПО */
				case CMD_BL_PROG:
					mg.set_data_short((( PENDPROGRESS ) d )->byEndCode, sizeof( byte ));
                    pthread_mutex_lock( &bl_mutex );
                    bl_queue.push( mg );
                    pthread_mutex_unlock( &bl_mutex );
					break;
				/* Расширенный результат теста */
				case CMD_EX:
					report_test_result(( PENDPROGRESS ) d, mg.abonent );
					break;
				}
			} else {
				ERROR_MESSAGE( udp_r_len, "CMD_RESULT", mg.abonent, msg_l, sizeof( struct ENDPROGRESS ));
			}
			break;

		/* Блок ТД */
		case COM_SENDBLOCKDATA:
			udp_sendto_p( &__r_msg, msg_l + HEADER_SIZE );
			break;
		}
	}
	/* Проверка таймаута связи со станциями */
	if ( clock_time_after( __online_to )) {
		uint32_t ol = __get_online();

		data_set_dword( var_online_mask, ol );
		data_set_dword( var_operate_mask, __operate );  /* ВУ, МИ */

		for ( int i = 0; i < st_count; i++ ) {
			dword mask;
			dword bit = BIT( i );

			switch ( st_list[ i ].TypeMRMO ) {
			case MRMOY:
            case MRMOY_KPA:
				voted_set_byte( var_operate, st_list[ i ].byChannel + 1, (( __operate & bit ) != 0 )
				, vote_byte );
                /* fallthrough */
			case MRMOT:
			case IOCONT:
			case IOCONT_K:
			case SSBVS:
				if (( ol & bit ) == 0 ) {
					datn_set_byte( varn_state, TURN_OFF, i );
					data_set_dword( var_onelink_mask, CLEAR_VALUE( data_get_dword( var_onelink_mask ), bit ));
                    gui_send_event( EVENT_DATN_RECEIVED + i );
                }
				if ( ini_net_count == 1 ) break;

				mask = data_get_dword( var_onelink_mask );
				if ( __only_1_online() & bit ) {
					/* Прием только по одной сети - мигаем */
                    data_set_dword( var_onelink_mask, ( mask & bit ) ? CLEAR_VALUE( mask, bit )
                                                                     : SET_VALUE( mask, bit ));
//printf("CELL MASK %X \n", data_get_dword( var_onelink_mask ) & bit);
                    datn_set_changed( varn_failure, i );
					gui_send_event( EVENT_DATN_RECEIVED + i );

				} else if ( mask & bit ) {
					/* Сброс ошибки приема */
					data_set_dword( var_onelink_mask, CLEAR_VALUE( mask, bit ));
					datn_set_changed( varn_failure, i );
					gui_send_event( EVENT_DATN_RECEIVED + i );
				}
				break;

            case MCP:
                //bit = bit>>1;

                if ( ( ol & bit ) == 0 ) {
                    uint8_t mcp = i - SEND_MCP1;
                    uint8_t mi  = mcp * MI_MAX_COUNT;
                    datmcp_set_byte( varmcp_stateM, TURN_OFF, mcp);
                    for(int j = 0; j < MI_MAX_COUNT; j++){
                        datm_set_byte(varm_state, TURN_OFF, mi + j);
                    }
                    gui_send_event( EVENT_DATM_RECEIVED + mcp );
                } else
                    if(__only_1_online() & bit){
                    /* Проверка приема только по одной сети - мигаем если... */
                    datmcp_set_byte( varmcp_onelink
                    , ( __only_1_online() & bit ) && ( datmcp_get_byte( varmcp_onelink, i - 4 ) == 0 )
                    , i - 4
                    );

                    if (datmcp_get_byte(varmcp_link1,i-4) || _AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ]==ini_udp_net2){

                        if(datmcp_get_byte(varmcp_red,  i-4)==0){
                        datmcp_set_byte(varmcp_red, 1,  i-4);
                        datmcp_set_byte(varmcp_net1, 1, i-4);
                        datmcp_set_byte(varmcp_net0, 0, i-4);
                        }
                        else if(datmcp_get_byte(varmcp_red, i-4)>0){
                            datmcp_set_byte(varmcp_red, 0,  i-4);
                            datmcp_set_byte(varmcp_net1, 1, i-4);
                            datmcp_set_byte(varmcp_net0, 0, i-4);
                        }
                        gui_send_event( EVENT_DATM_RECEIVED + i-4 );

                    }
                    else if(datmcp_get_byte(varmcp_link0,i-4)  || _AS_PCHAR( __r_addr.sin_addr.s_addr )[ 2 ]==ini_udp_net1 ){
                        if(datmcp_get_byte(varmcp_red,  i-4)==0){
                        datmcp_set_byte(varmcp_red, 1,  i-4);
                        datmcp_set_byte(varmcp_net0, 1, i-4);
                        datmcp_set_byte(varmcp_net1, 0, i-4);
                        }
                        else if(datmcp_get_byte(varmcp_red, i-4)>0){
                            datmcp_set_byte(varmcp_red, 0,  i-4);
                            datmcp_set_byte(varmcp_net0, 1, i-4);
                            datmcp_set_byte(varmcp_net1, 0, i-4);
                        }
                        gui_send_event( EVENT_DATM_RECEIVED + i-4 );

                    }


                }
                else{
                    datmcp_set_byte(varmcp_net0, 0, i-4);
                    datmcp_set_byte(varmcp_net1, 0, i-4);
                    gui_send_event( EVENT_DATM_RECEIVED + i-4 );

                }
                break;
            }
		}
		__reset_online();
		__operate = 0;

		__online_to = clock_t_set( ini_udp_online );
	}
}

static void udp_transmit( void )
{
    char s[32];
    int l1, l;
    int l2 = 0;
    PMESSNETHEAD h = &__t_msg.Head;
	uint8_t *d = ( uint8_t *) &__t_msg.byDataMess;
	MESSAGE_REMOTE* mg = __queue_front();
    QString str0="";

	/* Внимание! Общая очередь на передачу ВУ и МИ */
	while ( mg ) {

            /* Заголовок сообщения */

            h->IdDst = mg->abonent;
            h->byCommand = mg->cmd;
            if (( mg->net == 2 ) || ( data_get_byte( var_td_operate ) != TD_WRITE )) {
                h->Veryfi = datn_get_dword( varn_verify, st_tst );
            } else {
                /* Обнуляем верификатор на время записи ТД */
                h->Veryfi = 0;
            }
            l = HEADER_SIZE + mg->get_len();

            if ( mg->cmd != CMD_TEST_NET ) {
                h->dwMessNum = __t_messnum;
            }

            if ( mg->cmd == CMD_EX ) {  /* Расширенный заголовок */
                (( EX_HEADER *) d )->mask = mg->mask;
                (( EX_HEADER *) d )->mask1 = 0;
                (( EX_HEADER *) d )->cmd = mg->ex_cmd;
                d += EX_HEADER_SIZE;
                l += EX_HEADER_SIZE;
            }
            /* Данные */
            memcpy( d, mg->get_data(), mg->get_len());
            /* Контрольная сумма */
            h->wCheckSum = 0;
            h->wCheckSum = CRC16(( PBYTE ) h, l );

            if ( mg->net == 2 ) {

                /* Передача сообщений в сеть МИ */
                l1 = sendto( sockfd_out1, ( const char *) &__t_msg, l, 0, ( struct sockaddr *) &__t_addr_0
                , sizeof( __t_addr_0 ));
                if ( l1 != l ) {
                    ERROR_MESSAGE( udp_t_sendto, errno );
                    perror( __CRLF__"    sendto(1 -> 1MI)" );
                }
                if(ini_net_count>1){
                l2 = sendto( sockfd_out2, ( const char *) &__t_msg, l, 0, ( struct sockaddr *) &__t_addr_1
                , sizeof( __t_addr_1 ));
                if ( l2 != l ) {
                    ERROR_MESSAGE( udp_t_sendto, errno );
                    perror( __CRLF__"    sendto(2 -> 2MI)" );
                    }
                }

                if (( l1 != l ) && ( l2 != l )) {
                    if ( ++__t_error[ 1 ] == __T_ERR_LIMIT ) {
                        mg->set_transmitted( MESSAGE_DISCARDED );
                        __queue_pop();
                        __t_error[ 1 ] = 0;
                    }
                    if ( ++__t_error[ 2 ] == __T_ERR_LIMIT ) {
                        mg->set_transmitted( MESSAGE_DISCARDED );
                        __queue_pop();
                        __t_error[ 2 ] = 0;
                    }
                    if ( ++__t_error[ 3 ] == __T_ERR_LIMIT ) {
                        mg->set_transmitted( MESSAGE_DISCARDED );
                        __queue_pop();
                        __t_error[ 3 ] = 0;
                    }

                    break;
                }
                ++__t_messnum;
            } else {
                /* Передача сообщений в обе сети ВУ */
                l1 = sendto( sockfd_out1, ( const char *) &__t_msg, l, 0, ( struct sockaddr *) &__t_addr_0
                , sizeof( __t_addr_0 ));
                if ( l1 != l ) {
                    ERROR_MESSAGE( udp_t_sendto, errno );
                    perror( __CRLF__"    sendto(1 -> 1)" );
                }
                if ( ini_net_count > 1 ) {
                    l2 = sendto( sockfd_out2, ( const char *) &__t_msg, l, 0, ( struct sockaddr *) &__t_addr_1
                    , sizeof( __t_addr_1 ));
                    if ( l2 != l ) {
                        ERROR_MESSAGE( udp_t_sendto, errno );
                        perror( __CRLF__"    sendto(2 -> 2)" );
                    } // else
                }
                if (( l1 != l ) && ( l2 != l )) {
                    if ( ++__t_error[ 0 ] == __T_ERR_LIMIT ) {
                        mg->set_transmitted( MESSAGE_DISCARDED );
                        __queue_pop();

                        __t_error[ 0 ] = 0;
                    }
                    break;
                }
                ++__t_messnum;
    //        }
        }
        mg->set_transmitted( MESSAGE_TRANSMITTED );  /* Сообщение отправлено */
        __queue_pop();

        break;  /* one message at once */
/*		mg = __queue_front();
 */
	}
}

static void udp_poll( void )
{
	/* Проверка таймаута опроса */
	if ( clock_time_after( __poll_to )) {
		if ( datn_get_dword( varn_verify, st_tst ) == 0 ) {
			/* РЗУ не выбран */
			dword host = data_get_dword( var_operate_mask ) & st_host_mask;
			if ( host ) {
				/* РЗУ запущен - заимствовать верификатор МРМОУ с младшим индексом */
				datn_set_dword( varn_verify, datn_get_dword( varn_verify, __builtin_ffs( host ) - 1 ), st_tst );
				gui_send_event( EVENT_RZU_CHANGED );
			} else {
				if ( data_get_byte( var_select_rzu ) == 0 ) {
					/* Выбрать РЗУ */
					gui_send_event( EVENT_SELECT_RZU );
				}
			}
		} else {

//printf("Make Tx message:%d\n", CMD_STATE);
            message_transmit( SEND_ALL, CMD_STATE );  /* Опрос новых станций в сети */
		}
		__poll_to = clock_t_set( data_get_byte( var_operate ) ? ini_udp_poll_operate : ini_udp_poll );
    }
	/* Проверка таймаута опроса МИ */
    if ( true && clock_time_after( __poll_to2 )) {
        if ( datn_get_dword( varn_verify, st_tst )) {
            //message_transmit2( SEND_BI, CMD_STATE );  /* Опрос МИ */
            //message_transmit2( SEND_MCP1, CMD_STATE ); // не знаю сработает ли на строчку выше для МЦП
            //message_transmit2( SEND_MCP2, CMD_STATE ); //
            //message_transmit2( SEND_MCP3, CMD_STATE );
		}
		__poll_to2 = clock_t_set( ini_udp_poll_mi );
	}
}

static void udp_stat_request( void )
{
	/* Проверка таймаута запроса статистики */
	if ( clock_time_after( __stat_request_to )) {

		/* Проверка передачи данных */
		if (( data_get_byte( var_td_operate ) == TD_NONE ) && ( data_get_byte( var_test_net_command ) == 0 )) {
			/* Запрос статистики */
			message_transmit( SEND_ALL, CMD_STAT_REQUEST );
			if ( ini_net_mi ) {
                message_transmit2( SEND_MCP1, CMD_STAT_REQUEST );
                message_transmit2( SEND_MCP2, CMD_STAT_REQUEST );
                message_transmit2( SEND_MCP3, CMD_STAT_REQUEST );			}
		}
		__stat_request_to = clock_t_set( ini_udp_stat_request_sec * 1000 );
	}
}

static int udp_chrono( uint8_t st, dword num )
{
	dword next = __r_next[ st ];

	if ( num == next ) {
		/* Ожиданмый номер принятого сообщения */
		++__r_next[ st ];
		__r_error[ st ] = 0;

		return ( CHRONO_OK );
	}

	if ( num == next - 1 ) {
		/* Номер принятого сообщения равен номеру предыдущего */
		return ( CHRONO_REPLAY );
	}

	if ( num == 0 ) {
		/* Счетчик сброшен */
		__r_next[ st ] = 1;
		__r_error[ st ] = 0;

		return ( CHRONO_FIRST );
	}

	if ( num < __r_next[ st ]) {
		/* Старое сообщение или сбой счетчика */
		if ( ++__r_error[ st ] == __R_ERR_LIMIT ) {
			/* Сбой счетчика */
			__r_next[ st ] = num + 1;
			__r_error[ st ] = 0;
            __echo_warn( "udp_receive(%d): num (%d - %d)", st, num, __r_next[ st ]);

			return ( CHRONO_TIMEOUT );
		}
        __echo_warn( "udp_receive(%d): num (%d - %d) err %d", st, num, __r_next[ st ], __r_error[ st ]);
		/* Считаем, что сообщение старое */
		return ( CHRONO_WRONG );
	}
	/* Были пропущены сообщения */
	__r_next[ st ] = num + 1;
	__r_error[ st ] = 0;

	return ( CHRONO_MISSED );
}

static void udp_open_p( void )
{
	sockfd_p = udp_open_sock( ini_udp_ip_prot, ini_udp_port_prot );

	bzero( &__t_addr_p, sizeof( struct sockaddr_in ));
	__t_addr_p.sin_family = AF_INET;
	__t_addr_p.sin_port = htons( ini_udp_port_prot + 1 );
    __t_addr_p.sin_addr.s_addr = inet_addr( ini_udp_ip_prot ); // положили ip-addres
}

static void udp_relay_p( void )
{
	int msg_l;
	socklen_t addr_l = sizeof( struct sockaddr_in );

	/* Прием сообщения от программы обработки протокола */
	if (( msg_l = recvfrom( sockfd_p, ( char *) &__r_msg, sizeof( __r_msg ), 0
	, ( struct sockaddr *) &__r_addr, &addr_l )) > 0 ) {
		MESSAGE_REMOTE mg;
		uint8_t *d = ( uint8_t *) &__r_msg.byDataMess;
		PMESSNETHEAD h = &__r_msg.Head;
		/* Проверка размера сообщения */
		if ( msg_l < ( int ) HEADER_SIZE ) {
			ERROR_MESSAGE( udp_r_len_p, "MESSNETHEAD", msg_l, HEADER_SIZE );

			return;
		}
		msg_l -= HEADER_SIZE;
		/* Ретрансляция сообщения в сеть */
		mg.net = 0;
		mg.abonent = h->IdDst;
		mg.cmd = h->byCommand;
        memcpy( mg.malloc_data( msg_l ), d, msg_l );
		mg.init_transmitted( NULL );

        pthread_mutex_lock( &udp_t_mutex );
        udp_t_queue.push( mg );
        pthread_mutex_unlock( &udp_t_mutex );
	}
}

static void udp_sendto_p( void *msg, int len )
{
    if ( sockfd_p != EOF ) {
		/* Передать сообщение программе обработки протокола */
		if ( sendto( sockfd_p, ( const char *) msg, len, 0, ( struct sockaddr *) &__t_addr_p
			, sizeof( __t_addr_p )) != len ) {
			ERROR_MESSAGE( udp_r_sendto_p, errno );
			perror( __CRLF__"    sendto()" );
		}
	}
}

static void udp_close_p( void )
{
	if ( sockfd_p != EOF ) {
		close( sockfd_p );
		sockfd_p = EOF;
	}
}

static MESSAGE_REMOTE *__queue_front( void )
{
	MESSAGE_REMOTE *result = NULL;

	pthread_mutex_lock( &udp_t_mutex );

	if ( !udp_t_queue.empty()) {
		result = &udp_t_queue.front();
	}
	pthread_mutex_unlock( &udp_t_mutex );

	return ( result );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &udp_t_mutex );

	udp_t_queue.front().free_data();
	udp_t_queue.pop();

	pthread_mutex_unlock( &udp_t_mutex );
}

/** \} */
