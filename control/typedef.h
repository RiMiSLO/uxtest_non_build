/***********************************************************************
 *  Автоматизированная Система Управления Технологическими Процессами  *
 *          Версия технологии КОНТУР-М 1.0 для Windows                 *
 *    Разработано в 1996 году отд. 063 РКК "Энергия" им. С.П. Королева *
 ***********************************************************************/

// Определение типов для системы реализации законов управления
// typedef.h        Селиван В.С.

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#define FAR

#include <stdint.h>

typedef int8_t                   _CHAR;
typedef uint8_t                  BYTE;
typedef uint16_t                 WORD;
typedef int32_t                  _LONG;
typedef int16_t                  SINT;
typedef uint32_t                 _DWORD;
typedef uint64_t                 DDWORD;

typedef void *                   PVOID;
typedef void const *             PCVOID;
typedef int8_t *                 _PCHAR;
typedef char*                    PSTR;
typedef char const*              PCSTR;
typedef uint8_t *                PBYTE;
typedef uint16_t *               PWORD;
typedef int32_t *                _PLONG;
typedef uint32_t *               _PDWORD;

typedef PVOID                    LPVOID;
typedef PCVOID                   LPCVOID;
typedef _PCHAR                   LPCHAR;
typedef PSTR                     LPSTR;
typedef PCSTR                    LPCSTR;
typedef PBYTE                    LPBYTE;
typedef PWORD                    LPWORD;
typedef _PLONG                    _LPLONG;
typedef _PDWORD                  _LPDWORD;

typedef _DWORD                   _BOOL;

typedef _DWORD                   _COLORREF;
typedef _COLORREF *              PCOLORREF;

#define _MAXCHAR                 ((_CHAR)0x7f)
#define _MINCHAR                 ((_CHAR)0x80)
#define _MAXBYTE                 ((BYTE)0xff)

#define _MAXINT                  ((SINT) 0x7fff)
#define _MININT                  ((SINT) 0x8000)
#define _MAXWORD                 ((WORD)0xffff)

#define _MAXLONG                 ((_LONG) 0x7fffffffL)
#define _MINLONG                 ((_LONG) 0x80000000L)
#define _MAXDWORD                ((_DWORD)0xffffffffL)

#define _MAXREAL                 ((_REAL)  999999.999)
#define _MINREAL                 ((_REAL) -999999.999)

// Типы переменных в законе управления
typedef _CHAR                    _INT;
typedef SINT                     _LINT;
typedef _LONG                    _TIME;
typedef float                    _REAL;
#define MAXTIME                  MAXLONG
#define MINTIME                  MINLONG

typedef _CHAR  *                 P_INT;
typedef SINT  *                  P_LINT;
typedef _LONG  *                 P_TIME;
typedef float *                  P_REAL;

typedef _CHAR  *                 ST_INT;
typedef SINT  *                  ST_LINT;
typedef _LONG  *                 ST_TIME;
typedef float *                  ST_REAL;
typedef _DWORD *                 ST_DWORD;

#endif // __TYPEDEF_H__

