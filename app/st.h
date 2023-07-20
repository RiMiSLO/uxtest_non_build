/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  st.h
 *  \brief  Чтение конфигурации рабочих мест.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */
#include <vector>
#ifndef ST_H
#define ST_H

#ifdef  ST_IMPLEMENT
#define ST_EXT
#define ST_EXT_INIT( dec, init ) \
	dec = init
#else
#define ST_EXT extern
#define ST_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \defgroup st Конфигурация сети
 *  \ingroup app
 *  \{
 */

/** \} */

/**
 *  \defgroup map_kind Вид элемента в схеме
 *  \{
 */
#define MAP_NONE                0                 /**< Нет элемента \hideinitializer */
#define MAP_ST                  1                 /**< Рабочая станция \hideinitializer */
#define MAP_BL                  2                 /**< Блок УПО \hideinitializer */
#define MAP_MI                  3                 /**< МИ \hideinitializer */
#define MAP_IMAGE               4                 /**< Статическое изображение \hideinitializer */
#define MAP_UPS                 5                 /**< Источник бесперебойного питания \hideinitializer */
#define MAP_MCP                 6                 /**< Мoдуль центрального процессора \hideinitializer */


/** \} */

/**
 *  \brief Номер ПКФ по умолчанию.
 *  \hideinitializer
 */
#define ST_TST_DEFAULT          ( DATN_DEPTH - 1 )

/**
 *  \brief Максимальное количество элементов схемы. \hideinitializer
 */
#define MAP_COUNT               255
#define PAGE_COUNT              4

#define BR_LINE                 1
#define BR_COLUMN               2
#define BR_PAGE                 3

/**
 *  \brief Элемент схемы расположения элементов.
 */
ST_EXT struct {
	uint8_t kind;                                 /**< Вид элемента */
    uint8_t ind;                                  /**< Индекс */
	uint8_t pic;                                  /**< Индекс картинки для режима проверок */
	uint8_t pico;                                 /**< Индекс картинки для режима - выключен */
	uint8_t pice;                                 /**< Индекс картинки для состояния ошибки */
	uint8_t picw;                                 /**< Индекс картинки для режима штатной работы */
    uint8_t picp;                                 /**< Индекс картинки для режима штатной работы с некритическими ошибками для UPS */
    uint16_t x;                                    /**< Смещение активной области картинки / x-координата картинки */
    uint16_t y;                                    /**< y-координата картинки */
	int8_t  perx;                                 /**< Смещение строки процента выполнения теста */
    uint8_t mask;                                 /**< Маска сети ups !!*/
	QString mark;                                 /**< Маркировка элемента */
	int16_t br;                                   /**< Разрыв схемы ( 1 - конец строки
																	  2 - конец колонки
																	  3 - конец страницы
																	< 0 - смещение по Y в пикселях ) */
    uint8_t position;                             /**< Тип позиционирования элемента: 0 - относительный, 1 - абсолютный */
    uint8_t type;                                 /**< Тип абонента */
    uint16_t label_x;                             /**< Смещение надписи относительно картинки / x-координата */
    uint16_t label_y;                             /**< y-координата */
} map[ MAP_COUNT ];

/**
 *  \brief Страница схемы.
 */
ST_EXT struct {
	QString name;                                 /**< Имя страницы */
	uint8_t first;                                /**< Первый элемент */
} page_list[ PAGE_COUNT ];

// NIK
// сообщения-параметры ибп
typedef struct
{
    int upsCap;             /**< запас баттареи */
    int upsBatSt;           /**< состояние */
    int upsloadP;           /**< номинальная мощность */
    int upsinVolt;          /**< входное линейное напряжение */
    int upsOutStat;         /**< состояние */
    int upsErrorStatus;     /**< есть ли ошибка */
    int upsInx;             /**< индес в массиве st_list_ex1 */
    int upsMap;             /**< положение в массиве map */
} upsParams;

ST_EXT std::vector<upsParams> upsParametrs2;

ST_EXT int ups_count;
// NIK

/**
 *  \brief Число элементов схемы.
 */
ST_EXT uint8_t map_count;

/**
 *  \brief Число страниц схемы.
 */
ST_EXT uint8_t page_count;

/**
 *  \brief Максимальное количество картинок. \hideinitializer
 */
#define MAP_PIC_COUNT  255
// Индекс начало группы картинок монитора
#define MAP_PIC_INDEX_MONITOR 0

/**
 *  \brief Имена файлов картинок.
 */
ST_EXT QString map_pic[ MAP_PIC_COUNT ];

/**
 *  \brief Число картинок.
 */
ST_EXT uint8_t map_pic_count;

/**
 *  \brief Максимальное количество программ блоков. \hideinitializer
 */
#define MAP_HEX_COUNT  CAN_ADDR_MAX

/**
 *  \brief Имена файлов программ блоков.
 */
ST_EXT QString map_hex[ MAP_HEX_COUNT ];

/**
 *  \brief Число программ блоков.
 */
ST_EXT uint8_t map_hex_count;

/**
 *  \brief Элемент списка блоков.
 */
typedef struct {
	QString name;                                 /**< Название блока. */
	char name_ch[ 64 ];                           /**< Название блока (char[]). */
	uint8_t type;                                 /**< Тип блока. */
	uint8_t addr;                                 /**< Адрес блока. */
	uint8_t ind;                                  /**< Индекс блока на схеме. */
	uint8_t gr;                                   /**< Группа. */
    uint32_t app;                                 /**< Эталонная контрольная сумма программы. */
	int8_t  rs;                                   /**< Битовая маска каналов блока подключенных к последовательным
													   интерфейсам для БИК и БПИ */
	uint8_t hex;                                  /**< Индекс имени файла программы */

} BL_LIST;

typedef struct {
    QString name;                                 /**< Название ВУ. */
    char name_ch[ 64 ];                           /**< Название ВУ (char[]). */
    uint8_t ind;                                  /**< Индекс станции на схеме. */
    uint8_t gr;                                   /**< Группа. */
    uint8_t can_ol;                               /**< Маска наличия связи по сетям CAN. */
    uint8_t mask;                                 /**< Маска точнее третий байт ip-адреса. */
} UP_LIST;



/**
 *  \brief Список блоков.
 */
ST_EXT BL_LIST bl_list[ BL_LIMIT ];

/**
 *  \brief Контрольные суммы блока.
 */
typedef struct {
    uint32_t app;                                 /**< Контрольная сумма программы. */
	uint16_t source;                              /**< Контрольная сумма исходных данных. */
} BL_CHSUM;

/**
 *  \brief Контрольные суммы блоков.
 */
ST_EXT BL_CHSUM bl_chsum[ BL_LIMIT ][ BL_CH_COUNT ];

/**
 *  \brief Число блоков.
 */
ST_EXT uint8_t bl_count;

/**
 *  \brief Элемент списка групп блоков.
 */
typedef struct {
	uint8_t st[ BL_HOST_COUNT ];                  /**< Станции подключения блоков. */
	uint8_t bl_count;                             /**< Количество блоков в группе (UPO_Init.dat). */
	uint8_t bl_limit;                             /**< Количество блоков в группе (map.cfg). */
	uint8_t bl[ BL_LIMIT ];                       /**< Индексы блоков в bl_list (после запуска - по map.cfg,
													   после выбора РЗУ - по UPO_Init.dat). */
} GR_LIST;

/**
 *  \brief Список групп блоков.
 */
ST_EXT GR_LIST gr_list[ GR_LIMIT ];

/**
 *  \brief Элемент списка МИ.
 */
typedef struct {
	QString name;                                 /**< Название МИ. */
	char name_ch[ 64 ];                           /**< Название МИ (char[]). */
	uint16_t sn;                                  /**< Заводской номер МИ. */
	uint16_t verify_c;                            /**< Верификатор коррект. кодов. */
	uint8_t ind;                                  /**< Индекс МИ на схеме. */
} MI_LIST;

#define MI_TYPE    MI

#define MI_ID_MIN    0
#define MI_ID_MAX    23
#define MI_COUNT     ( MI_ID_MAX - MI_ID_MIN + 1 )

#define MI_MASK_ALL  ((( 1 << MI_COUNT ) - 1 ) << MI_ID_MIN )

/**
 *  \brief Список МИ.
 */
ST_EXT MI_LIST mi_list[ DATM_DEPTH ];

ST_EXT uint8_t gr_mi_count1;
ST_EXT uint8_t gr_mi_count2;
ST_EXT uint8_t gr_mi_count3;


/**
 *  \brief Список UPS.
 */
ST_EXT UP_LIST up_list[ 255 ];

/**
 *  \brief Число МИ.
 */
ST_EXT uint8_t mi_count;

/**
 *  \brief Маска выбранных МИ на схеме.
 */
ST_EXT MI_MASK mi_mask;

//nik

#define MCP_TYPE    MCP
#define MCP_ID_MIN    5
#define MCP_ID_MAX    7
#define MCP_COUNT     ( MCP_ID_MAX - MCP_ID_MIN + 1 )

#define MCP_MASK_ALL  ((( 1 << MCP_COUNT ) - 1 ) << MCP_ID_MIN )

/**
 *  \brief Элемент списка МЦП.
 */

typedef struct {
    QString name;                                 /**< Название МЦП. */
    char name_ch[ 64 ];                           /**< Название МЦП (char[]). */
    uint16_t snmcp;                                  /**< Заводской номер МЦП. */
    uint16_t verify_c;                            /**< Верификатор коррект. кодов. */
    uint8_t ind;                                  /**< Индекс МЦП на схеме. */
    uint8_t addr;
} MCP_LIST;

/**
 *  \brief Список МЦП
 */

ST_EXT MCP_LIST mcp_list[MCP_ID_MAX - MCP_ID_MIN + 1];

/**
 * @brief количество МЦП
 */
ST_EXT int mcp_count;

/**
 * @brief маска выбранных МЦП на схеме
 */
ST_EXT MI_MASK mcp_mask;


//nik

/**
 *  \brief Дополнительные поля для списка станций.
 */
typedef struct {
	QString name;                                 /**< Название ВУ. */
	char name_ch[ 64 ];                           /**< Название ВУ (char[]). */
	uint8_t ind;                                  /**< Индекс станции на схеме. */
	uint8_t gr;                                   /**< Группа. */
	uint8_t can_ol;                               /**< Маска наличия связи по сетям CAN. */
    uint8_t mask;
} ST_LIST_EX;

/**
 *  \brief Список станций.
 */
ST_EXT LISTMRMO st_list[ 255 ];

/**
 *  \brief Дополнительные поля списка станций.
 */
ST_EXT ST_LIST_EX st_list_ex[ 255 ];

/**
 *  \brief Список станций.
 */
ST_EXT LISTMRMO st_list1[ 255 ];

/**
 *  \brief Дополнительные поля списка станций.
 */
ST_EXT ST_LIST_EX st_list_ex1[ 255 ];


/**
 *  \brief Число станций.
 */
ST_EXT uint8_t st_count1;

/**
 *  \brief Число станций.
 */
ST_EXT uint8_t st_count;

/**
 *  \brief Индекс ПКФ в списке станций.
 */
ST_EXT uint8_t st_tst;

/**
 *  \brief Маска МРМОУ.
 */
ST_EXT uint32_t st_host_mask;

/**
 *  \brief Маска выбранных рабочих станций на схеме.
 */
ST_EXT ST_MASK st_mask;

/**
 *  \brief Число РЗУ.
 */
ST_EXT uint8_t rzu_count;
ST_EXT uint8_t rzu_error;
ST_EXT uint8_t dbg_td;

/**
 *  \brief Индекс выбранного РЗУ.
 */
ST_EXT_INIT( uint8_t rzu_ind, RZU_LIMIT );

/**
 *  \brief Список пакетов тестов.
 */
ST_EXT PACK_TEST pack_list[ PACK_COUNT ];

/**
 *  \brief Паспорта РЗУ.
 */
ST_EXT struct PASSPROT rzu_passport[ RZU_LIMIT ];

#ifdef __cplusplus
/**
 *  \brief Папки РЗУ.
 */
ST_EXT QString rzu_dir[ RZU_LIMIT ];

/**
 *  \brief Проверка списка станций.
 *  \return Признак успешного завершения операции.
 */
ST_EXT int st_list_check( QString fn );

ST_EXT int bl_list_read( QString fn );

#endif

/**
 *  \brief Чтение описания рабочих станций и блоков УПО.
 */
ST_EXT void map_read( void );

ST_EXT void map_free( void );

ST_EXT void mi_comp_read( void );

ST_EXT void mcp_comp_read( void );

ST_EXT int st_list_write( void );

/**
 *  \brief Чтение РЗУ.
 */
ST_EXT void rzu_read( void );

ST_EXT uint8_t rzu_search( dword verify );

/**
 *  \brief Чтение пакетов тестов.
 */
ST_EXT void pack_read( void );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* ST_H */
