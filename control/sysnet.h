/***********************************************************************
 *          Система Автоматизированного Проектирования                 *
 *        Законов Управления Технологическими Процессами               *
 *          Версия технологии КОНТУР-М 8.0 для Windows                 *
 ***********************************************************************/
// SYSNET.H - описание процедур работы с сетью

#ifndef __SYSNET_H__
#define __SYSNET_H__

#include <typedef.h>
#include <netdrv.h>
#include <drdisk.h>
#include <listmrmo.h>

enum TYPEABONENTS            // Типы абонентов (типы для MRMO_List)
  {
   MRMOY,                    // тип МРМО управления
   MRMOT,                    // тип МРМО техенолога
   IOCONT,                   // контроллер ввода вывода
   MRMOKPA,                  // ПКФ
   IOCONT_K,                 // контроллер ввода вывода
   MRMOVIEW,                 // модуль отображения
   SSBVS,                    // ССБВС
   MRMOY_KPA,                // ВУ ПКФ
   MI,                       // МИ
   MCP // тут
  };

enum MRMO_STATE             // Состояние работы МРМО
 {
  TURN_OFF     = 0,         // Выключен
  TURN_ON      = 0x01,      // Включен
  TEST_STATE   = 0x02,      // Проверки
  RESTORE_WORK = 0x04,      // Восстановление работы
  WORK         = 0x08,      // Штатная работа
  REMOTE_DEBUG = 0x10,      // Режим отладки
  PKF          = 0xFF,       // ПКФ
  UPSYELLOW    = 0xA1
 };

enum TestMaskMI
{
MASK_MI1 = 0x0100,
MASK_MI2 = 0x0200,
MASK_MI3 = 0x0400,
MASK_MI4 = 0x0800,
MASK_MI5 = 0x1000,
MASK_MI6 = 0x2000,
MASK_MI7 = 0x4000,
MASK_MI8 = 0x8000
};

#define NEW_WORK      0     // Новая работа
#define CONTINUE_WORK 1     // Продолжение работы
#define RESTORE       2     // Восстановление

#define COM_SENDVS          ((BYTE)1)  // передача вектора состояния
#define COM_SENDUSERVS      ((BYTE)2)  // сообщение об изменении ВС
#define COM_STATE           ((BYTE)3)  // Состояние
#define COM_CURTIME         ((BYTE)4)  // *cообщение о текущем времени
                                       // В этом случае MESSNET.byDataMess есть
                                       // TIMEDATE;
#define COM_SEND_SIP        ((BYTE)6)  // сообщение установка режима, варианта и синхронизация
#define CHAT_MESSAGE        ((BYTE)8)  // Сообщение чата
#define COM_RESET           ((BYTE)9)  // Перезагрузка

#define COM_TEST_NET        ((BYTE)10) // Тестовое сообщение сети Ethernet

#define COM_INIT_NETSTAT    ((BYTE)11) // Инициализация сбора статистики работы сети Ethernet
#define COM_GET_NETSTAT     ((BYTE)12) // Запрос статистики работы сети Ethernet
#define COM_NETSTATISTIC    ((BYTE)13) // Сообщение о статистике работы сети Ethernet

#define COM_GETBLOCKDATA    ((BYTE)20) // запрос на получение блока данных диска
                                       // В этом случае MESSNET.byDataMess есть
                                       // _DWORD dwAdresByte - номер блока на диске
#define COM_SENDBLOCKDATA   ((BYTE)21) // передача блока данных диска (ответ на запрос
                                       // о получении блока или самостоятельная передача
                                       // при загрузке РЗУ)
                                       // В этом случае MESSNET.byDataMess есть запись

#define COM_FILEDATA        ((BYTE)22) // Сообщение о передачи части ПО блока УПО
#define COM_CHECKBLOCKDATA  ((BYTE)23) // Проверить блок данных диска (new!)
#define COM_FILEWRDAT       ((BYTE)25) // Сообщение "Запись данных в таблицу"
#define COM_FILERDDAT       ((BYTE)26) // Сообщение "Чтение данных из таблицы"
#define COM_ROMOPER         ((BYTE)27) // Сообщение "Работа с ППЗУ"

#define COM_FILEDATAB       ((BYTE)28) // Сообщение "Данные таблицы"
#define COM_RUN_TEST        ((BYTE)30) // Сообщение о тестировании
#define COM_INIT_UPO        ((BYTE)31) // Инициализация блока УПО
#define COM_SETNAME_UPO     ((BYTE)32) // Задать имя блока УПО
#define COM_SETNAME         ((BYTE)33) // Задать имя МРМО


#define COM_PROGRESS        ((BYTE)40) // *Сообщение о выполнении длительной операции
#define COM_ENDPROGRESS     ((BYTE)41) // Сообщение о завершении операции
#define COM_START           ((BYTE)50) // * загрузка задачи управления
                                       // В этом случае MESSNET.byDataMess есть запись
                                       // START

#define COM_RESTORE         ((BYTE)51) // Восстановление

#define COM_GETDEBUGSTATE   ((BYTE)60) // запрос состояния отладки
#define COM_SENDDEBUGSTATE  ((BYTE)61) // состояние отладки
#define COM_GETMEMORYDATA   ((BYTE)62) // запрос блока памяти
#define COM_SENDMEMORYDATA  ((BYTE)63) // блок памяти
#define COM_SETBREAKPOINTS  ((BYTE)64) // добавить точку останова
#define COM_EXITDEBUG       ((BYTE)65) // продолжить
#define COM_STEPDEBUG       ((BYTE)66) // по шагам
#define COM_DEBUGMESS       ((BYTE)67) // отладочные сообщения


typedef BYTE SELF_DIAGNOSIS[2];        // Массив битов самодиагностики

#pragma pack(1)
typedef struct MESSNETHEAD   // Заголовок сетевого сообщения
  {                          // уровня задач приема/передачи
   WORD  wCheckSum;          // Циклический код сообщения
   _CHAR  IdDst;              // Номер абонента получателя
   _CHAR  IdSrc;              // Номер абонента отправителя
     #define SEND_ALL     ((char)-1)   // послать сообщение всем MRMO
     #define SEND_MRMOY   ((char)-2)   // послать сообщение всем МРМО_У
     #define SEND_BI      ((char)-4)   // послать сообщение всем каналам БИ
     #define SEND_PKF      0x7F        // ПКФ
    #define SEND_MCP1     ((char)4)
    #define SEND_MCP2     ((char)5)
    #define SEND_MCP3     ((char)6)
   BYTE  byCommand;          // Тип сообщения
   BYTE  bNodeState;         // Состояние узла
   SELF_DIAGNOSIS SE_VS;     // Биты самодиагностики абонента.
                             // Формируется абонентом сети Ethernet.
   _DWORD   Veryfi;           // Верификатор РЗУ
   _DWORD dwMessNum;          // Номер сообщения
  } *PMESSNETHEAD;

typedef struct MESSNET       // Сетевое сообщение уровня задач
  {                          // приема/передачи
   struct MESSNETHEAD Head;  // Заголовок сетевого сообщения
   BYTE  byDataMess          // Данные
      [MAX_NET_DATA_SIZE - sizeof(struct MESSNETHEAD)];
  } *PMESSNET;

typedef struct MESSAGENET    // Сетевое сообщение Ethernet
  {
   MAC     byDst;            // Адрес получателя
   MAC     bySrc;            // Адрес отправителя
   WORD    wID;              // Код пакета
   struct  MESSNET Mess;     // Данные
   BYTE    byAlignment       // Выравнивание длины пакета до двойного слова
          [ALIGNMENT((2*sizeof(MAC) + sizeof(WORD) + sizeof(struct MESSNET)), sizeof(_DWORD))];
  } *PMESSAGENET, FAR* LPMESSAGENET;

typedef struct TSTATEUPO
 {
  BYTE	 byState[4];		                       // Состояние системных ошибок
  _DWORD	dwCheckSumID;	                      // Контрольная сумма исходных данных
  _DWORD	dwCheckSumPO;	                      // Контрольная сумма ПО блока
 } *PTSTATEUPO;

typedef struct MRMOSTATE                    // Массив результатов тестирования
  {                                         // для всех абонентов сети
   _DWORD dwNumber;                          // Номер последнего принятого
                                            // (для чужого абонента) или переданного
                                            // (для своего абонента) тестового
                                            // сообщения с момента
                                            // начала тестирования
   _DWORD dwMissMess;                        // Число пропущенных сообщений
                                            // (для своего абонента – всегда 0)
  } *PMRMOSTATE;

typedef struct TESTMESS                     // Тестовое сообщение сети
  {
   _DWORD         dwRate;                    // Число сообщений в секунду
   struct TNETSTATISTIC Statistic;                 // Статистика
   struct MRMOSTATE     TestResult[1];             //
  } *PTESTMESS;

typedef struct FILEDAT                      // Сообщение о передачи части ПО блока УПО
  {
   WORD	 wAdr;                              // Адрес блока УПО
   _DWORD dwLenData;                         // Длина данных в байтах
   _DWORD dwSeekSet;                         // Указатель записи
   BYTE  byData[512];                       // Данные
  } *PFILEDAT;

typedef struct TESTEX                       // Расширенное сообщение о тестировании
  {
   BYTE mask[8];                            // маска получателей
   WORD wCmdEx;                             // расширенная команда
   WORD wNumTest;                           // тест
  } PTESTEX;

typedef struct TEST                         // Сообщение о тестировании
  {
   WORD wTestNum;                           // Номер теста
   WORD wRepeat;                            // Число повторений
                                            // (0 – непрерывное повторение)
   WORD wParam[1];                          // Параметры тестирования
                                            // (зависят от теста)
  } TEST, *PTEST;

typedef struct TEST2                        // Сообщение о тестировании
  {
   WORD wTestNum;                           // Номер теста
   WORD wRepeat;                            // Число повторений
                                            // (0 – непрерывное повторение)
   WORD wParam[2];                          // Параметры тестирования
                                            // (зависят от теста)
  } TEST2, *PTEST2;

typedef struct SET_UPO_ADR                  // Сообщение ПКФ об идентификации ячейки блока УПО
  {
   BYTE  adrCANold;                         // Старый адрес блока
   BYTE  adrCANnew;                         // Новый адрес блока
   BYTE  typeBlock;                         // Тип блока
   BYTE  bParam;                            // Резерв
  } *PSET_UPO_ADR;

typedef struct INIT_UPO                     // Сообщение ПКФ об инициализации блока УПО
  {
   BYTE  adrCAN;                            // Aдрес блока
   BYTE  typeBlock;                         // Тип блока
  } *PINIT_UPO;

typedef struct START                        // Сообщение о запуске РЗУ
  {
   BYTE  Regime;            // Режим запуска РЗУ
   _DWORD NumNZU;           // Номер РЗУ
   BYTE  byCurDay;          // Текущие день,
   BYTE  byCurMonth;        // месяц,
   WORD  wCurYear;          // год
   _LONG  CurTime;          // Текущее время в сек
   BYTE  byDay;             // День, месяц, год
   BYTE  byMonth;           // начала работы
   WORD  wYear;
   _DWORD StartTime;        // Время начала работы в ссек
  } *PSTART;

typedef struct PASSPROT     // Запрос о контроле паспорта протокола
  {                         // (блок 0 технологического диска)
   BYTE  byDay;             // День, месяц, год
   BYTE  byMonth;           //
   WORD  wYear;             //
   _DWORD Time;             // Время начала работы
   _DWORD Veryfi;           // Верификатор РЗУ
   _CHAR  szRemark          // Коментарий к протоколу
           [DISK_BLOCK_SIZE - sizeof(_DWORD) * 3 -
                           sizeof(WORD) -
                           sizeof(BYTE) * 2];
   _DWORD Reserved;
  } *PPASSPROT;

typedef struct  TIMEDATE             // Сообщение о текущем времени
  {
   BYTE  byCurDay;                   // Текущие день,
   BYTE  byCurMonth;                 // месяц,
   WORD  wCurYear;                   // год
   _DWORD CurTime;                    // Текущее время в сек
  }  *PTIMEDATE;


typedef struct DISKMEM                      // Дисковая память
  {
   _DWORD dwAdressBlock;              // Относительный номер блока на технологическом диске
   BYTE  byData[DISK_BLOCK_SIZE];    // Данные (содержимое блока)
  } *PDISKMEM;

typedef struct PROGRESS              // Сообщение о выполнении длительной операции
  {
   BYTE byCommand;                   // Код выполняемой операции
   BYTE byProgress;                  // Процент выполнения (0..100)
   WORD wTestNum;                    // Номер теста
   BYTE State[1];                    // Состояние тестирования
                                     // (зависят от теста)
  } *PPROGRESS;

typedef struct ENDPROGRESS                  // Сообщение о завершении операции
  {
   BYTE byCommand;                   // Код выполняемой операции
   BYTE byEndCode;                   // Код завершения
     #define END_OK     ((BYTE)0)    // Норма
     #define END_BREAK  ((BYTE)1)    // операция прервана
     #define END_ERROR  ((BYTE)2)    // ошибка выполнения операции
   WORD	wTestNum;                    // Номер теста
   _DWORD State[2];                   // Состояние тестирования
                                     // (зависит от теста)
  } *PENDPROGRESS;

typedef struct IRPSDATA                     // Сообщение между СС БВС и КА
  {
   BYTE byIRPSNumber;                       // Номер ИРПС
   BYTE byNumPacket;                        // Номер пакета
   BYTE byLenght;                           // Число информационных байт сообщения
   BYTE byData[1];                          // Информация сообщения
  } *PIRPSDATA;

typedef struct FILEWRDAT                    // Сообщение "Запись данных в таблицу"
  {
   WORD  wTblNum;                           // Номер записываемой таблицы
                                            // 0 - таблица исходных данных;
                                            // 1 - таблица градуировок пользователя;
                                            // 4 - таблица корректирующих кодов;
                                            // 5 - таблица сетевых настроек;
                                            // 0xff - таблица ВПО МИ
   WORD  wLenData;                          // Количество оставшихся байт данных
   _DWORD dwSeekSet;                         // Указатель записи (от начала таблицы)
   BYTE  byData[512];                       // Данные
  } *PFILEWRDAT;

typedef struct FILERDDAT                    // Сообщение "Чтение данных из таблицы"
  {
   WORD	wTblNum;                            // Номер считываемой таблицы
     #define TBL_ID     0                   // 0 - таблица исходных данных;
     #define TBL_GRD    1                   // 1 - таблица градуировок пользователя;
     #define TBL_TEST   2                   // 2 - таблица результатов тестирования;
     #define TBL_IC     3                   // 3 - таблица мониторинга измерительных каналов;
     #define TBL_CODE   4                   // 4 - таблица корректирующих кодов;
     #define TBL_NET    5                   // 5 - таблица сетевых настроек;
     #define TBL_PARAM  6                   // 6 - таблица заводских параметров;
                                            // 0xff - таблица ВПО МИ
   WORD  wRserved;                          // Зарезервировано для выравнивания
   _DWORD dwSeekSet;                         // Указатель начала чтения (от начала таблицы)
  } *PFILERDDAT;

typedef struct COMMANDS                     // Сообщение о командах от ЦОИ к ШИ
  {
   BYTE  byReg;                             // Режим.0- «останов», 1- «измерений»
   BYTE  byVar;                             // Номер варианта
   _DWORD dwSync;                            // Команда синхронизации
  } *PCOMMANDS;

typedef struct SIGNALS                      // Результаты измерений
  {
#define COUNT_PARAM        64            // Кол-во параметров в МИ

WORD  wVarSig;                           // Номер варианта
WORD  wCSProg;                           // циклический контрольный код ВПО МЦП
WORD  wCSProgMI[8];                      // циклический контрольный код ВПО МИ
WORD  wCSData;                           // циклический контрольный код ИД
WORD  wCSGrad;                           // циклический контрольный код таблицы
                                         // градуировок пользователя
WORD  wCSComp[8];                           // циклический контрольный код таблицы
                                         // корректирующих кодов
WORD  wCSNet;                            // циклический контрольный код таблицы
                                         // сетевых настроек
_DWORD dwSyncSig;                         // Последняя выполненная команда синхронизации
_DWORD dwSyncTime[3];                     // Время прихода последнего сообщения
                                         // синхронизации от ВУ1, ВУ2, ВУ3, относительно времени [мс]
                                         // соответствующей синхронизации, 3 времени по 4 байта;
_DWORD	dwSigTime;                         // время начала цикла измерений в [мс]
                                         // от последней команды синхронизации
WORD  wParamsNums[COUNT_PARAM];          // Условный порядковый номер(идентификатор) параметра
float flParams[COUNT_PARAM];             // результаты измерений параметров
BYTE  byParamsQuality[COUNT_PARAM];      // признаки качества(достоверности) параметров
  #define MSK_ERR_PROC  0x0F             // ошибки обработки
     // 0 - обработка результата измерений проведена успешно;
     // 1 - выход за нижний предел диапазона по электрической величине (переполнение АЦП в минус);
     // 2 - выход за верхний предел диапазона по электрической величине (переполнение АЦП в плюс);
     // 3 - (если нет ошибки 1) выход за нижнюю границу градуировки пользователя;
     // 4 - (если нет ошибки 2) выход за верхнюю границу градуировки пользователя;
     // 5 - (если нет ошибок 1 и 3) выход за нижнюю границу стандартной градуировки;
     // 6 - (если нет ошибок 2 и 4) выход за верхнюю границу стандартной градуировки;
  #define MSK_ERR_MEAS  0x70             // ошибки измерений
     // 0 - измерение и обработка результата измерений проведены успешно;
     // 1 - отсутствует связь с АЦП;
     // 2 - отказ АЦП по времени измерений (отказ тактового генератора);
     // 3 - напряжения питания измерительного канала вне нормы;
     // 4 - температура измерительного канала вне нормы;
  #define ERR_LINE      0x80             // отказ линии связи с датчиком
BYTE  byParamsLineStat[COUNT_PARAM];     // результаты тестирования линий связи
BYTE  byMIerrors;
BYTE  byReserve[3];
  } *PSIGNALS;

typedef struct ROMOPER                      // Сообщение "Работа с ППЗУ"
  {
   WORD wTblNum;                            // Номер таблицы, для которой выполняется операция
                                            // 0 - таблица исходных данных;
                                            // 1 - таблица градуировок пользователя;
                                            // 4 - таблица корректирующих кодов;
                                            // 5 - таблица сетевых настроек;
                                            // 6 - таблица заводских параметров;
                                            // 0xff - таблица ВПО МИ
   WORD wROMOpCode;                         // Код операции с ППЗУ
     #define ROM_READ   0                   // 0 - чтение таблицы из ППЗУ в ОЗУ;
     #define ROM_WRITE  1                   // 1 - запись таблицы из ОЗУ в ППЗУ;
  } *PROMOPER;

typedef struct REFCODTABLE           // Элемент таблицы перевода для абонента
  {
   BYTE  TypeMODULE;                 // Тип абонента (модуля АКУ)
   BYTE  NumMODULE;                  // Номер среди однотипных модулей (три КВВ, образующие
                                     // трехканальный модуль АКУ СП
                                     // имеют один и тот же номер)
   WORD  wNumModuleID;               // Номер в каталоге исходных данных
   BYTE  byChannal;                  // Номер канала
   WORD  wState;                     // Номер первого байта состояния модуля в векторе
                                     // состояния модулей (системных ошибок).
                                     // Состояние включает NODE_STATE и SELF_DIAGNOSIS
   WORD  wSignalsQuan;               // Число сигналов (переменных) абонента
                                     // (без NODE_STATE и SELF_DIAGNOSIS)
   _DWORD dwVSCodTable;               // Относительная ссылка на таблицу соответствия
                                     // вектора состояния абонента вектору состояния УВК
  }* PREFCODTABLE;

#pragma pack()

#endif
