/*--------------------------------------------------------------------
                   "АПС" Ростов-на-Дону
                        NetDrv.h
   Файл описаний основных данный и процедур драйвера Ethernet
                   версия 1.0   01/12/2003
--------------------------------------------------------------------*/
#ifndef __NETDRV_H
#define __NETDRV_H

#include <typedef.h>
#include <sapr_lim.h>

// Макрос вычисления числа, на которое нужно увеличить L,
// чтобы получить число, кратное S
#define ALIGNMENT(L, S) (((L)+(S-1))/(S)*(S) - (L))

typedef BYTE      MAC[6];                   // MAC адрес
typedef MAC*      PMAC;                     // Ссылка на MAC адрес
typedef MAC FAR*  LPMAC;                    // Дальняя ссылка на MAC адрес

#define LENGTH_IA sizeof(MAC)               // Длина IA (MAC) адреса

#pragma pack(1)

typedef struct ETHERNETMESS                 // Сетевое сообщение Ethernet
  {
   MAC   byDst;                             // Адрес получателя
   MAC   bySrc;                             // Адрес отправителя
   WORD  wID;                               // Код пакета
   BYTE  Mess [MAX_NET_DATA_SIZE];          // Данные
   BYTE  byAlignment                        // Выравнивание длины пакета до двойного слова
        [ALIGNMENT((2*LENGTH_IA + sizeof(WORD) + MAX_NET_DATA_SIZE), sizeof(_DWORD))];
  } *PETHERNETMESS, FAR* LPETHERNETMESS;


#define LEN_MESSAGENET_HEAD (2*sizeof(MAC) + sizeof(WORD)) // Длина заголовка сетевого
                                                           // сообщения уровня драйвера ETHERNET

#define MAX_PACKET_SIZE sizeof(ETHERNETMESS)// Максимальный размер пакета Ethernet
#define MIN_PACKET_LENGTH    64             // Минимальный размер пакета Ethernet

#define MAX_NET_DEVICES       5             // Максимальное количество сетевых устройств

#define NUM_RX_BUFFERS        8             // Число буферов приема сообщений
#define NUM_TX_BUFFERS        1             // Число буферов передачи сообщений

typedef struct TNETSTATISTIC                // Статистика работы карты
  {
   _DWORD dwDisconnect;                      // Число потерь связи (отсоединений кабеля)
   _DWORD dwLostCarrier;                     // Число потерь несущей

   _DWORD dwRxGoodFrame;                     // Число успешно принятых пакетов (по аппаратным счетчикам)
   _DWORD dwRxOk;                            // Число успешно принятых пакетов (по драйверу)
   _DWORD dwRxDisable;                       // Число отключений (запретов) приема
   _DWORD dwRxError;                         // Число ошибок приема (по аппаратным счетчикам)
   _DWORD dwRxErrorPacket;                   // Число ошибочных принятых пакетов (по драйверу)
   _DWORD dwRxMinPacketLength;               // Число приемов слишком коротких пакетов
   _DWORD dwRxMaxPacketLength;               // Число приемов слишком длинных пакетов
   _DWORD dwRxErrorCRC;                      // Число ошибок CRC при приеме
   _DWORD dwRxFrameAlign;                    // Число ошибок выравнивания пакета
   _DWORD dwRxOverrun;                       // Количество пакетов, не принятых из-за переполнения приемного FIFO
   _DWORD dwRxErrResource;                   // Число ошибок, связанных с нехваткой места в буфере

   _DWORD dwTxGoodFrame;                     // Число успешно переданных пакетов (по аппаратным счетчикам)
   _DWORD dwTxOk;                            // Число успешно переданных пакетов (по драйверу)
   _DWORD dwTxWriteOk;                       // Число пакетов, успешно внесенных в буфер (по драйверу)
   _DWORD dwTxDisable;                       // Число отключений (запретов) передачи
   _DWORD dwTxLostCarrier;                   // Число потерь несущей при передаче
   _DWORD dwTxAbort;                         // Число прерываний передачи
   _DWORD dwTxMaxCollision;                  // Число непереданных пакетов из-за превышения числа коллизий
   _DWORD dwTxUnderrun;                      // Число непереданных пакетов из-за нехватки данных при DMA
   _DWORD dwTxJabber;                        // Число ошибок из-за длинной передачи
   _DWORD dwTxBusy;                          // Число невозможности передачи из-за занятости буфера

  }*PNETSTATISTIC, FAR *LPNETSTATISTIC;
                                            // Коды ошибок и завершения операций
#define RESULT_OK          0                // Нет ошибок
#define RESULT_ERR         1                // Ошибка операции
#define ERR_CODE_PARAM     2                // Ошибка в параметрах вызова
#define ERR_NO_DEV         3                // Неправильный номер карты
#define NOPACKET           0                // Нет принятого пакета
#define ERPACKET           0x1000           // Пакет принят с ошибкой
#define CAN_TRANSMIT       0                // Передающий буфер драйвера свободен для пакета указанной длины
#define CAN_NOT_TRANSMIT   1                // Нет места для пакета указанной длины
#define TRANSMIT           0                // Успешный перенос пакета во внутренний буфер и выдача
                                            // команды устройству
#define NOTRANSMIT         1                // Пакет не передан

#pragma pack()

#endif   /* __NETDRV_H  */
