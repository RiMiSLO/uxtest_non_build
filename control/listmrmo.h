/***********************************************************************
 *          Система Автоматизированного Проектирования                 *
 *        Законов Управления Технологическими Процессами               *
 *          Версия технологии КОНТУР-М 1.0 для Windows                 *
 *    Разработано в 1995 году отд. 065 РКК "Энергия" им. С.П. Королева *
 ***********************************************************************/

#ifndef __LISTMRMO_H__
#define __LISTMRMO_H__

#include <sapr_lim.h>
#include <netdrv.h>

#pragma pack(push, 1)
typedef struct                           // Структура описания рабочего места
  {
   BYTE NameMRMO[MAX_MRMO_NAMELEN+1];    // имя МРМО в системе
   BYTE TypeMRMO;                        // Тип абонента:
   BYTE byChannel;                       // Канал абонента (0..)
   BYTE byNetNumsMask;                   // Битовая маска сетей, к которым должен
                                         // быть подключен данный абонент (бит N –
                                         // абонент должен быть подключен к сети N)
  }LISTMRMO, * PLISTMRMO;

typedef struct                           // Структура описания сети
  {
   BYTE  NetNumber;                      // Номер сети
   MAC   byAdapterName;                  // MAC – адрес сетевого адаптера
  } NETDEF, * PNETDEF;

typedef struct                           // Структура идентифицирующей записи
  {
   BYTE   NameMRMO [MAX_MRMO_NAMELEN+1]; // Имя абонента в системе
   BYTE   byAdapters;                    // Число сетей (1..3)
   NETDEF NetDevise[QCHANAL];            // Описание сетей (по числу сетей)
  } MRMONAME, *PMRMONAME;

#pragma pack(pop)

#endif
