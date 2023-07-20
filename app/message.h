/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  message.h
 *  \brief  Сообщение сетевого обмена.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */




#ifndef MESSAGE_H
#define MESSAGE_H

//#include <string>

#ifdef  MESSAGE_IMPLEMENT
#define MESSAGE_EXT
#define MESSAGE_EXT_INIT( dec, init ) \
	dec = init
#else
#define MESSAGE_EXT extern
#define MESSAGE_EXT_INIT( dec, init ) \
	extern dec
#endif

/**
 *  \defgroup message Сообщение сетевого обмена
 *  \ingroup udp
 *  \{
 */

/**
 *  \brief Маска рабочих станций.
 */
typedef uint32_t ST_MASK;

/**
 *  \brief Маска МИ.
 */
typedef uint32_t MI_MASK;


/**
 *  \defgroup wCommand Команды сетевого обмена
 *  \{
 */

#define CMD_VS                  ( BYTE ( 1 ))       /**< Вектор состояния. \hideinitializer */
#define CMD_VSSU                COM_SENDUSERVS      /**< Запрос на изменение вектора состояния. \hideinitializer */
#define CMD_STATE               COM_STATE           /**< Вектор состояния станции. \hideinitializer */
#define CMD_SET_DATETIME        COM_CURTIME         /**< Установка даты и времени. \hideinitializer */
#define CMD_REBOOT              COM_RESET           /**< Перезагрузка рабочей станции. \hideinitializer */
#define CMD_TEST_NET            COM_TEST_NET        /**< Тестовое сообщение. \hideinitializer */
#define CMD_STAT_RESET          COM_INIT_NETSTAT    /**< Сброс статистики. \hideinitializer */
#define CMD_STAT_REQUEST        COM_GET_NETSTAT     /**< Запрос статистики. \hideinitializer */
#define CMD_STAT                COM_NETSTATISTIC    /**< Статистика работы сети. \hideinitializer */
#define CMD_TD_READ_BLOCK       COM_GETBLOCKDATA    /**< Чтение блока технологического диска. \hideinitializer */
#define CMD_TD_WRITE_BLOCK      COM_SENDBLOCKDATA   /**< Запись блока данных диска. \hideinitializer */
#define CMD_TD_CHECK_BLOCK      COM_CHECKBLOCKDATA  /**< Проверка блока технологического диска. \hideinitializer */
#define CMD_BL_PROG             COM_FILEDATA        /**< ПО блока. \hideinitializer */
#define CMD_EX                  COM_RUN_TEST        /**< Расширенная команда ПУНВД. \hideinitializer */
#define CMD_NAME_BLOCK          COM_SETNAME_UPO     /**< Идентификация блока. \hideinitializer */
#define CMD_INIT_BLOCK          COM_INIT_UPO        /**< Инициализация блока. \hideinitializer */
#define CMD_NAME                COM_SETNAME         /**< Идентификация станции. \hideinitializer */
#define CMD_RESULT              COM_ENDPROGRESS     /**< Результат операции. \hideinitializer */
#define CMD_START               COM_START           /**< Запуск задачи управления. \hideinitializer */
#define CMD_RESTORE             COM_RESTORE         /**< Восстановление задачи управления. \hideinitializer */
#define CMD_MI_SEND_TABLE       COM_FILEWRDAT       /**< Передать блок данныx таблицы в МИ. \hideinitializer */
#define CMD_MI_PROG_TABLE       COM_ROMOPER         /**< Запись таблицу в EEPROM МИ. \hideinitializer */
#define CMD_MI_FILE_REC         COM_FILEDATAB       /**< Данные от мцп (таблица) для проверки. \hideinitializer*/
#define CMD_MI_READ             COM_FILERDDAT
/** \} */

/**
 *  \brief Расширенный заголовок (ПУНВД).
 */
#pragma pack( 1 )
typedef struct {
    uint32_t mask;  /**< Битовая маска получателей. */
    uint32_t mask1;
	uint16_t cmd;   /**< Расширенная команда. */
} EX_HEADER;
#pragma pack()

#define MESSAGE_TRANSMITTED     0
#define MESSAGE_IN_QUEUE        1
#define MESSAGE_DISCARDED       2

/**
 *  \brief Сетевое сообщение.
 */
typedef struct {
    uint32_t mask;                  /**< Битовая маска получателей. */
	int8_t   abonent;               /**< Станция отправитель/получатель. */
	uint8_t  net;                   /**< Номер сети принятого сообщения / 0 - сообщение для станций, 2 - для МИ. */
	uint8_t  cmd;                   /**< Команда. */
	uint8_t  ex_cmd;                /**< Расширенная команда. */
	int16_t  len;                   /**< Размер данных (меньше 0 - статические данные). */
	uint8_t  _short;                /**< 0 - указатель на данные; 1 - данные. */
	union {
	void*    data;                  /**< Указатель на данные. */
	uint32_t dw_data;               /**< Данные. */
	};
    uint8_t thirdbait;              /**< третий байт ip-адреса. */
	volatile
	uint8_t  *transmitted;          /**< Указатель на признак - Сообщение передано. */
 //   bool isForUps;                  /**< Признак того, что сообщение относится к только к UPS-ам. */




	/**
	 *  \brief Получить размер данных.
	 *  \return Размер данных.
	 */
	__inline uint16_t get_len( void )
	{
		return abs( len );
	}

	/**
	 *  \brief Получить ссылку на данные.
	 *  \return Ссылка на данные.
	 */
	__inline void *get_data( void )
	{
		return ( _short ) ? &dw_data : data;
	}

	/**
	 *  \brief Заполнить данные.
	 *  \param val Данные.
	 *  \param sz Размер данных.
	 */
	__inline void set_data_short( uint32_t val, uint16_t sz )
	{
		_short = 1;
		len = -(( sz < sizeof( dw_data )) ? sz : sizeof( dw_data ));
		dw_data = val;
	}

	/**
	 *  \brief Выделить память для данных.
	 *  \param sz Размер данных.
	 */
	__inline void *malloc_data( uint16_t sz )
	{
		_short = ( sz <= sizeof( dw_data ));
		if ( _short ) {
			len = -sz;
		} else {
			len = sz;
			data = malloc( len );
		}
		return get_data();
	}

	/**
	 *  \brief Освободить память данных.
	 */
	__inline void free_data( void )
	{
		if ( len > 0 ) {
			free( data );
		}
		data = NULL;
		len = 0;
	}

	/**
	 *  \brief Заполнить признак передачи сообщения.
	 *  \param var Указатель на признак.
	 */
	__inline void init_transmitted( volatile uint8_t *var )
	{
		transmitted = var;
		if ( transmitted == NULL ) return;

		*transmitted = MESSAGE_IN_QUEUE;
	}

	/**
	 *  \brief Установить признак передачи сообщения.
	 *  \param val Значение признака.
	 */
	__inline void set_transmitted( uint8_t val )
	{
		if ( transmitted == NULL ) return;

		*transmitted = val;
		transmitted = NULL;
	}

} MESSAGE_REMOTE;






/**
 *  \brief Передача сетевого сообщения станциям.
 *  \param abonent Получатель.
 *  \param cmd Команда.
 *  \param mask Битовая маска получателей.
 *  \param ex_cmd Расширенная команда.
 *  \param sz Размер данных.
 *  \param data Указатель на данные.
 *  \param in_heap Признак - данные в куче.
 *  \param transmitted Указатель на признак - Сообщение передано.
 */
MESSAGE_EXT void message_transmit(
	int      abonent,
	uint32_t cmd,
	uint32_t mask = 0,
	uint32_t ex_cmd = 0,
	uint32_t sz = 0,
	void*    data = NULL,
	bool     in_heap = false,
	uint8_t  *transmitted = NULL
);

/**
 *  \brief Передача сетевого сообщения МИ.
 *  \param abonent Получатель.
 *  \param cmd Команда.
 *  \param mask Битовая маска получателей.
 *  \param ex_cmd Расширенная команда.
 *  \param sz Размер данных.
 *  \param data Указатель на данные.
 *  \param in_heap Признак - данные в куче.
 *  \param transmitted Указатель на признак - Сообщение передано.
 */
MESSAGE_EXT void message_transmit2(
	int      abonent,
	uint32_t cmd,
	uint32_t mask = 0,
	uint32_t ex_cmd = 0,
	uint32_t sz = 0,
	void*    data = NULL,
	bool     in_heap = false,
	uint8_t  *transmitted = NULL
);

// nik

MESSAGE_EXT void message_transmit3(
        int      abonent,
        uint32_t cmd,
        uint32_t mask = 0,
        uint32_t ex_cmd = 0,
        uint32_t sz = 0,
        void*    data = NULL,
        bool     in_heap = false,
        uint8_t  *transmitted = NULL

);


/** \} */

#endif /* MESSAGE_H */
