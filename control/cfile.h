#ifndef __CFILE_H__
#define __CFILE_H__

#include <typedef.h>
#include <drdisk.h>

#pragma pack(1)
typedef struct BOOTSECTOR
  {
   char  boot[DISK_BLOCK_SIZE - 3 * sizeof(_DWORD)]; // Код загрузчика
   _DWORD dwSizeFileSystem;                          // Размер файловой системы (в блоках)
   _DWORD dwCheckSum;                                // Контрольная сумма диска
   _DWORD dwMagicWord;                               // Магическое слово
  } *PBOOTSECTOR;
#pragma pack()

#define BEGIN_FILE_SYSTEM 3
#define MRMO_NAME_FILE    2

#define FILE_NAME_SIZE  23

#endif
