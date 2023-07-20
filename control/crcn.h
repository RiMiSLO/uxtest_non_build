#ifndef __CRC_H_
#define __CRC_H_

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

WORD CRC16           // Возвращает циклическую контрольную сумму
  (
   const BYTE * msg, // адрес сообщения для вычисления CRC кода
   _DWORD  dataLen   // длина сообщения в байтах
  );

#ifdef __cplusplus
}
#endif

#endif
