/***********************************************************************
 *          Система Автоматизированного Проектирования                 *
 *        Законов Управления Технологическими Процессами               *
 *          Версия технологии КОНТУР-М 8.0 для Windows                 *
 ***********************************************************************/

#ifndef __BASENET_H__
#define __BASENET_H__

#include <typedef.h>

#define NET_TRASMIT         500

enum NM_TEST
  {
   NO_TEST = 0,             // Нет тестирования
   TEST_MCP_1 = 1,          // Тест MCP
   TEST_MI_2 = 2,           // Тест измерительной части МИ
   TEST_ETHERNET = 3,       // Тест сети Ethernet
   TEST_KBD = 4,            // Тест клавиатуры
   TEST_RAM_EXP = 5,        // Тест экспресс RAM
   TEST_RAM = 6,            // Тест RAM
   TEST_HDD = 7,            // Тест HDD
   TEST_HDD_EXP = 8,        // Тест экспресс HDD
   TEST_CMOS = 9,           // Тест CMOS
   TEST_PIC = 10,           // Тест PIC
   TEST_MOUSE = 11,         // Тест mouse
   TEST_SPEAKER = 12,       // Тест системного динамика
   TEST_VIDEO_EXP = 13,     // Тест экспресс Video
   TEST_VIDEO = 14,         // Тест Video
  };

#define TEST_UPS  15          // Тест UPS
#define TEST_FULL  16          // test MCP/MI

#pragma pack(1)

typedef                      // Запрос на изменение состояния КВВ
struct  CIOREQUEST           // (вектор состояния КВВ)
  {
   _DWORD dwNumber;           // Номер изменения состояния КВВ
   _DWORD dwLenData;          // Длина данных
   _DWORD dwOffsetData;       // Смещение
   BYTE  Inputs[1];           // Сигналы ОУ
  } *PCIOREQUEST;

#pragma pack()

#endif
