#include "include.h"
// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#define socklen_t int32_t
// UNIX //////////////////////////////////////////
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>

//////////////////////////////////////////////////
#endif


// PDU types in SNMPv1, SNMPsec, SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
//#define SNMP_MSG_GET	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x0)
//#define SNMP_MSG_GETNEXT    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x1)
//#define SNMP_MSG_RESPONSE   (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x2)
//#define SNMP_MSG_SET	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x3)

///* PDU types in SNMPv1 and SNMPsec */
//#define SNMP_MSG_TRAP	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x4) /* c4 = 196 */

///* PDU types in SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
//#define SNMP_MSG_GETBULK    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x5) /* c5 = 197 */
//#define SNMP_MSG_INFORM	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x6) /* c6 = 198 */
//#define SNMP_MSG_TRAP2	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x7) /* c7 = 199 */

///* PDU types in SNMPv2u, SNMPv2*, and SNMPv3 */
//#define SNMP_MSG_REPORT	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x8) /* c8 = 200 */

#define SNMP_MAX_LEN	    1500        /* typical maximum message size */
#define SNMP_MIN_MAX_LEN    484         /* minimum maximum message size */


//#define SNMP_PORT	    161         /* standard UDP port for SNMP agents
//                                           to receive requests messages */
//#define SNMP_TRAP_PORT	    162         /* standard UDP port for SNMP
//                                           managers to receive notificaion
//                                          (trap and inform) messages


//static struct sockaddr_in  ups_addr;  /**< ????????????. */


//------------------------------------------------------------------

#define bzero( ptr, sz ) memset( ptr, 0, sz );
static struct sockaddr_in ups_addr, server_addr, server_addr0;
static int socketServer=0, socketServer0=0;
using namespace std;
static word dly;
static char translateUtf8[256];
//static uint16_t pov = 0;


snmpmess::snmpmess()
{

}

int get_int_from_bytes(char bytes[], uint32_t idx, uint32_t count)  // ???????????????? ???????????????? oid
{
    int res = 0;
    uint32_t end_len_idx = idx + count;
    if ( count > 4 ){
        return 0;
    }
    if (count == 1){

        return res=(char)bytes[idx];
    }

    while ( idx < end_len_idx )
    {
        res = res << 8;
        res |= (unsigned char) bytes[idx];
        (idx)++;
    }
    (idx)--;

    return res;
}

void snmp_server_open_sock(){
    char addr[32];
    int ara;
// WIN32 /////////////////////////////////////////
    #ifdef __WIN32__
         WSADATA wsa;
        int kvota = WSAStartup( MAKEWORD( 2, 2 ), &wsa );
        if ( kvota != NO_ERROR ) {
            return;
        }
    #endif
//////////////////////////////////////////////////

        socketServer = socket(PF_INET, SOCK_DGRAM, 0);//?????????? ??????????????
        if (socketServer < 0){

// WIN32 /////////////////////////////////////////
    #ifdef __WIN32__
            WSACleanup();
//////////////////////////////////////////////////
    #endif
    socketServer=-1;
    return;
        }

        socketServer0 = socket(PF_INET, SOCK_DGRAM, 0);//?????????? ??????????????
        if (socketServer0 < 0){

// WIN32 /////////////////////////////////////////
    #ifdef __WIN32__
            WSACleanup();
//////////////////////////////////////////////////
    #endif
    socketServer0=-1;
    return;
        }

        bzero(&server_addr, sizeof( struct sockaddr_in ));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(0); // ?????????? ?????? ???????????????? ?????????????? ????????,(htons)

// WIN32 ////////////////////////////////
#ifdef __WIN32__
        sprintf(addr, IP_MASK_FORMAT_WIN, ini_udp_net1, ini_udp_host);
//UNIX//////////////////////////////////
#else
      sprintf( addr, IP_MASK_FORMAT, ini_udp_net1);
#endif
      //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      //server_addr.sin_addr.s_addr = ( addr ) ? inet_addr( addr ) : htonl( INADDR_ANY );
      server_addr.sin_addr.s_addr = inet_addr("192.168.101.4");

        bzero(&server_addr0, sizeof( struct sockaddr_in ));
        server_addr0.sin_family = AF_INET;
        server_addr0.sin_port = htons(0); // ?????????? ?????? ???????????????? ?????????????? ????????,(htons)

// WIN32 ////////////////////////////////
#ifdef __WIN32__
        sprintf(addr, IP_MASK_FORMAT_WIN, ini_udp_net1, ini_udp_host);
//UNIX//////////////////////////////////
#else
      sprintf( addr, IP_MASK_FORMAT, ini_udp_net1);
#endif
      //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      //server_addr.sin_addr.s_addr = ( addr ) ? inet_addr( addr ) : htonl( INADDR_ANY );
      server_addr0.sin_addr.s_addr = inet_addr("192.168.100.4");


        ara=bind(socketServer, (struct sockaddr *) & server_addr, sizeof (server_addr));
        if (ara < 0){
    // WIN32 /////////////////////////////////////////
    #ifdef __WIN32__
                    WSACleanup();
                    closesocket(socketServer);
    // UNIX //////////////////////////////////////////
    #else
                    close(socketServer);
    #endif
                    socketServer=-1;
                    return;
        }



        ara=bind(socketServer0, (struct sockaddr *) & server_addr0, sizeof (server_addr0));
        if (ara < 0){
    // WIN32 /////////////////////////////////////////
    #ifdef __WIN32__
                    WSACleanup();
                    closesocket(socketServer0);
    // UNIX //////////////////////////////////////////
    #else
                    close(socketServer0);
    #endif
                    socketServer0=-1;
                    return;
        }



        // WIN32 /////////////////////////////////////////
        #ifdef __WIN32__
        DWORD noBlockme = 1;
        if (ioctlsocket(socketServer, FIONBIO, &noBlockme) != 0){
            closesokcet(socketServer);
            socketServer=-1;
            return;
        }
        // UNIX //////////////////////////////////////////
                #else
        u_long noBlockme = 1;
        setsockopt( socketServer, SOL_SOCKET, SO_BROADCAST, &noBlockme, sizeof(noBlockme));
        fcntl ( socketServer, F_SETFL, O_NONBLOCK );
        #endif

                // WIN32 /////////////////////////////////////////
        #ifdef __WIN32__
        if (ioctlsocket(socketServer0, FIONBIO, &noBlockme) != 0){
            closesocket(socketServer0);
            socketServer0=-1;
            return;
        }
        // UNIX //////////////////////////////////////////
                #else
        setsockopt( socketServer0, SOL_SOCKET, SO_BROADCAST, &noBlockme, sizeof(noBlockme));
        fcntl ( socketServer0, F_SETFL, O_NONBLOCK );
        #endif
        return;
}

void snmp_open_sock(char ipaddr[32]){
    bzero(&ups_addr, sizeof(struct sockaddr_in));
    ups_addr.sin_family=AF_INET;
    ups_addr.sin_addr.s_addr=inet_addr(ipaddr);
    ups_addr.sin_port=htons(161);
    return;
}

void snmp_sendmess(char snmpSendMess[], int sendMessSize, int socketrecv){
    int ara;
    socklen_t sizeadresa = sizeof(struct sockaddr_in);
    ara=sendto(socketrecv, snmpSendMess, sendMessSize, 0, (struct sockaddr *)&ups_addr, sizeof (ups_addr));
    if (ara!=sendMessSize){
 // WIN32 /////////////////////////////////////////
#ifdef __WIN32__
         WSACleanup();
         closesocket(socketServer);
         closesocket(socketServer0);
        return;
#else
// UNIX //////////////////////////////////////////
        close(socketServer);
        close(socketServer0);
        socketServer=-1;
        socketServer0=-1;
        return;
    #endif
    }
    return;
}

int snmp_recvmess(int socketrecv){

    int indx=0;
    int ara=0;
    int oidznach;
    socklen_t sizeadresa = sizeof(struct sockaddr_in);
    int mess_stop = 0;
    while( mess_stop < 50 ){
        usleep(6000);
        ara = recvfrom(socketrecv, RecvBuf, 1024, 0, (struct sockaddr *) &ups_addr, &sizeadresa);

        mess_stop++;
        if (ara > 0){
            break;
        }
    }
    if (ara <= 0){
        return 0;
    }

    if(RecvBuf[indx]!=0x30){
        // WIN32 /////////////////////////////////////////
       #ifdef __WIN32__
        //printf("error: no snmp pack");
               return 0;
       #else
       // UNIX //////////////////////////////////////////
               //printf("error: no snmp pack")
               return 0;
         #endif
    }
    while(true){
        indx++;
        if (indx>=ara){
            break;
        }
        if (RecvBuf[indx]==0x01){
            indx++;
            if (RecvBuf[indx]!=0x00){
#ifdef __WIN32__
 //printf("error: invalid version");
        return 0;
#else
// UNIX //////////////////////////////////////////
        //printf("error: invalid version");

        return 0;
#endif
            }
            else {

                indx++;
            }
            break;
        }
    }
    while (true){ // ?? ?????????? ???? ????????????
        indx++;
        if (indx>=ara){
            break;
        }
        if (RecvBuf[indx]==0x06){
            indx++;
        }
        if (RecvBuf[indx]==char(0xa2)){ // ?????????? ???? ?????????? ??????????????
            break;
        }
        //std::cout<<std::hex<<RecvBuf[indx];
    }
while (true){ // request id,
    indx++;
    if (indx>=ara){
        break;
    }
    if (RecvBuf[indx]==0x02){
        indx++;
        if (RecvBuf[indx]==0x02){
            indx++;
            }
        break;
    }
}
while(true){  // error status
    indx++;
    if (indx>=ara){
        break;
    }
    if (RecvBuf[indx]==0x01){
        indx++;
        if (RecvBuf[indx]!=0x00){
        return 0;
        }
        else {
            break;
        }
    }
}
while(true){  // error index
    indx++;
    if (indx>=ara){
        break;
    }
    if (RecvBuf[indx]==0x01){
        indx++;
        if (RecvBuf[indx]!=0x00){
#ifdef __WIN32__
        closesocket(socketUps);
        return 0;
#else
// UNIX //////////////////////////////////////////
        close(socketUps);

        return 0;
#endif
        }
        else {
            break;
        }
    }
}

while(true){
    indx++;
    if(RecvBuf[indx]==0x00){
        indx++;
        indx++;
        indx++;
        break;
    }
}
oidznach=get_int_from_bytes(RecvBuf, indx,ara-indx);
return oidznach;

    }

/* проверяем ошибки упса */
void check_snmp_error(uint var){
    double errnoPack = 0;
    snmpErrorBuf=" ";

    if (upsParametrs2[var].upsBatSt != 2 && upsParametrs2[var].upsBatSt != 0){
        upsParametrs2[var].upsErrorStatus=1;
        //snmpErrorBuf+=upsParametrs2[var].upsBatSt;
        snmpErrorBuf+=UPS_STATUS_ERROR;

    }
    if (upsParametrs2[var].upsCap < 50 && upsParametrs2[var].upsCap!=0){
        upsParametrs2[var].upsErrorStatus=1;
        snmpErrorBuf+=UPS_BATTERY_ERROR;
    }
    if ( (upsParametrs2[var].upsloadP<200 || upsParametrs2[var].upsloadP>240) && upsParametrs2[var].upsloadP!=0){
        upsParametrs2[var].upsErrorStatus=1;
        snmpErrorBuf+=UPS_OUT_ERROR;
    }
    if ((upsParametrs2[var].upsinVolt<1700 || upsParametrs2[var].upsinVolt>1900) && upsParametrs2[var].upsinVolt!=0){
        upsParametrs2[var].upsErrorStatus=1;
        snmpErrorBuf+=UPS_IN_VOLT_ERROR;
    }
    if (upsParametrs2[var].upsOutStat !=2 && upsParametrs2[var].upsOutStat !=4 && upsParametrs2[var].upsOutStat !=8 &&
        upsParametrs2[var].upsOutStat !=10 && upsParametrs2[var].upsOutStat!=0){
        upsParametrs2[var].upsErrorStatus=1;
        //snmpErrorBuf+=upsParametrs2[var].upsOutStat;
        snmpErrorBuf+=UPS_ANY_STATUS_ERROR;
    }

    if (snmpErrorBuf==" "){
        upsParametrs2[var].upsErrorStatus=0;
        return;
    }
    if(upsParametrs2[var].upsErrorStatus!=0)
    {
        journal_printf("upsParametrs2[var].upsInx", "%s", encode_cp1251(translateUtf8, &snmpErrorBuf, sizeof (translateUtf8)));
    }
}


void snmp_button(){

    uint k=0;
    /* создаем список параметров упса */
    if(upsParametrs2.size()==0){
    for ( int i = 0; i < ups_count; i++ ) {
        upsParametrs2.push_back({0,0,0,0,0,0,0,0});
        k++;
    }
}
        /* создаем сокеты */
        if (socketServer == 0){
            snmp_server_open_sock();
            }

        if(socketServer <= 0){
            printf("error: snmp socket");
            snmp_server_open_sock();
        }

        if(socketServer0 <= 0){
            printf("error: snmp socket");
            snmp_server_open_sock();
        }

        k=0;

        /* отправляем каждому упсу */
      for ( int i = 0; i < map_count; i++ )
      {
            if (map[i].kind==MAP_UPS)
            {
               /* пишем адрес */
            sprintf (ini_addr_ups1, IP_MASK_FORMAT_SNMP, up_list[i].mask, map[i].ind);
            snmp_open_sock(ini_addr_ups1);

                if( up_list[i].mask == ini_udp_net2 ){
                    /* отправляем */
                snmp_sendmess(upsBatStatus, sizeof (upsBatStatus), socketServer);
                upsParametrs2[k].upsBatSt=snmp_recvmess(socketServer);
                if(upsParametrs2[k].upsBatSt == 0){
                    upsParametrs2[k].upsCap=0;
                    upsParametrs2[k].upsloadP=0;
                    upsParametrs2[k].upsinVolt=0;
                    upsParametrs2[k].upsOutStat=0;
                    upsParametrs2[k].upsInx=map[i].ind;
                    upsParametrs2[k].upsMap=i;
                    k++;
                    continue;
                }
                snmp_sendmess(upsCapacity, sizeof (upsCapacity), socketServer);
                upsParametrs2[k].upsCap=snmp_recvmess(socketServer);

                snmp_sendmess(upsLoadPower, sizeof (upsLoadPower), socketServer);
                upsParametrs2[k].upsloadP=snmp_recvmess(socketServer);

                snmp_sendmess(upsInputeVoltage, sizeof (upsInputeVoltage), socketServer);
                upsParametrs2[k].upsinVolt=snmp_recvmess(socketServer)/10;

                snmp_sendmess(upsOutputStatus, sizeof (upsOutputStatus), socketServer);
                upsParametrs2[k].upsOutStat=snmp_recvmess(socketServer);

                }

                if( up_list[i].mask == 100 ){
                    /* отправляем */
                snmp_sendmess(upsBatStatus, sizeof (upsBatStatus), socketServer0);
                upsParametrs2[k].upsBatSt=snmp_recvmess(socketServer0);
                if(upsParametrs2[k].upsBatSt == 0){
                    upsParametrs2[k].upsCap=0;
                    upsParametrs2[k].upsloadP=0;
                    upsParametrs2[k].upsinVolt=0;
                    upsParametrs2[k].upsOutStat=0;
                    upsParametrs2[k].upsInx=map[i].ind;
                    upsParametrs2[k].upsMap=i;
                    k++;
                    continue;
                }

                snmp_sendmess(upsCapacity, sizeof (upsCapacity), socketServer0);
                upsParametrs2[k].upsCap=snmp_recvmess(socketServer0);

                snmp_sendmess(upsLoadPower, sizeof (upsLoadPower), socketServer0);
                upsParametrs2[k].upsloadP=snmp_recvmess(socketServer0);

                snmp_sendmess(upsInputeVoltage, sizeof (upsInputeVoltage), socketServer0);
                upsParametrs2[k].upsinVolt=snmp_recvmess(socketServer0)/10;

                snmp_sendmess(upsOutputStatus, sizeof (upsOutputStatus), socketServer0);
                upsParametrs2[k].upsOutStat=snmp_recvmess(socketServer0);

                }

                upsParametrs2[k].upsInx=map[i].ind;
                upsParametrs2[k].upsMap=i;

                check_snmp_error(k);
                k++;
                }

           }
            gui_send_event(EVENT_SNMP_MESS);
            /* закрываем сокеты в режиме проверок */
            if(ups_count == 1 || socketServer0<=1 || socketServer<=1){
            // WIN32 ////////////////////////////////
            #ifdef __WIN32__
            closesocket(socketServer);
            closesocket(socketServer0);
            //UNIX//////////////////////////////////
            #else
            close(socketServer);
            close(socketServer0);
            #endif
            socketServer=0;
            socketServer0=0;
            }
      }
/////
//int snmpmess_init(){

//    return(CO_READY);
//}
//void snmpmess_uninit(void){

//}

//CORO_DEFINE (snmpmess)
//{

//    for( ; ; ){
//           CORO_WAIT(4444);
//           snmp_button();
//            CORO_YIELD();


//    }

//    CORO_END();
//}
