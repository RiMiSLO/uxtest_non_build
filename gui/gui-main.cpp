/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-main.cpp
 *  \brief  Главная форма приложения.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include <QSettings>
#include <QProcess>
#include "gui-main.h"
#include "message.h"
#include "snmp.h"


#define DESKTOP_WIDTH                    1920
#define DESKTOP_HEIGHT                   1080

#define MARGIN                           8
#define H_BOTTOM                         ( 186 + MARGIN * 2 )
#define W_TOP_RIGHT                      ( DESKTOP_WIDTH / 3 )
#define W_BTN_BOTTOM                     180
#define H_TITLE                          24
#define H_TAB_ROW                        24
#define H_TAB_ROW_BL                     20
#define W_TAB_VALUE                      125
#define H_MAP_LABEL                      18

#define X_MAP                            0
#define W_MAP                            ( DESKTOP_WIDTH - W_TOP_RIGHT - MARGIN - 2 )
#define Y_MAP                            0
#define H_MAP                            ( DESKTOP_HEIGHT - 70 ) /* - H_BOTTOM */

#define PAGE_BOTTOM_COUNT                4
#define PAGE_TEST                        0
#define PAGE_CTRL                        1
#define PAGE_PREP                        2
#define PAGE_SUMMARY                     3

#define M_STOP                           "[X]"

#ifdef TITLE_RU
#define M_DLG_NAME                       "Идентификация"
#define M_DLG_PROG                       "Обновление ПО"
#define M_DLG_INIT                       "Инициализация"
#define M_DLG_REBOOT                     "Перезагрузка ВУ"
#define M_DLG_JOURNAL_CLEAR              "Очистка журналов"
#define M_DLG_ERROR                      "ОШИБКА"
#define M_DLG_MESSAGE                    "Сообщение"
#define M_DLG_EXIT                       "Выход"

#else
#define M_DLG_NAME                       "SET NAME"
#define M_DLG_PROG                       "PROG"
#define M_DLG_INIT                       "INIT"
#define M_DLG_REBOOT                     "REBOOT"
#define M_DLG_JOURNAL_CLEAR              "CLEAR JOURNALS"
#define M_DLG_ERROR                      "ERROR"
#define M_DLG_MESSAGE                    "MESSAGE"
#define M_DLG_EXIT                       "EXIT"
#endif

#define M_BTN_TEST                       "Тестирование"
#define M_BTN_CONTROL                    "Реализация ЗУ"
#define M_BTN_PREPARE                    "Подготовка"
#define M_BTN_SUMMARY                    "Сообщения"
#define M_BTN_EXIT                       "Выход" M_STOP

#define M_TEST_HDD_E                     "Диск"
#define M_TEST_HDD                       "Диск"
#define M_TEST_RAM_E                     "ОЗУ"
#define M_TEST_RAM                       "ОЗУ"
#define M_TEST_VIDEO_E                   "Видео"
#define M_TEST_VIDEO                     "Видео"
#define M_TEST_CMOS                      "Таймеры"
#define M_TEST_PIC                       "Прерывания"
#define M_TEST_NET                       "Сеть"
#define M_TEST_KEYB                      "Клавиатура"
#define M_TEST_MOUSE                     "Мышь"
#define M_TEST_SPEAKER                   "Динамик"
#define M_TEST_MCP_1                     "Тест МЦП"
#define M_TEST_MI_2                      "Тест МИ"
#define M_TEST_UPS                       "ИБП"
#define M_TEST_FULL                      "Полное тест."
#define M_TEST_RTO                       "RTO"

#define M_REP_FOREVER                    "нонстоп"

#define M_CTRL_TIME                      "Время"
#define M_CTRL_START                     "Запуск РЗУ"
#define M_CTRL_START_1                   "Проверка блоков"
#define M_CTRL_REPAIR                    "Продолжение"
#define M_CTRL_PROT                      "Протокол"

#define M_PREP_TD_LOAD                   "Загрузка ТД"
#define M_PREP_TD_CHECK                  "Проверка ТД"
#define M_PREP_ST_NAME                   "Идентификация ВУ"
#define M_PREP_ST_REBOOT                 "Сброс ВУ"
#define M_PREP_TST_CLEAR                 "Очистка журналов"
#define M_PREP_MI_ID                     "Загрузка ИД МИ/МЦП"
#define M_PREP_MI_GRAD                   "Загрузка град МИ/МЦП"
#define M_PREP_MI_REPLACE                "Замена МИ/mcp"
#define M_PREP_BL_ADDR                   "Адрес блока"
#define M_PREP_BL_PROG                   "Загрузка ПО блока"
#define M_PREP_BL_INIT                   "Инициал. блока"
#define M_PREP_BL_RESERVE                "Резерв <-> ЗИП"

#define M_EXIT_OK                         "Завершить"
#define M_EXIT_MINIMIZE                   "Свернуть"
#define M_EXIT_CANCEL                     "Отмена"

#define M_NO_RZU                         "<font color=red>РЗУ не выбран.</font>"
#define M_PERFORM_TEST                   "ятест..."

#define M_UPS_STATE                      "ИБП"
#define M_STATION_STATE                  "ВУ"
#define M_METER_STATE                    "МИ"
#define M_MCP_STATE                      "MЦП"
#define M_NET                            "Сеть"

#define M_CLEAR_ALL                      "Очистить все"
#define M_CHECK_ALL                      "Отметить все"

#define M_NET_STAT_1                     "Потерь связи (отсоединений кабеля)"
#define M_NET_STAT_2                     "Потерь несущей"
#define M_NET_STAT_3                     "Принятых пакетов (по аппарат.сч.)"
#define M_NET_STAT_4                     "Принятых пакетов (по драйв.)"
#define M_NET_STAT_5                     "Отключений (запретов) приема"
#define M_NET_STAT_6                     "Ошибок приема (по аппарат.сч.)"
#define M_NET_STAT_7                     "Ошиб. принятых пакетов (по драйв.)"
#define M_NET_STAT_8                     "Приемов слишком коротких пакетов"
#define M_NET_STAT_9                     "Приемов слишком длинных пакетов"
#define M_NET_STAT_10                    "Ошибок CRC при приеме"
#define M_NET_STAT_11                    "Ошибок выравнивания пакета"
#define M_NET_STAT_12                    "Непринятых пакетов - переполн. FIFO"
#define M_NET_STAT_13                    "Ошибок отсутствия места в буфере"
#define M_NET_STAT_14                    "Передан. пакетов (по аппарат.сч.)"
#define M_NET_STAT_15                    "Передан. пакетов (по драйв.)"
#define M_NET_STAT_16                    "Внесен. в буфер пакетов (драйв.)"
#define M_NET_STAT_17                    "Отключений (запретов) передачи"
#define M_NET_STAT_18                    "Потерь несущей при передаче"
#define M_NET_STAT_19                    "Прерываний передачи"
#define M_NET_STAT_20                    "Ош. передачи (превыш. числа коллиз.)"
#define M_NET_STAT_21                    "Ош. передачи (нехватка данных DMA)"
#define M_NET_STAT_22                    "Ошибок из-за длинной передачи"
#define M_NET_STAT_23                    "Ош. передачи из-за занятости буфера"

#define M_TD_ERROR                       "Образ технологического диска отсутствует или испорчен.%1"
#define M_RZU_NOT_FOUND                  "Не найден каталог РЗУ: %1%2"
#define M_MI_FILE_ERROR                  "Файл коррект. кодов МИ имеет неверный формат или отсутствует: %1"
#define M_DLG_NAME_MSG                   "Задать имя ВУ?\nНовое имя: %1\nСтарое имя: %2"
#define M_DLG_NAME_NO                    "(нет)"
#define M_DLG_ADDR_MSG                   "Задать адрес блока?\nНовый адрес: %1\nСтарый адрес: %2"
#define M_DLG_PROG_MSG                   "Записать ПО в блок %1?"
#define M_DLG_PROG_ALL_MSG               "Записать ПО во все блоки УПО?"
#define M_DLG_INIT_MSG                   "Инициализировать блок %1?"
#define M_DLG_REPLACE_MSG                "Подтвердить установку модуля %1 с заводским номером %2?"
#define M_DLG_REBOOT_MSG                 "Перезагрузить отмеченные ВУ?"
#define M_DLG_JOURNAL_CLEAR_MSG          "Очистить журналы тестирования и отладки?"
#define M_DLG_EXIT_MSG                   "Завершить работу программы?"
#define M_DLG_EXIT_MINIMIZE_MSG          "Нажмите \"Свернуть\" чтобы минимизировать окно не завершая работу программы."
#define M_BL_PROG_OK                     "Передача ПО для блока УПО завершена."
#define M_BL_PROG_ALL_END                "Запись ПО блоков УПО завершена."
#define M_BL_PROG_NOT                    "Не завершена предыдущая операция!"
#define M_BL_PROG_ERROR                  "Ошибка передачи ПО для блока УПО (см. журнал тестирования)."

#define M_SET_NAME_ST                    "Идентификация ВУ: %s -> %s."
#define M_SET_ADDR_BL                    "Адрес блока: %d -> %d."
#define M_INIT_BLOCK                     "Инициализация блока %s."

#define F_PIC_EXPRESS                    ":/pic/express.png"
#define F_PIC_NOTFOUND                   ":/pic/_not_found.png"

#define STYLE_SUMMARY                    "QListWidget { border: 0px; }"
#define STYLE_TITLE_GRAY                 "QLabel { background-color: #e3dfde; }"
#define STYLE_TITLE_GREEN                "QLabel { background-color: #9acd32; }"
#define STYLE_TITLE_RED                  "QLabel { background-color: #fcb091; }"
#define STYLE_TITLE_YELLOW               "QLabel { background-color: #ffff00; }"
#define STYLE_TABBAR                     "QTabBar::tab { min-width: 120px; }"
#define STYLE_TABLE_DATA                 "QTableWidget { border: 0px; }"
#define STYLE_JOURNAL                    "QListWidget { border: 1px solid #bab9b9; margin-bottom: -1px; }"
#define STYLE_MAP                        "QGroupBox { border: 0px; margin-top: -1px; background-color: #f7f5f5; }"
#define STYLE_FONT_DARK                  "color: #474747;"

static QColor colTableBlack = QColor( COLOR_DEFAULT );

#define BLOCK_PROG_WAIT	    0x8000
#define BLOCK_PROG_TIMEOUT  ( 4 * 60 * 1000 )
#define BLOCK_PROG_PERIOD   ( 2 * 1000 )

/* Кнопки тестирования */

#define PACK_FIRST    ( TEST_COUNT + TEST_MI_COUNT + 2 )
#define PACK_BASE     0x200

static const int ID_TEST[] = {

    TEST_HDD_EXP,
	TEST_HDD,
	TEST_RAM_EXP,
	TEST_RAM,
	TEST_VIDEO_EXP,
	TEST_VIDEO,
    TEST_CMOS,
	TEST_PIC,
	TEST_ETHERNET,
	TEST_KBD,
	TEST_MOUSE,
    TEST_SPEAKER,
    TEST_UPS,
    TEST_MCP_1,
    TEST_MI_2,
    TEST_FULL,
    PACK_BASE + PACK_1,
    PACK_BASE + PACK_2,
    PACK_BASE + PACK_3,
};

#define TEST_BUTTON_COUNT  ( array_length( ID_TEST ))

static const char *M_TEST[] = {
    M_TEST_HDD_E,
	M_TEST_HDD,
	M_TEST_RAM_E,
	M_TEST_RAM,
	M_TEST_VIDEO_E,
	M_TEST_VIDEO,
	M_TEST_CMOS,
	M_TEST_PIC,
	M_TEST_NET,
	M_TEST_KEYB,
	M_TEST_MOUSE,
    M_TEST_SPEAKER,
    M_TEST_UPS,
    M_TEST_MCP_1,
    M_TEST_MI_2,
    M_TEST_FULL,
    "",
    "",
    "",
};

static const char *M_TEST_STOP[] = {
    M_TEST_HDD_E M_STOP,
	M_TEST_HDD M_STOP,
	M_TEST_RAM_E M_STOP,
	M_TEST_RAM M_STOP,
	M_TEST_VIDEO_E M_STOP,
	M_TEST_VIDEO M_STOP,
	M_TEST_CMOS M_STOP,
	M_TEST_PIC M_STOP,
	M_TEST_NET M_STOP,
	M_TEST_KEYB M_STOP,
	M_TEST_MOUSE M_STOP,
    M_TEST_SPEAKER M_STOP,
    M_TEST_UPS M_STOP,
    M_TEST_MCP_1 M_STOP,
    M_TEST_MI_2 M_STOP,
    M_TEST_FULL M_STOP,
    "",
    "",
    "",
};

static const bool BR_TEST[] = {
    false,
    false,
	false,
	true,
	false,
    false,
	false,
	true,
	false,
	false,
	false,
    true,
    false,
    false,
    false,
    true,
    true,
    false,
    false
};

static bool const EXPRESS_TEST[] = {
    true,
    false,
    true,
    false,
	true,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
    false,
    false,
    false,
    false,
    false,
    false,
    true
};

/* Кнопки реализации */

enum {
	CTRL_TIME,
	CTRL_START,
	CTRL_REPAIR,
	CTRL_PROT,
};

static const int ID_CTRL[] = {  /* inx == id */
	CTRL_TIME,
	CTRL_START,
	CTRL_REPAIR,
	CTRL_PROT,
};

#define CTRL_BUTTON_COUNT  array_length( ID_CTRL )

static const char * const M_CTRL[] = {
	M_CTRL_TIME,
	M_CTRL_START,
	M_CTRL_REPAIR,
	M_CTRL_PROT,
};

static const bool BR_CTRL[] = {
	true,
	true,
	true,
	true,
};

/* Кнопки подготовки */

enum {
    PREP_TD_LOAD,
    PREP_TD_CHECK,
    PREP_TST_CLEAR,
    PREP_ST_NAME,
    PREP_ST_REBOOT,
    PREP_MI_ID,
    PREP_MI_GRAD,
    PREP_BL_ADDR,
    PREP_BL_PROG,

    PREP_MI_REPLACE,
    PREP_BL_INIT,
    PREP_BL_RESERVE,
};

static const int ID_PREP[] = {  /* inx == id */
    PREP_TD_LOAD,
    PREP_TD_CHECK,
    PREP_TST_CLEAR,
    PREP_ST_NAME,
    PREP_ST_REBOOT,
    PREP_MI_ID,
    PREP_MI_GRAD,
    PREP_BL_ADDR,
    PREP_BL_PROG,
};

#define PREP_BUTTON_COUNT  ( array_length( ID_PREP ))

static const char * const M_PREP[] = {
    M_PREP_TD_LOAD,
    M_PREP_TD_CHECK,
    M_PREP_TST_CLEAR,
    M_PREP_ST_NAME,
    M_PREP_ST_REBOOT,
    M_PREP_MI_ID,
    M_PREP_MI_GRAD,
    M_PREP_BL_ADDR,
    M_PREP_BL_PROG,
};

static const bool BR_PREP[] = {
    false,
    false,
    true,
    false,
    false,
    true,
    false,
    false,
    true,
};

static const char * const M_NET_STAT[] = {
	M_NET_STAT_1,
	M_NET_STAT_2,
	M_NET_STAT_3,
	M_NET_STAT_4,
	M_NET_STAT_5,
	M_NET_STAT_6,
	M_NET_STAT_7,
	M_NET_STAT_8,
	M_NET_STAT_9,
	M_NET_STAT_10,
	M_NET_STAT_11,
	M_NET_STAT_12,
	M_NET_STAT_13,
	M_NET_STAT_14,
	M_NET_STAT_15,
	M_NET_STAT_16,
	M_NET_STAT_17,
	M_NET_STAT_18,
	M_NET_STAT_19,
	M_NET_STAT_20,
	M_NET_STAT_21,
	M_NET_STAT_22,
	M_NET_STAT_23,
};

const char * const Connect::pic[] = {
	":/pic/connect_o.tga",
	":/pic/connect.tga",
	":/pic/connect_d.tga",
	":/pic/connect_e.tga",
};

int  MapCell::shift = 0;
int  MapCell::new_line = 0;
bool MapCell::new_column = false;
bool MapCell::first_column = true;

static void setLabelPixmap( QLabel *l, QString fn );
static void setButtonPixmap( QAbstractButton *b, QString fn );
static void setButtonIco( QAbstractButton *b, QString fn );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

BLabel::BLabel( QPushButton *b, QWidget *parent ) :
	QLabel( parent ),
	button( b )
{
}

void BLabel::mousePressEvent( QMouseEvent *event )
{
	( void ) event;

	if ( button ) {
		button->setFocus();
		button->click();
	}
}

Connect::Connect( QWidget* parent ):
	QLabel( parent ),
	state( LINK_NONE )
{
}

void Connect::setup( int n, MapCell *cell )
{
    setLabelPixmap( this, pic[ state ]);

    if (cell->kind == MAP_ST){
       move(cell->x() + ((width() + 2) * n), cell->y() - cell->pic_shift - height());
    }else if (cell->kind == MAP_MCP){
       move(cell->x() + 210 + ((width() + 2) * n), cell->y() + 6);
    }else{
       move( cell->x() + map[ cell->map_index ].x + (( width() + 2 ) * n ), cell->y() - cell->pic_shift );
    }
}

void Connect::setState( int value )
{
    if ( state == value ) return;

    state = value;
    setLabelPixmap( this, pic[ state ]);
}

//--------------------------------------------------------------------------------------------

MapCell::MapCell( MainWindow* p, uint8_t pg, uint8_t inx ):
	QPushButton( p->grMap[ pg ]),
	main( p ),
	page( pg ),
	map_index( inx ),
	kind( map[ inx ].kind ),
	state( OFFLINE ),
	fail( false )
{
	QGroupBox *gb = p->grMap[ page ];

    selected = false;

	setAutoDefault( true );
	setFont( main->fontSmall );
	setStyleSheet( STYLE_FONT_DARK );
	pic_shift = 0;
	cell_height = H_MAP_LABEL * 2;
	caption = new BLabel( this, gb );
	caption->setFont( main->fontCell );
	caption->setWordWrap( true );
	caption->setAlignment( Qt::AlignCenter );

    type = map[ inx ].type;

	switch ( kind ) {

	/* ВУ, МИ */
	case MAP_ST:

        if ((map[ inx ].type == 1)||(map[ inx ].type == 3)||(map[ inx ].type == 7)){
            addBtn = new QPushButton(gb);
        }

        for ( int i = 0; i < NET_COUNT; i++ ) connector[ i ] = new Connect( gb );
        checkbox = new QCheckBox( gb );
        checkbox->setFont( main->fontCell );
        checkbox->setStyleSheet( STYLE_FONT_DARK );
        connect( checkbox, SIGNAL( stateChanged( int )), this, SLOT( onCheckChanged( int )));
        pic_shift = H_MAP_LABEL;
        break;

    /* МИ */
    case MAP_MI: // тут нужно будет менять сети тоже
        /*
        connector[ 0 ] = new Connect( gb ); // Одна сеть!
        for ( int i = 1; i < NET_COUNT; i++ ) connector[ i ] = NULL;
        checkbox = new QCheckBox( gb );
        checkbox->setFont( main->fontCell );
        checkbox->setStyleSheet( STYLE_FONT_DARK );
        connect( checkbox, SIGNAL( stateChanged( int )), this, SLOT( onCheckChanged( int )));
        */
//        for ( int i = 0; i < NET_COUNT; i++ ) connector[ i ] = NULL;
//        checkbox = NULL;
//        pic_shift = H_MAP_LABEL;
//        caption->setStyleSheet( STYLE_FONT_DARK );

        connector[ 0 ] = new Connect( gb ); // Одна сеть!
        //for ( int i = 1; i < NET_COUNT; i++ ) connector[ i ] = NULL;
        checkbox = new QCheckBox( gb );
        checkbox->setFont( main->fontCell );
        checkbox->setStyleSheet( STYLE_FONT_DARK );
        connect( checkbox, SIGNAL( stateChanged( int )), this, SLOT( onCheckChanged( int )));
        pic_shift = H_MAP_LABEL;
        break;

     /* МЦП */
     case MAP_MCP: // тут нужно будет менять сети тоже
        //connector[ 0 ] = new Connect( gb );
        for ( int i = 0; i < NET_COUNT; i++ ) connector[ i ] = new Connect( gb );
        checkbox = new QCheckBox( gb );
        checkbox->setFont( main->fontCell );
        checkbox->setStyleSheet( STYLE_FONT_DARK );
        connect( checkbox, SIGNAL( stateChanged( int )), this, SLOT( onCheckChanged( int )));
        pic_shift = H_MAP_LABEL;
        break;
    /* Блок */
	case MAP_BL:
        caption->setStyleSheet( STYLE_FONT_DARK );
		for ( int i = 0; i < NET_COUNT; i++ ) connector[ i ] = NULL;
        checkbox = NULL;
		break;
	/* Статическое изображение */
	case MAP_IMAGE:
		caption->setStyleSheet( STYLE_FONT_DARK );
		for ( int i = 0; i < NET_COUNT; i++ ) connector[ i ] = NULL;
		checkbox = NULL;
		state = IDLE;
		setFocusPolicy( Qt::NoFocus );
		cell_height = 0;
		break;
    case MAP_UPS:

        //connector[ 0 ] = new Connect( gb ); /* Одна сеть! */
        /*for ( int i = 1; i < NET_COUNT; i++ ) connector[ i ] = NULL;*/
        /*
        checkbox = new QCheckBox( gb );
        checkbox->setFont( main->fontCell );
        checkbox->setStyleSheet( STYLE_FONT_DARK );
        connect( checkbox, SIGNAL( stateChanged( int )), this, SLOT( onCheckChanged( int )));
        pic_shift = H_MAP_LABEL;
        checkbox->setVisible(false);
        */
        checkbox = NULL;
        break;

	default:
		/* Не отображать */
		kind = MAP_NONE;
		checkbox = NULL;
		setVisible( false );
		return;
	}
	setCell();
	setEnabled( false );
	cell_height += height();
}

void MapCell::setCell()
{
    int p = 0;
    int pMonitor = 0;

    if ( kind == MAP_NONE ) return;

    if ((kind == MAP_ST)&&(type == 3)){
        // ПКФ всегда зеленый

        p = map[ map_index ].pic;
        pMonitor = 1;

    }else{
        // Не ПКФ - в зависимости от состояния

        switch ( state ) {

        case OFFLINE:
            p = map[ map_index ].pico;
            pMonitor = 0;
            break;

        case IDLE:
            p = ( fail ) ? map[ map_index ].pice : map[ map_index ].pic;
            pMonitor = ( fail ) ? 2 : 1;

            break;

        case OPERATE:
            p = (fail) ? map[ map_index ].pice:map[ map_index ].picw;   // тут
            //p = map[map_index].picw;
            pMonitor = 3;
            break;

        case UPSERRORSMALL:
            p = map[ map_index].picp;
            break;

        default:
            p = map[ map_index ].pice;
            pMonitor = 2;
        }
    }

    setButtonPixmap( this, ( p ) ? map_pic[ p - 1 ] : QString( F_PIC_NOTFOUND ));

    if (this->addBtn && (kind == MAP_ST)&&((type == 1)||(type == 3)||(type == 7))){
        // Монитор показывается для МРМО и ПКФ


        setButtonPixmap(this->addBtn, map_pic[pMonitor]);//MAP_PIC_INDEX_MONITOR]);
    }


    if (selected){
        // Выбранный элемент
        if (kind == MAP_BL)
            this->caption->setStyleSheet("color: #3300ff; font-weight: bold;");
        else if ((kind == MAP_ST)||(kind == MAP_MI))
            if ( checkbox )
            this->checkbox->setStyleSheet("color: #3300ff; font-weight: bold;");
//            this->caption->setStyleSheet("color: #000000; background: #dddddd; font-weight: bold;");
    }else{
        // Невыбранный элемент
        if (kind == MAP_BL)
            this->caption->setStyleSheet("color: #444444; font-weight: normal;");
        else if ((kind == MAP_ST)||(kind == MAP_MI))
            if ( checkbox )
                this->checkbox->setStyleSheet("color: #000000; font-weight: normal;");

            //this->caption->setStyleSheet("color: #444444; background: #eeeeee; font-weight: normal;");
    }

}

void MapCell::setupCell( int16_t br, QString title )
{
	int i;
	int x, y;

	if ( kind == MAP_NONE ) return;

    if (map[map_index].position == 0){
        // Относительное позиционирование

        /* Find previus item */
        for ( i = map_index - 1; i >= 0; i-- ) {
            if (main->cell[ i ]->page != page) {
                i = -1;
                break;
            }
            if ((main->cell[ i ] != MAP_NONE)&&(map[i].position == 0)) {
                break;
            }
        }
        /* Calc position */
        if ( i >= 0 ) {
            MapCell *c = main->cell[ i ]; /* Get previous cell */
            x = c->x() + c->width();

            if (( x + width()) > ( X_MAP + W_MAP ) || new_line ) {
                /* End of line */
                if ( new_line > 0 ) {
                    y = c->y() - c->pic_shift + c->cell_height + MARGIN;
                } else {
                    y = c->y() - c->pic_shift - new_line;
                }

                if (( y + cell_height + MARGIN ) > ( Y_MAP + H_MAP ) || new_column ) {
                    /* End of column */
                    /* int t = ( width() + 2 ) * 3 + MARGIN;
                     */
                    shift = W_MAP / 2 + MARGIN / 2;
                    /* if ( shift < t ) shift = t;
                     */
                    y = Y_MAP + MARGIN;
                    if ( new_column )
                        y += ( cell_height + MARGIN ) * ini_map_indent_2;

                    first_column = false;
                    new_column = false;
                }
                x = X_MAP + shift;

                new_line = 0;
            } else {
                /* Same line */
                y = c->y();
                if (( c->height() + c->pic_shift ) != ( height() + pic_shift )) {
                    /* Align by picture bottom */
                    y += c->height() - height() - pic_shift;
                } else {
                    y -= c->pic_shift;
                }
            }
        } else {
            /* First cell */
            shift = MARGIN;
            x = X_MAP + shift;
            y = Y_MAP + MARGIN + ( cell_height + MARGIN ) * ini_map_indent_1;
        }
    }else{
        // Абсолютное позиционирование

        x = map[map_index].x;
        y = map[map_index].y;

//printf("CELL(%d): %d / %d \n", map_index, map[map_index].x, map[map_index].y);

        pic_shift = 0;
    }

	/* Setup cell */
//if (kind == MAP_IMAGE)
//x = x - 4;

	move( x, y + pic_shift );

	switch ( kind ) {

	case MAP_ST:
        //  checkbox->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 2, width() - map[ map_index ].x, H_MAP_LABEL );
        if (map[map_index].type == 0){
            // СКУ
            checkbox->setGeometry( x, y + pic_shift + height() + 2, width(), H_MAP_LABEL );
        }else if ((map[map_index].type == 1)||(map[map_index].type == 3)||(map[map_index].type == 7)){
            // РМО

            if (addBtn)
            addBtn->move(x - 10, y - 96);
            checkbox->setGeometry( x, y + pic_shift + height() + 15, width(), H_MAP_LABEL );
        }else{

 //   addBtn->move(x, y - 100);

            checkbox->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 2, width() - map[ map_index ].x, H_MAP_LABEL );
        }
        checkbox->setText( title );
        checkbox->adjustSize();
        checkbox->setChecked( false );

        for ( i = 0; i < ini_net_count; i++ ) connector[ i ]->setup( i, this );

        caption->setGeometry( x + map[ map_index ].x, y + pic_shift + ( height() - H_MAP_LABEL ) / 2
          , width() - map[ map_index ].x, H_MAP_LABEL );
        checkbox->setChecked( false );
        break;


    case MAP_MCP:

        checkbox->setGeometry(x + 10, y + 4, 60, H_MAP_LABEL);
        checkbox->setText(title);
        checkbox->adjustSize();
        checkbox->setChecked( false );

        for ( i = 0; i < ini_net_count; i++ ) connector[ i ]->setup( i, this );
        caption->setGeometry( x + 100, y + 4, 100, H_MAP_LABEL );
        checkbox->setChecked( false );

        break;


    case MAP_MI: // тут нужно будет менять сети тоже
      //  checkbox->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 2, width() - map[ map_index ].x, H_MAP_LABEL );

        checkbox->setGeometry( x+10, y-60 + pic_shift + height() + 2, width(), H_MAP_LABEL );
//        checkbox->setText( title );
        checkbox->adjustSize();
        checkbox->setChecked( false );
        connector[ 0 ]->setup( 0, this );
//        checkbox->setChecked( false );

   //     caption->setGeometry( x + map[ map_index ].x, y + pic_shift + ( height() - H_MAP_LABEL ) / 2 + 20
   //     , width() - map[ map_index ].x, H_MAP_LABEL );

        caption->setGeometry(x, y + height() + 1,
          width(), H_MAP_LABEL * 2);
        caption->setText( title );
        caption->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
        break;

    case MAP_UPS:
        /*
        checkbox->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 2
        , width() - map[ map_index ].x, H_MAP_LABEL );
        checkbox->setText( title );
        checkbox->adjustSize();
        checkbox->setChecked( false );
        connector[ 0 ]->setup( 0, this );
        connector[ 0 ]->setVisible(false);
        */
        caption->setGeometry( x + 20, y + 8, width(), H_MAP_LABEL );
        caption->setText( title );
        caption->setAlignment( Qt::AlignTop | Qt::AlignLeft );
        break;

	case MAP_BL:
    //	caption->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 4, ini_bl_label_width, H_MAP_LABEL * 2 );
        caption->setGeometry(x, y + pic_shift + height() + 4, ini_bl_label_width, H_MAP_LABEL * 2 );
		caption->setText( title );
		caption->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
		break;

	case MAP_IMAGE:
		if ( title.isEmpty()) {
			caption->setVisible( false );
			break;
		}

        if (map[map_index].position == 0){
            caption->setGeometry( x + map[ map_index ].x, y + pic_shift + height() + 4, width(), H_MAP_LABEL * 2 );
        }else{
            // caption->setGeometry(map[ map_index ].x, map[ map_index ].y + height() + 4, width(), H_MAP_LABEL * 2 );
            if ((map[ map_index ].label_x != 0)||(map[ map_index ].label_y != 0)){
                caption->setGeometry(map[ map_index ].label_x, map[ map_index ].label_y, width(), H_MAP_LABEL * 2 );
                caption->setAlignment( Qt::AlignTop | Qt::AlignLeft );
            }else{
                caption->setGeometry(map[ map_index ].x, map[ map_index ].y - 20, width(), H_MAP_LABEL * 2 );
                caption->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
            }
        }
		caption->setText( title );

		break;
	}


    if (map[map_index].position == 0){
        // Относительное позиционирование

        if ( br == BR_PAGE ) {
            new_line = 0;
            first_column = true;
            new_column = false;
        } else {
            new_line = br;
            if ( first_column && ( br == BR_COLUMN )) new_column = true;
        }
    }

//printf("CELL end \n");
}

void MapCell::setState( int value )
{
	switch ( kind ) {
	case MAP_ST:
		switch ( value ) {
		case TURN_OFF:
            value = OFFLINE;
			break;

		case TURN_ON:
		case RESTORE_WORK:
		case TEST_STATE:
		case PKF:
			value = IDLE;
			break;

		case WORK:
			value = OPERATE;
			break;
		}
		break;

	case MAP_MI:
		switch ( value ) {
		case MI_OFF:
			value = OFFLINE;
			break;

		case MI_ON:
		case MI_ON_RESET:
			value = IDLE;
			break;

		case MI_WORK:
		case MI_WORK_RESET:
			value = OPERATE;
            break;
		}
		break;

    case MAP_MCP:
        switch( value ){
        case MCP_OFF:
            value = OFFLINE;
            break;
        case MCP_ON:
        case MCP_ON_RESET:
            value = IDLE;
            break;
        case MCP_WORK:
        case MCP_WORK_RESET:
             value = OPERATE;
             break;
        }
        break;

	case MAP_BL:
		switch ( value ) {
		case REGWORK_VU_OFF:
			value = OFFLINE;
			break;

		case REGWORK_VU_CHECK:
		case REGWORK_VU_ON:
			value = IDLE;
			break;

		case REGWORK_VU_WORK:
			value = IDLE; /* OPERATE - нет картинок, нет отображения неисправности */
            break;
		}
		break;

	case MAP_IMAGE:
		value = IDLE;
		break;

    case MAP_UPS:
        switch ( value ) {
        case TURN_OFF:
            value = OFFLINE;
            break;

        case TURN_ON:
        case RESTORE_WORK:
        case TEST_STATE:
        case PKF:
            value = IDLE;
            break;
        case UPSYELLOW:
            value = UPSERRORSMALL;
            break;
        case WORK:
            value = OPERATE;
            break;

        }
        break;
	}
	if ( state == value ) return;

//printf("CELL state %d %d (%d)\n", value, map_index, kind);

	state = value;
	setCell();
}

void MapCell::setSelected( bool value )
{
    selected = value;
    setCell();
}


void MapCell::setText( const char *s )
{
	if ( caption ) caption->setText( s );
}

void MapCell::setFail( bool value )
{
	if ( fail == value ) return;

	fail = value;
	setCell();
	main->updatePageTitleColor( page );

//printf("cell fail %d (%d)\n", map_index, value);
}

bool MapCell::getFail( void )
{
	switch ( kind ) {
	case MAP_ST:
	case MAP_BL:
	case MAP_MI:
    case MAP_MCP:
    case MAP_UPS:
		return ( fail );
	}
	return ( false );
}

void MapCell::setEnabled( bool value )
{
/*	QPushButton::setEnabled( value );
 */
    if ( checkbox ) {
		checkbox->setChecked( value );
		checkbox->setEnabled( value );
	}
/*	if ( caption ) caption->setEnabled( value );
 */
}

void MapCell::onCheckChanged( int state )
{
	switch ( kind ) {
	case MAP_ST:
		main->setMask( map[ map_index ].ind, state );
		return;

	case MAP_MI:
		main->setMask2( map[ map_index ].ind, state );
		return;

    case MAP_MCP:
        main->setMask3( map[ map_index ].ind, state );
        break;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void setLabelPixmap( QLabel *l, QString fn )
{
	QPixmap p;

	if ( !QPixmapCache::find( fn, &p )) {
		p.load( fn );
		QPixmapCache::insert( fn, p );
	}
	l->setUpdatesEnabled( false );
	l->setPixmap( p );
	l->adjustSize();
	l->setUpdatesEnabled( true );
	l->repaint();
}

static void setButtonPixmap( QAbstractButton *b, QString fn )
{
	QPixmap p;

	if ( !QPixmapCache::find( fn, &p )) {
		p.load( fn );
		QPixmapCache::insert( fn, p );
	}
	b->setUpdatesEnabled( false );
	b->setIcon( QIcon( p ));
	b->setIconSize( p.rect().size());
	b->setFixedSize( p.rect().size());
	b->setUpdatesEnabled( true );
	b->repaint();
}

static void setButtonIco( QAbstractButton *b, QString fn )
{
	QPixmap p;

	if ( !QPixmapCache::find( fn, &p )) {
		p.load( fn );
		QPixmapCache::insert( fn, p );
	}
	b->setUpdatesEnabled( false );
	b->setIcon( QIcon( p ));
	b->setUpdatesEnabled( true );
	b->repaint();
}

static REPORT_MESSAGE *__queue_front( void )
{
	REPORT_MESSAGE *mg = NULL;

	pthread_mutex_lock( &report_mutex );

	if ( !report_queue.empty()) {
		mg = &report_queue.front();
	}
	pthread_mutex_unlock( &report_mutex );

   return ( mg );
}

static void __queue_pop( void )
{
	pthread_mutex_lock( &report_mutex );

	report_queue.pop();

	pthread_mutex_unlock( &report_mutex );
}

unsigned MainWindow::getTestButtonInx( int id )
{
    for ( unsigned i = 0; i < TEST_BUTTON_COUNT; i++ ) {
		if ( ID_TEST[ i ] == id ) return ( i );
    }
    return ( TEST_BUTTON_COUNT );
}

void MainWindow::setTestButtonRunning( int id, bool running )
{
	QAbstractButton *b = grTestButton->button( id );
	if ( !b ) return;

	unsigned inx = getTestButtonInx( id );
    if ( inx == TEST_BUTTON_COUNT ) return;

	b->setUpdatesEnabled( false );
	b->setStyleSheet(( running ) ? "color: #695c7d;" : "color: black;" );
	b->setText(( running ) ? M_TEST_STOP[ inx ] : M_TEST[ inx ]);
	b->setUpdatesEnabled( true );
	b->repaint();
}

void MainWindow::setBottomPage( unsigned int page )
{
	emit grBottomButton->button( page )->click();
}

void MainWindow::selectMapFirst( void )
{
	for ( int i = 0; i < map_count; i++ ) {
		switch ( map[ i ].kind ) {
		case MAP_ST:
        case MAP_MCP:
		case MAP_BL:
			selected = i + 1;
			onMapClicked( i );
			return;
		}
	}
	selected = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

QButtonGroup *MainWindow::setupButtonGroup( QGridLayout *lo, const int id[], const char * const tl[]
, const bool br[], unsigned count )
{
	int max_col = 0, col = 0, row = 0;
	unsigned i;
	QWidget* prop;
	QButtonGroup* gr = new QButtonGroup( this );

	for ( i = 0; i < count; i++ ) {
		col = ( br[ i ]) ? 1 : col + 1;
		if ( col > max_col ) max_col = col;
	}
	col = 0;
	row = 0;
	for ( i = 0; i < count; i++ ) {
		QPushButton *btn = new QPushButton( QString( tl[ i ]), this );
		btn->setMinimumWidth( W_BTN_BOTTOM );
		gr->addButton( btn, id[ i ]);
		lo->addWidget( btn, row, col++ );
		if ( br[ i ]) {
			if ( col == max_col ) {
				prop = new QWidget( this );
				prop->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
				lo->addWidget( prop, row, col );
			}
			col = 0;
			++row;
		}
	}
	prop = new QWidget( this );
	prop->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
	gr->setExclusive( true );
	lo->addWidget( prop, row, 0 );

	return ( gr );
}

void MainWindow::setupBottom( QHBoxLayout *loBottom )
{
	unsigned i;

    QVBoxLayout *loBottomLeft = new QVBoxLayout();
	grBottomButton = new QButtonGroup( this );
	QPushButton *btnTest = new QPushButton( M_BTN_TEST, this );
	QPushButton *btnControl = new QPushButton( M_BTN_CONTROL, this );
	QPushButton *btnPrepare = new QPushButton( M_BTN_PREPARE, this );
	QPushButton *btnSummary = new QPushButton( M_BTN_SUMMARY, this );
	QPushButton *btnExit = new QPushButton( M_BTN_EXIT, this );
	QStackedWidget *pgBottomRight = new QStackedWidget( this );
	QList< QFrame* > wgBottomRight;
	for ( i = 0; i < PAGE_BOTTOM_COUNT; i++ ) wgBottomRight.append( new QFrame( this ));
	QList< QGridLayout* > loBottomRight;
    for ( i = 0; i < PAGE_BOTTOM_COUNT; i++ ) loBottomRight.append( new QGridLayout( wgBottomRight[ i ]));
	lstSummary = new QListWidget( this );

	loBottom->setContentsMargins( MARGIN, MARGIN, MARGIN, MARGIN );

	/* Группы */
	loBottomLeft->setContentsMargins( 0, 2, MARGIN, 2 );
	loBottomLeft->setSpacing( 10 );
	btnTest->setMinimumWidth( W_BTN_BOTTOM );
	btnTest->setAutoExclusive( true ); /* Выбор групп клавишами "вверх"/"вниз" */
	btnTest->setCheckable( true );
	btnTest->setChecked( true );
	btnControl->setCheckable( true );
	btnControl->setAutoExclusive( true );
	btnPrepare->setCheckable( true );
	btnPrepare->setAutoExclusive( true );
	btnSummary->setCheckable( true );
	btnSummary->setAutoExclusive( true );
	btnExit->setStyleSheet( "color: #695c7d;" );
    pgBottomRight->setMaximumHeight( H_BOTTOM - MARGIN * 2 );

	/* Тесты */
	for ( i = 0; i < PAGE_BOTTOM_COUNT; i++ ) {
		wgBottomRight[ i ]->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
		loBottomRight[ i ]->setContentsMargins( MARGIN, MARGIN, MARGIN, MARGIN );
        loBottomRight[ i ]->setSpacing( 10 );
	}
	loBottomRight[ PAGE_SUMMARY ]->setContentsMargins( 0, 0, 0, 0 );
    grTestButton = setupButtonGroup( loBottomRight[ PAGE_TEST ], ID_TEST, M_TEST, BR_TEST, TEST_BUTTON_COUNT );
    unsigned row = 0, col = 1, max_col = 1;
    for ( i = 0; i < TEST_BUTTON_COUNT; i++ ) {
		if ( EXPRESS_TEST[ i ]) {
			setButtonIco( grTestButton->buttons()[ i ], QString( F_PIC_EXPRESS ));
		}
        if ( *M_TEST[ i ] == '\0' ) {
            grTestButton->buttons()[ i ]->setVisible( false );
        }
		if ( BR_TEST[ i ]) {
			col = 1;
			row++;
		} else {
			col++;
		}
		if ( col > max_col ) max_col = col;
	}
	connect( grTestButton, SIGNAL( buttonClicked( int )), this, SLOT( onTestClicked( int )));

	/* Повторы */
	edRepValue = new QSpinBox( this );
	edRepValue->setMinimumWidth( W_BTN_BOTTOM / 2 );
	edRepValue->setRange( 0, 255 );
	edRepValue->setSpecialValueText( M_REP_FOREVER );
	edRepValue->setValue( 1 );
	loBottomRight[ PAGE_TEST ]->addWidget( new QLabel( "Циклов", this ), row - 1, max_col + 1 );
	loBottomRight[ PAGE_TEST ]->addWidget( edRepValue, row - 1, max_col + 2 );

	/* Реализация */
	grCtrlButton = setupButtonGroup( loBottomRight[ PAGE_CTRL ], ID_CTRL, M_CTRL, BR_CTRL, CTRL_BUTTON_COUNT );
	connect( grCtrlButton, SIGNAL( buttonClicked( int )), this, SLOT( onCtrlClicked( int )));

	/* Подготовка */
	grPrepButton = setupButtonGroup( loBottomRight[ PAGE_PREP ], ID_PREP, M_PREP, BR_PREP, PREP_BUTTON_COUNT );
	connect( grPrepButton, SIGNAL( buttonClicked( int )), this, SLOT( onPrepClicked( int )));

	/* Сообщения */
	lstSummary->setStyleSheet( STYLE_SUMMARY );
	lstSummary->setSpacing( 1 );
	lstSummary->setSelectionMode( QAbstractItemView::NoSelection );
	lstSummary->setWrapping( false );
	lstSummary->setWordWrap( false );
	for ( i = 0; i < SUMMARY_COUNT; i++ ) lstSummary->addItem( "" );
	loBottomRight[ PAGE_SUMMARY ]->addWidget( lstSummary );

	for ( i = 0; i < PAGE_BOTTOM_COUNT; i++ ) wgBottomRight[ i ]->setLayout( loBottomRight[ i ]);
	for ( i = 0; i < PAGE_BOTTOM_COUNT; i++ ) pgBottomRight->addWidget( wgBottomRight[ i ]);
	grBottomButton->addButton( btnTest, PAGE_TEST );
	grBottomButton->addButton( btnControl, PAGE_CTRL );
	grBottomButton->addButton( btnPrepare, PAGE_PREP );
	grBottomButton->addButton( btnSummary, PAGE_SUMMARY );
	grBottomButton->setExclusive( true );
	connect( grBottomButton, SIGNAL( buttonClicked( int )), pgBottomRight, SLOT( setCurrentIndex( int )));
	connect( btnExit, SIGNAL( clicked()), this, SLOT( onExitClicked()));
	loBottomLeft->addWidget( btnTest );
	loBottomLeft->addWidget( btnControl );
	loBottomLeft->addWidget( btnPrepare );
	loBottomLeft->addWidget( btnSummary );
	loBottomLeft->addWidget( btnExit );
    loBottom->addLayout( loBottomLeft, 0 );
	loBottom->addWidget( pgBottomRight, 1 );

	/* Hotkeys */
	hotStopTesting = new QShortcut( QKeySequence( Qt::Key_Backspace ), this, SLOT( onStopTesting()));
	hotBlockProgAll = new QShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_B ), this, SLOT( onBlockProgAll()));

	/* Вариация формы */
    if ( ini_net_count == 1 ) {
        grCtrlButton->button( CTRL_START )->setText( M_CTRL_START_1 );
        grCtrlButton->button( CTRL_REPAIR )->setHidden( true );
    }
    if ( ini_gui_bl_init == 0 ) {
        grPrepButton->button( PREP_BL_PROG )->setHidden( true );
    }
    if ( mcp_count == 0 ) {
        grTestButton->button( TEST_MCP_1 )->setHidden( true );
        grTestButton->button( TEST_MI_2 )->setHidden( true );
        grTestButton->button( TEST_FULL )->setHidden( true );
        grPrepButton->button( PREP_MI_ID )->setHidden( true );
        grPrepButton->button( PREP_MI_GRAD )->setHidden( true );
    }
}

void MainWindow::setupTop( QHBoxLayout *loTop )
{
	unsigned i;
	char s[ 128 ];
	QTableWidgetItem *tcell;

	pgMap = new QTabWidget( this );
	pgTopRight = new QStackedWidget( this );
	QWidget *wgTopStation = new QWidget( this );
	QVBoxLayout *loTopStation = new QVBoxLayout( wgTopStation );
	QWidget *wgTopBlock = new QWidget( this );
	QVBoxLayout *loTopBlock = new QVBoxLayout( wgTopBlock );
	QWidget *wgTopMeter = new QWidget( this );
	QVBoxLayout *loTopMeter = new QVBoxLayout( wgTopMeter );
    QWidget *wgTopUps = new QWidget( this );
    QVBoxLayout *loTopUps = new QVBoxLayout( wgTopUps );
    QWidget *wgTopMCP = new QWidget(this);
    QVBoxLayout *loTopMCP = new QVBoxLayout(wgTopMCP);

	QTabWidget *pgStation = new QTabWidget( this );
	QWidget *wgStationState = new QWidget( this );
	QVBoxLayout *loStationState = new QVBoxLayout( wgStationState );
	lbStationTitle = new QLabel( this );
	tabStationDatn = new QTableWidget( this );
    for ( i = 0; i < st_count; i++ ) lstJournal.append( new QListWidget( this ));
	tabStationNet = new QTableWidget( this );

    QTabWidget *pgUps = new QTabWidget( this );
    QWidget *wgUpsState = new QWidget( this );
    QVBoxLayout *loUpsState = new QVBoxLayout( wgUpsState );

    for ( i = 0; i < ups_count; i++ ) lbUpsTitle.append( new QLabel( this ));
    for ( i = 0; i < ups_count; i++ ) tabUpsDatn.append( new QTableWidget( this ));

	QTabWidget *pgMeter = new QTabWidget( this );
	QWidget *wgMeterState = new QWidget( this );
	QVBoxLayout *loMeterState = new QVBoxLayout( wgMeterState );
	lbMeterTitle = new QLabel( this );
	tabMeterDatm = new QTableWidget( this );
    //tabMeterNet = new QTableWidget( this );


    QTabWidget *pgMCP = new QTabWidget( this );
    QWidget *wgMCPState = new QWidget( this );
    QVBoxLayout *loMCPState = new QVBoxLayout( wgMCPState );
    lbMCPTitle = new QLabel( this );
    tabMCPDatmcp = new QTableWidget( this );
    tabMCPNet = new QTableWidget( this );

	for ( i = 0; i < BL_CH_COUNT; i++ ) lbBlockTitle.append( new QLabel( this ));
	for ( i = 0; i < BL_CH_COUNT; i++ ) tabBlockDatb.append( new QTableWidget( this ));

	QPushButton *btnCheckAll = new QPushButton( pgMap );
	QPushButton *btnClearAll = new QPushButton( pgMap );
	lbRZU = new QLabel( pgMap );

	/* Уменьшенный размер шрифта */
	fontSmall = tabStationDatn->font();
	fontSmall.setPointSize( fontSmall.pointSize() - 2 );
	fontCell = tabStationDatn->font();
	fontCell.setPointSize( fontCell.pointSize() - ini_cell_font_dec );

	loTop->setContentsMargins( 2, MARGIN / 2, MARGIN, 0 );

	/* Схема */
	pgMap->setStyleSheet( STYLE_TABBAR );
	setupMap();

	/* Кнопки схемы */
	btnClearAll->setMinimumWidth( 140 );
	btnClearAll->setStyleSheet( STYLE_FONT_DARK );
	btnClearAll->setText( M_CLEAR_ALL );
	int x = DESKTOP_WIDTH - W_TOP_RIGHT - btnClearAll->width() - MARGIN - MARGIN - 1;
	int y = DESKTOP_HEIGHT - H_BOTTOM - btnClearAll->height() - MARGIN / 2 - 1 - MARGIN;
	btnClearAll->move( x, y );
	btnCheckAll->setMinimumWidth( 140 );
	btnCheckAll->setStyleSheet( STYLE_FONT_DARK );
	btnCheckAll->setText( M_CHECK_ALL );
	btnCheckAll->move( x - btnCheckAll->width() - MARGIN, y );

	/* Метка НЗУ */
	int n = pgMap->count() - 1;
	x = pgMap->tabBar()->tabRect( n ).x() + pgMap->tabBar()->tabRect( n ).width();
	int w = DESKTOP_WIDTH - W_TOP_RIGHT - x - MARGIN;
	lbRZU->setAlignment( Qt::AlignCenter );
	lbRZU->setGeometry( x, 0, w, H_TITLE );
	lbRZU->setText( M_NO_RZU );

	/* Состояние */
	pgTopRight->setContentsMargins( MARGIN, MARGIN / 2, 0, 0 );
	pgTopRight->setMinimumWidth( W_TOP_RIGHT );

    /* ВУ */
    loTopStation->setContentsMargins( 0, 0, 0, 0 );
    loTopStation->setSpacing( 0 );
    lbStationTitle->setAlignment( Qt::AlignCenter );
    lbStationTitle->setMinimumHeight( H_TITLE );
    pgStation->setStyleSheet( STYLE_TABBAR );
    pgStation->setDocumentMode( true );
    pgStation->setTabPosition( QTabWidget::South );
    loStationState->setContentsMargins( 0, 0, 0, 0 );
    loStationState->setSpacing( MARGIN );

    /* UPS */
    loTopUps->setContentsMargins( 0, 0, 0, 0 );
    loTopUps->setSpacing( 0 );
    for( int i = 0; i < ups_count; i++){
    lbUpsTitle[i]->setAlignment( Qt::AlignCenter );
    lbUpsTitle[i]->setMinimumHeight( H_TITLE );
//    lbUpsTitle->setAlignment( Qt::AlignCenter );
//    lbUpsTitle->setMinimumHeight( H_TITLE );
    pgUps->setStyleSheet( STYLE_TABBAR );
    pgUps->setDocumentMode( true );
    pgUps->setTabPosition( QTabWidget::South );
    loUpsState->setContentsMargins( 0, 0, 0, 0 );
    loUpsState->setSpacing( MARGIN );

    /* datn */
    tabUpsDatn[i]->setStyleSheet( STYLE_TABLE_DATA );
    tabUpsDatn[i]->verticalHeader()->setVisible( false );
    tabUpsDatn[i]->horizontalHeader()->setVisible( false );
    tabUpsDatn[i]->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tabUpsDatn[i]->setSelectionMode( QAbstractItemView::NoSelection );
    tabUpsDatn[i]->setFocusPolicy( Qt::NoFocus );
    tabUpsDatn[i]->setFont( fontSmall );
    tabUpsDatn[i]->setColumnCount( 2 );
    tabUpsDatn[i]->setColumnWidth( 0, W_TAB_VALUE + 108 );
    tabUpsDatn[i]->setColumnWidth( 1, W_TOP_RIGHT - W_TAB_VALUE - MARGIN - 108 );

#ifdef    LINK_UT_DATU
#define __LINK_UT_DATU__
#undef    LINK_UT_DATU
#endif

/** \cond */

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
    tabUpsDatn[i]->insertRow( varu_##var ); \
    tabUpsDatn[i]->setRowHeight( varu_##var, H_TAB_ROW ); \
    tcell = new QTableWidgetItem( "" ); \
    tcell->setTextAlignment( Qt::AlignCenter ); \
    tabUpsDatn[i]->setItem( varu_##var, 0, tcell ); \
    tcell = new QTableWidgetItem( title ); \
    tcell->setForeground( colTableBlack ); \
    tabUpsDatn[i]->setItem( varu_##var, 1, tcell );

#define T( N, len, var, type, title, string ) \
    tabUpsDatn[i]->insertRow( varu_##var##_##N ); \
    tabUpsDatn[i]->setRowHeight( varu_##var##_##N, H_TAB_ROW ); \
    tcell = new QTableWidgetItem( "" ); \
    tcell->setTextAlignment( Qt::AlignCenter ); \
    tabUpsDatn[i]->setItem( varu_##var##_##N, 0, tcell ); \
    snprintf( s, sizeof( s ), title " %u", N + 1 ); \
    tcell = new QTableWidgetItem( s ); \
    tcell->setForeground( colTableBlack ); \
    tabUpsDatn[i]->setItem( varu_##var##_##N, 1, tcell );

#define DATU_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T

/** \endcond */

#ifdef __LINK_UT_DATU__
#define  LINK_UT_DATU
#undef __LINK_UT_DATU__
#endif

    tabUpsDatn[i]->setMinimumHeight( tabUpsDatn[i]->rowCount() * H_TAB_ROW );
}
    /* Журнал тут может нужно оставить */
//    for ( i = 0; i < mcp_count; i++ ) {
//        lstJournal1[ i ]->setStyleSheet( STYLE_JOURNAL );
//        lstJournal1[ i ]->setSelectionMode( QAbstractItemView::NoSelection );
//        lstJournal1[ i ]->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//        lstJournal1[ i ]->setFlow( QListView::TopToBottom );
//        lstJournal1[ i ]->setWrapping( false );
//        lstJournal1[ i ]->setWordWrap( true );
//        lstJournal1[ i ]->setFont( fontSmall );
//        lstJournal1[ i ]->setSpacing( 2 );
//        lstJournal1[ i ]->setVisible( false );
//    }
    /* Сеть */
//    tabUpsNet->setStyleSheet( STYLE_TABLE_DATA );
//    tabUpsNet->verticalHeader()->setVisible( false );
//    tabUpsNet->horizontalHeader()->setVisible( false );
//    tabUpsNet->setEditTriggers( QAbstractItemView::NoEditTriggers );
//    tabUpsNet->setSelectionMode( QAbstractItemView::NoSelection );
//    tabUpsNet->setFocusPolicy( Qt::NoFocus );
//    tabUpsNet->setFont( fontSmall );
//    tabUpsNet->setColumnCount( 3 );
//    tabUpsNet->setColumnWidth( 0, W_TAB_VALUE - 8 );
//    tabUpsNet->setColumnWidth( 2, W_TAB_VALUE / 3 );
//    tabUpsNet->setColumnWidth( 1, W_TOP_RIGHT - MARGIN - tabUpsNet->columnWidth( 0 ) - tabUpsNet->columnWidth( 2 )
//    - (( ini_net_count > 1 ) ? qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) : 0 ));
//    tabUpsNet->setRowCount( ini_net_count * NET_STAT_COUNT );

//    for ( i = 0; i < NET_STAT_COUNT; i++ ) {
//        for ( int n = 0; n < ini_net_count; n++ ) {
//            int r = ( i * ini_net_count ) + n;
//            tabUpsNet->setRowHeight( r, H_TAB_ROW );
//            tcell = new QTableWidgetItem( "" );
//            tcell->setTextAlignment( Qt::AlignCenter );
//            tcell->setForeground( colTableBlack );
//            tabUpsNet->setItem( r, 0, tcell );
//            tcell = new QTableWidgetItem( M_NET_STAT[ i ]);
//            tcell->setForeground( colTableBlack );
//            tabUpsNet->setItem( r, 1, tcell );
//            tcell = new QTableWidgetItem( QString( "%1" ).arg( n + 1 ));
//            tcell->setTextAlignment( Qt::AlignCenter );
//            tcell->setForeground( colTableBlack );
//            tabUpsNet->setItem( r, 2, tcell );
//        }
//    }

    //*////////////////////////////* datn *//
    tabStationDatn->setStyleSheet( STYLE_TABLE_DATA );
    tabStationDatn->verticalHeader()->setVisible( false );
    tabStationDatn->horizontalHeader()->setVisible( false );
    tabStationDatn->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tabStationDatn->setSelectionMode( QAbstractItemView::NoSelection );
    tabStationDatn->setFocusPolicy( Qt::NoFocus );
    tabStationDatn->setFont( fontSmall );
    tabStationDatn->setColumnCount( 2 );
    tabStationDatn->setColumnWidth( 0, W_TAB_VALUE );
    tabStationDatn->setColumnWidth( 1, W_TOP_RIGHT - W_TAB_VALUE - MARGIN );

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
    tabStationDatn->insertRow( varn_##var ); \
    tabStationDatn->setRowHeight( varn_##var, H_TAB_ROW ); \
    tcell = new QTableWidgetItem( "" ); \
    tcell->setTextAlignment( Qt::AlignCenter ); \
    tabStationDatn->setItem( varn_##var, 0, tcell ); \
    tcell = new QTableWidgetItem( title ); \
    tcell->setForeground( colTableBlack ); \
    tabStationDatn->setItem( varn_##var, 1, tcell );

#define T( N, len, var, type, title, string ) \
    tabStationDatn->insertRow( varn_##var##_##N ); \
    tabStationDatn->setRowHeight( varn_##var##_##N, H_TAB_ROW ); \
    tcell = new QTableWidgetItem( "" ); \
    tcell->setTextAlignment( Qt::AlignCenter ); \
    tabStationDatn->setItem( varn_##var##_##N, 0, tcell ); \
    snprintf( s, sizeof( s ), title " %u", N + 1 ); \
    tcell = new QTableWidgetItem( s ); \
    tcell->setForeground( colTableBlack ); \
    tabStationDatn->setItem( varn_##var##_##N, 1, tcell );

#define DATN_ARRAY( len, var, type, title, string ) \
    UNFOLD( len, T, T, len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif

    tabStationDatn->setMinimumHeight( tabStationDatn->rowCount() * H_TAB_ROW );

    /* Журнал */
    for ( i = 0; i < st_count; i++ ) {
        lstJournal[ i ]->setStyleSheet( STYLE_JOURNAL );
        lstJournal[ i ]->setSelectionMode( QAbstractItemView::NoSelection );
        lstJournal[ i ]->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        lstJournal[ i ]->setFlow( QListView::TopToBottom );
        lstJournal[ i ]->setWrapping( false );
        lstJournal[ i ]->setWordWrap( true );
        lstJournal[ i ]->setFont( fontSmall );
        lstJournal[ i ]->setSpacing( 2 );
        lstJournal[ i ]->setVisible( false );
    }

    /* Сеть */
    tabStationNet->setStyleSheet( STYLE_TABLE_DATA );
    tabStationNet->verticalHeader()->setVisible( false );
    tabStationNet->horizontalHeader()->setVisible( false );
    tabStationNet->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tabStationNet->setSelectionMode( QAbstractItemView::NoSelection );
    tabStationNet->setFocusPolicy( Qt::NoFocus );
    tabStationNet->setFont( fontSmall );
    tabStationNet->setColumnCount( 3 );
    tabStationNet->setColumnWidth( 0, W_TAB_VALUE - 8 );
    tabStationNet->setColumnWidth( 2, W_TAB_VALUE / 3 );
    tabStationNet->setColumnWidth( 1, W_TOP_RIGHT - MARGIN - tabStationNet->columnWidth( 0 ) - tabStationNet->columnWidth( 2 )
    - (( ini_net_count > 1 ) ? qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) : 0 ));
    tabStationNet->setRowCount( ini_net_count * NET_STAT_COUNT );

    for ( i = 0; i < NET_STAT_COUNT; i++ ) {
        for ( int n = 0; n < ini_net_count; n++ ) {
            int r = ( i * ini_net_count ) + n;
            tabStationNet->setRowHeight( r, H_TAB_ROW );
            tcell = new QTableWidgetItem( "" );
            tcell->setTextAlignment( Qt::AlignCenter );
            tcell->setForeground( colTableBlack );
            tabStationNet->setItem( r, 0, tcell );
            tcell = new QTableWidgetItem( M_NET_STAT[ i ]);
            tcell->setForeground( colTableBlack );
            tabStationNet->setItem( r, 1, tcell );
            tcell = new QTableWidgetItem( QString( "%1" ).arg( n + 1 ));
            tcell->setTextAlignment( Qt::AlignCenter );
            tcell->setForeground( colTableBlack );
            tabStationNet->setItem( r, 2, tcell );
        }
    }

    /* Блок */
    loTopBlock->setContentsMargins( 0, 0, 0, 0 );
    loTopBlock->setSpacing( 0 );
    for ( i = 0; i < BL_CH_COUNT; i++ ) {
        lbBlockTitle[ i ]->setAlignment( Qt::AlignCenter );
        lbBlockTitle[ i ]->setMinimumHeight( H_TITLE );

        /* datb */
        tabBlockDatb[ i ]->setStyleSheet( STYLE_TABLE_DATA );
        tabBlockDatb[ i ]->verticalHeader()->setVisible( false );
        tabBlockDatb[ i ]->horizontalHeader()->setVisible( false );
        tabBlockDatb[ i ]->setEditTriggers( QAbstractItemView::NoEditTriggers );
        tabBlockDatb[ i ]->setSelectionMode( QAbstractItemView::NoSelection );
        tabBlockDatb[ i ]->setFocusPolicy( Qt::NoFocus );
        tabBlockDatb[ i ]->setFont( fontSmall );
        tabBlockDatb[ i ]->setColumnCount( 3 );
        tabBlockDatb[ i ]->setColumnWidth( 0, W_TAB_VALUE );
        tabBlockDatb[ i ]->setColumnWidth( 1, W_TOP_RIGHT - W_TAB_VALUE - W_TAB_VALUE / 2 - MARGIN );
        tabBlockDatb[ i ]->setColumnWidth( 2, W_TAB_VALUE / 2 );

/** \cond */

        int l = 0;
#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
		tabBlockDatb[ i ]->insertRow( l ); \
        tabBlockDatb[ i ]->setRowHeight( l, H_TAB_ROW_BL ); \
		tcell = new QTableWidgetItem( "" ); \
		tcell->setTextAlignment( Qt::AlignCenter ); \
		tabBlockDatb[ i ]->setItem( l, 0, tcell ); \
		tcell = new QTableWidgetItem( title ); \
		tcell->setForeground( colTableBlack ); \
		tabBlockDatb[ i ]->setItem( l, 1, tcell ); \
		tcell = new QTableWidgetItem( "" ); \
		tcell->setTextAlignment( Qt::AlignCenter ); \
		tcell->setForeground( colTableBlack ); \
		tabBlockDatb[ i ]->setItem( l++, 2, tcell );

#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */
    }

    /* МЦП */
    loTopMCP->setContentsMargins( 0, 0, 0, 0);
        loTopMCP->setSpacing( 0 );
        lbMCPTitle->setAlignment( Qt::AlignCenter );
        lbMCPTitle->setMinimumHeight( H_TITLE );
        pgMCP->setStyleSheet( STYLE_TABBAR );
        pgMCP->setDocumentMode( true );
        pgMCP->setTabPosition( QTabWidget::South );
        loMCPState->setContentsMargins( 0, 0, 0, 0 );
        loMCPState->setSpacing( MARGIN );

        /* datfile */
        tabMCPDatmcp->setStyleSheet( STYLE_TABLE_DATA );
        tabMCPDatmcp->verticalHeader()->setVisible( false );
        tabMCPDatmcp->horizontalHeader()->setVisible( false );
        tabMCPDatmcp->setEditTriggers( QAbstractItemView::NoEditTriggers );
        tabMCPDatmcp->setSelectionMode( QAbstractItemView::NoSelection );
        tabMCPDatmcp->setFocusPolicy( Qt::NoFocus );
        tabMCPDatmcp->setFont( fontSmall );
        tabMCPDatmcp->setColumnCount( 2 );
        tabMCPDatmcp->setColumnWidth( 0, W_TAB_VALUE );
        tabMCPDatmcp->setColumnWidth( 1, W_TOP_RIGHT - W_TAB_VALUE - MARGIN );

        /** \cond */

        #define DATMCP_THREAD( thread )
        #define DATMCP_TYPE( T, format, strto )
        #define DATMCP_VAR( var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMCPDatmcp->insertRow( varmcp_##var ); \
                tabMCPDatmcp->setRowHeight( varmcp_##var, H_TAB_ROW ); \
                tcell = new QTableWidgetItem( "" ); \
                tcell->setTextAlignment( Qt::AlignCenter ); \
                tabMCPDatmcp->setItem( varmcp_##var, 0, tcell ); \
                tcell = new QTableWidgetItem( title ); \
                tcell->setForeground( colTableBlack ); \
                tabMCPDatmcp->setItem( varmcp_##var, 1, tcell ); \
            }

        #define T( N, len, var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMCPDatmcp->insertRow( varmcp_##var##_##N ); \
                tabMCPDatmcp->setRowHeight( varmcp_##var##_##N, H_TAB_ROW ); \
                tcell = new QTableWidgetItem( "" ); \
                tcell->setTextAlignment( Qt::AlignCenter ); \
                tabMCPDatmcp->setItem( varmcp_##var##_##N, 0, tcell ); \
                snprintf( s, sizeof( s ), title " %u", N + 1 ); \
                tcell = new QTableWidgetItem( s ); \
                tcell->setForeground( colTableBlack ); \
                tabMCPDatmcp->setItem( varmcp_##var##_##N, 1, tcell ); \
            }

        #define DATMCP_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

        #include "datmcp-dict.h"

        #undef DATMCP_THREAD
        #undef DATMCP_TYPE
        #undef DATMCP_VAR
        #undef DATMCP_ARRAY
        #undef T

        /** \endcond */


        tabMCPDatmcp->setMinimumHeight( tabMCPDatmcp->rowCount() * H_TAB_ROW );
        /* Сеть */

        tabMCPNet->setStyleSheet( STYLE_TABLE_DATA );
        tabMCPNet->verticalHeader()->setVisible( false );
        tabMCPNet->horizontalHeader()->setVisible( false );
        tabMCPNet->setEditTriggers( QAbstractItemView::NoEditTriggers );
        tabMCPNet->setSelectionMode( QAbstractItemView::NoSelection );
        tabMCPNet->setFocusPolicy( Qt::NoFocus );
        tabMCPNet->setFont( fontSmall );
        tabMCPNet->setColumnCount( 3 );
        tabMCPNet->setColumnWidth( 0, W_TAB_VALUE - 8 );
        tabMCPNet->setColumnWidth( 2, W_TAB_VALUE / 3 );
        tabMCPNet->setColumnWidth( 1, W_TOP_RIGHT - MARGIN - tabMCPNet->columnWidth( 0 ) - tabMCPNet->columnWidth(2)
          -(( ini_net_count > 1 ) ? qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) : 0 ));
        tabMCPNet->setRowCount( ini_net_count*NET_STAT_COUNT );


        for ( i = 0; i < NET_STAT_COUNT; i++ ) {
            for( int n = 0; n < ini_net_count; n++ ){
                int f = ( i * ini_net_count ) + n ;
            tabMCPNet->setRowHeight( f, H_TAB_ROW );
            tcell = new QTableWidgetItem( "" );
            tcell->setTextAlignment( Qt::AlignCenter );
            tcell->setForeground( colTableBlack );
            tabMCPNet->setItem( f, 0, tcell );
            tcell = new QTableWidgetItem( M_NET_STAT[ i ]);
            tcell->setForeground( colTableBlack );
            tabMCPNet->setItem( f, 1, tcell );
            tcell = new QTableWidgetItem( QString( "%1" ).arg( n + 1 ));
            tcell->setTextAlignment( Qt::AlignCenter );
            tcell->setForeground( colTableBlack );
            tabMCPNet->setItem( f, 2, tcell);
            }
        }

    /* МИ */
    loTopMeter->setContentsMargins( 0, 0, 0, 0 );
    loTopMeter->setSpacing( 0 );
    lbMeterTitle->setAlignment( Qt::AlignCenter );
    lbMeterTitle->setMinimumHeight( H_TITLE );
    pgMeter->setStyleSheet( STYLE_TABBAR );
    pgMeter->setDocumentMode( true );
    pgMeter->setTabPosition( QTabWidget::South );
    loMeterState->setContentsMargins( 0, 0, 0, 0 );
    loMeterState->setSpacing( MARGIN );

    /* datm */
    tabMeterDatm->setStyleSheet( STYLE_TABLE_DATA );
    tabMeterDatm->verticalHeader()->setVisible( false );
    tabMeterDatm->horizontalHeader()->setVisible( false );
    tabMeterDatm->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tabMeterDatm->setSelectionMode( QAbstractItemView::NoSelection );
    tabMeterDatm->setFocusPolicy( Qt::NoFocus );
    tabMeterDatm->setFont( fontSmall );
    tabMeterDatm->setColumnCount( 2 );
    tabMeterDatm->setColumnWidth( 0, W_TAB_VALUE );
    tabMeterDatm->setColumnWidth( 1, W_TOP_RIGHT - W_TAB_VALUE - MARGIN );

    /** \cond */

    #define DATM_THREAD( thread )
    #define DATM_TYPE( T, format, strto )
    #define DATM_VAR( var, type, title, string ) \
        if ( sizeof( title ) > 1 ) { \
            tabMeterDatm->insertRow( varm_##var ); \
            tabMeterDatm->setRowHeight( varm_##var, H_TAB_ROW ); \
            tcell = new QTableWidgetItem( "" ); \
            tcell->setTextAlignment( Qt::AlignCenter ); \
            tabMeterDatm->setItem( varm_##var, 0, tcell ); \
            tcell = new QTableWidgetItem( title ); \
            tcell->setForeground( colTableBlack ); \
            tabMeterDatm->setItem( varm_##var, 1, tcell ); \
        }

    #define T( N, len, var, type, title, string ) \
        if ( sizeof( title ) > 1 ) { \
            tabMeterDatm->insertRow( varm_##var##_##N ); \
            tabMeterDatm->setRowHeight( varm_##var##_##N, H_TAB_ROW ); \
            tcell = new QTableWidgetItem( "" ); \
            tcell->setTextAlignment( Qt::AlignCenter ); \
            tabMeterDatm->setItem( varm_##var##_##N, 0, tcell ); \
            snprintf( s, sizeof( s ), title " %u", N + 1 ); \
            tcell = new QTableWidgetItem( s ); \
            tcell->setForeground( colTableBlack ); \
            tabMeterDatm->setItem( varm_##var##_##N, 1, tcell ); \
        }

    #define DATM_ARRAY( len, var, type, title, string ) \
        UNFOLD( len, T, T, len, var, type, title, string )

    #include "datm-dict.h"

    #undef DATM_THREAD
    #undef DATM_TYPE
    #undef DATM_VAR
    #undef DATM_ARRAY
    #undef T

    /** \endcond */

    tabMeterDatm->setMinimumHeight( tabMeterDatm->rowCount() * H_TAB_ROW );

    /* Сеть */
//	tabMeterNet->setStyleSheet( STYLE_TABLE_DATA );
//	tabMeterNet->verticalHeader()->setVisible( false );
//	tabMeterNet->horizontalHeader()->setVisible( false );
//	tabMeterNet->setEditTriggers( QAbstractItemView::NoEditTriggers );
//	tabMeterNet->setSelectionMode( QAbstractItemView::NoSelection );
//	tabMeterNet->setFocusPolicy( Qt::NoFocus );
//	tabMeterNet->setFont( fontSmall );
//	tabMeterNet->setColumnCount( 2 );
//	tabMeterNet->setColumnWidth( 0, W_TAB_VALUE - 8 );
//	tabMeterNet->setColumnWidth( 1, W_TOP_RIGHT - MARGIN - tabMeterNet->columnWidth( 0 ));
//	tabMeterNet->setRowCount( NET_STAT_COUNT );

//	for ( i = 0; i < NET_STAT_COUNT; i++ ) {
//		tabMeterNet->setRowHeight( i, H_TAB_ROW );
//		tcell = new QTableWidgetItem( "" );
//		tcell->setTextAlignment( Qt::AlignCenter );
//		tcell->setForeground( colTableBlack );
//		tabMeterNet->setItem( i, 0, tcell );
//		tcell = new QTableWidgetItem( M_NET_STAT[ i ]);
//		tcell->setForeground( colTableBlack );
//		tabMeterNet->setItem( i, 1, tcell );
//	}
    //nik
    loMCPState->addWidget(tabMCPDatmcp);
    wgMCPState->setLayout( loMCPState);
    pgMCP->addTab( wgMCPState, M_MCP_STATE);
    pgMCP->addTab( tabMCPNet, M_NET );
    loTopMCP->addWidget(lbMCPTitle);
    loTopMCP->addWidget(pgMCP);
    wgTopMCP->setLayout(loTopMCP);

//    for( i = 0; i < st_count; i++){
//        if ( st_list[ i ].TypeMRMO == MCP_TYPE){ // тут проверить на имя
//           loMCPState->addWidget( lstJournal[i]);
//        }
//    }
    //nik

    loMeterState->addWidget( tabMeterDatm );
    wgMeterState->setLayout( loMeterState );
    pgMeter->addTab( wgMeterState, M_METER_STATE );
    //pgMeter->addTab( tabMeterNet, M_NET );
    loTopMeter->addWidget( lbMeterTitle );
    loTopMeter->addWidget( pgMeter );
    wgTopMeter->setLayout( loTopMeter );

    for ( i = 0; i < BL_CH_COUNT; i++ ) {
        loTopBlock->addWidget( lbBlockTitle[ i ]);
        loTopBlock->addWidget( tabBlockDatb[ i ]);
    }
    wgTopBlock->setLayout( loTopBlock );

    for ( i = 0; i < ups_count; i++ ) {
        loUpsState->addWidget( lbUpsTitle[i] );
        loUpsState->addWidget( tabUpsDatn[i] );
    }
	
//    for ( i = 0; i < st_count; i++ ) {
//        if ( st_list[ i ].TypeMRMO == MI_TYPE ) {
//            loMeterState->addWidget( lstJournal[ i ]); ну это все дублируется ниже тут
//        }
//    }

    wgUpsState->setLayout( loUpsState );
    pgUps->addTab( wgUpsState, M_UPS_STATE );
    loTopUps->addWidget( pgUps );
    wgTopUps->setLayout( loTopUps );
    loStationState->addWidget( tabStationDatn );
    for ( i = 0; i < st_count; i++ ) {
        if ( st_list[ i ].TypeMRMO == MCP_TYPE ) {
            loMCPState->addWidget( lstJournal[ i ]);
        } else {
            loStationState->addWidget( lstJournal[ i ]);
        }
    }

    wgStationState->setLayout( loStationState );
    pgStation->addTab( wgStationState, M_STATION_STATE );
    pgStation->addTab( tabStationNet, M_NET );
    loTopStation->addWidget( lbStationTitle );
    loTopStation->addWidget( pgStation );
    wgTopStation->setLayout( loTopStation );

    pgTopRight->addWidget( wgTopStation );
    pgTopRight->addWidget( wgTopBlock );
    pgTopRight->addWidget( wgTopMeter );
    pgTopRight->addWidget( wgTopUps );
    pgTopRight->addWidget( wgTopMCP );
    connect( btnClearAll, SIGNAL( clicked()), this, SLOT( onClearAllClicked()));
    connect( btnCheckAll, SIGNAL( clicked()), this, SLOT( onCheckAllClicked()));
    loTop->addWidget( pgMap, 1 );
    loTop->addWidget( pgTopRight, 0 );
}

void MainWindow::setupMap( void )
{
	unsigned i;
	uint8_t page = 0;
	QString _name;

	for ( i = 0; i < page_count; i++ ) {
		grMap.append( new QGroupBox( pgMap ));
		grMapButton.append( new QButtonGroup( pgMap ));
	}
	pgMap->setDocumentMode( true );

	for ( i = 0; i < page_count; i++ ) {
		grMapButton[ i ]->setExclusive( true );
		grMap[ i ]->setTitle( "" );
		grMap[ i ]->setStyleSheet( STYLE_MAP );
	}
	page = 0;
	for ( int i = 0; i < map_count; i++ ) {
		MapCell *c = new MapCell( this, page, i );
		/* Check picture indexes */
		if ( map[ i ].pic  > map_pic_count ) map[ i ].pic  = 0; /* 0 - Skin::BUTTON */
		if ( map[ i ].pico > map_pic_count ) map[ i ].pico = 0;
		if ( map[ i ].pice > map_pic_count ) map[ i ].pice = 0;
        if ( map[ i ].picw > map_pic_count ) map[ i ].picw = 0;
        if ( map[ i ].picp > map_pic_count ) map[ i ].picp = 0;

		switch ( map[ i ].kind ) {
		/* ВУ */
		case MAP_ST:
			_name = st_list_ex[ map[ i ].ind ].name;
            _name.replace( "_", " " );
			break;
		/* Блок */
		case MAP_BL:
			_name = bl_list[ map[ i ].ind ].name;
			_name.replace( "  ", "\n" );
            _name.replace( "_", "\n" );
			break;
		/* МИ */
		case MAP_MI:
			_name = mi_list[ map[ i ].ind ].name;
			break;
        /* МЦП */
        case MAP_MCP:
            _name = mcp_list[map[i].ind].name;
            break;
		/* Статическое изображение */
		case MAP_IMAGE:
			_name = QString( map[ i ].mark );
			break;
        case MAP_UPS:
            _name = st_list_ex1[ map[ i ].ind ].name;
            break;
		default:
			_name = QString( "<unknown>" );
		}
		cell.append( c );
		c->setupCell( map[ i ].br, _name );
		grMapButton[ page ]->addButton( c, i );
		if (( map[ i ].br == BR_PAGE ) && ( page < page_count - 1 )) {
			++page;
			page_list[ page ].first = i + 1;
		}
	}
	for ( i = 0; i < page_count; i++ ) {
		pgMap->addTab( grMap[ i ], page_list[ i ].name );
		connect( grMapButton[ i ], SIGNAL( buttonClicked( int )), this, SLOT( onMapClicked( int )));
	}
}

void MainWindow::setup( void )
{
	setGeometry( 1, 1, DESKTOP_WIDTH, DESKTOP_HEIGHT);
	if ( ini_net_count == 1 ) {
		setWindowTitle( __PROGRAM__ "_t v" __PROGRAM_VERSION__ );
	} else {
		setWindowTitle( __PROGRAM__ " v" __PROGRAM_VERSION__ );
	}
	QWidget *wgCentral = new QWidget( this );
	QVBoxLayout *loCentral = new QVBoxLayout( wgCentral );
	QHBoxLayout *loTop = new QHBoxLayout();
	QHBoxLayout *loBottom = new QHBoxLayout();

	loCentral->setSpacing( 0 );
	loCentral->setMargin( 0 );
	loCentral->setContentsMargins( 0, 0, 0, 0 );

	setupBottom( loBottom );
	setupTop(loTop );

    loCentral->addLayout( loTop, 1 );
    loCentral->addLayout( loBottom, 0 );
    wgCentral->setLayout( loCentral );
	setCentralWidget( wgCentral );

	timerStop = new QTimer( this );
    connect( timerStop, SIGNAL( timeout()), this, SLOT( onTimerStop()));
	timerStop->start( 100 );
}

MainWindow::MainWindow( QWidget *parent ) :
QMainWindow( parent ),
testRunning( 0 ),
packRunning( 0 ),
testRunning2( 0 ),
testRunning3( 0 ),
idle( true ),
idle2( true ),
idle3( true ),
cntBackspace( 0 ),
timerBlockProgAll( NULL ),
dlgSelectRZU( NULL ),
dlgStartRZU( NULL ),
dlgTestNet( NULL ),
dlgClock( NULL )
{
	st_mask = 0;
	mi_mask = 0;
    mcp_mask = 0;

	gui_event_init();
	setupPack();
	setup();

	setBottomPage( PAGE_TEST );
	selectMapFirst();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onTimerStop( void )
{
	static bool firstCall = true;

	if ( firstCall ) {
		firstCall = false;
		timerStop->stop();
		timerStop->start( 1000 );
	} else {
		switch ( cntBackspace ) {

		default:
			/* Прервать пакетное тестирование */
			stopPackMessage();
			/* fallthrough */

		case 1:
			/* Остановить любой выполняемый тест */
			stopTestMessage( NO_TEST );
			cntBackspace = 0;
			break;

		case 0:
			break;
		}
	}
}

void MainWindow::onExitClicked( void )
{
//   char package[44] = {
//        0x30, 0x2a, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, (char) 0xa2, 0x1d, 0x02,
//        0x01, 0x01, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x12, 0x30, 0x10, 0x06, 0x09, 0x2b, 0x06,
//       0x01, 0x02, 0x01, 0x19, 0x02, 0x02, 0x00, 0x02, 0x03, 0x3a, (char) 0xdc, 0x38
//   };

//    UPS_RESPONSE* oids = parse_snmp(package);
//   if ( oids != 0 )
//    {
//       std::string str = "1.3.6.1.2.1.25.2.2.0: " + std::to_string(oids->val1);
//        printf(str.c_str());
//        return;
//  }



//    SNMP_MESSAGE snmp;
//    char pass[6] = {'p', 'u', 'b', 'l', 'i', 'c'};
//    snmp.init_message(0, pass, sizeof (pass));
//    uint32_t OID_SYS_UP_TIME[9] = {0x2b, 6, 1, 2, 1, 25, 2, 2, 0};
////    uint32_t OID_SYS_UP_TIME[8] = {0x2b, 6, 1, 2, 1, 1, 3, 0};
//    snmp.add_oid(OID_SYS_UP_TIME, 9);
//    snmp.commit();

//    MESSAGE_REMOTE mg;
////    char payload[39] = "Custom message";
//    mg.len = snmp.frame_len;
//    mg.data = snmp.frame;
//    mg.isForUps = true;

////    memcpy( mg.malloc_data( snmp.frame_len ), snmp.frame, snmp.frame_len );
//    mg.init_transmitted( NULL );
//    pthread_mutex_lock( &udp_t_mutex );
//    udp_t_queue.push( mg );
//    pthread_mutex_unlock( &udp_t_mutex );

//    snmp.free();

    if ( ini_gui_exit_confirm ) {
        QMessageBox msgBox;
        QPushButton *btnExit = msgBox.addButton( M_EXIT_OK, QMessageBox::AcceptRole );
        QPushButton *btnMinimize = msgBox.addButton( M_EXIT_MINIMIZE, QMessageBox::AcceptRole );
        QPushButton *btnCancel = msgBox.addButton( M_EXIT_CANCEL, QMessageBox::RejectRole );

        msgBox.setWindowTitle( M_DLG_EXIT );
        msgBox.setIcon( QMessageBox::Question );
        msgBox.setText( M_DLG_EXIT_MSG );
        msgBox.setInformativeText( M_DLG_EXIT_MINIMIZE_MSG );
        btnExit->setMinimumWidth( W_BTN_BOTTOM );
        btnMinimize->setMinimumWidth( W_BTN_BOTTOM );
        btnCancel->setMinimumWidth( W_BTN_BOTTOM );

        msgBox.exec();
        if ( msgBox.clickedButton() == btnExit ) {
            app_alive = 0;
        } else if ( msgBox.clickedButton() == btnMinimize ) {
            minimizeWindow();
        }
    } else {
        app_alive = 0;
    }
}

#define _OLD_  2



void MainWindow::customEvent( QEvent *event )
{
    QString msg;
    int _type = event->type() - QEvent::User;
    REPORT_MESSAGE* mg;
    int st, cicle;
    uint8_t ind, abonent;
    //
     char s[ 256 ];

    switch ( _type ) {


    case EVENT_SELECT_RZU:
        if ( data_get_byte( var_select_rzu ) == 1 ) break;

        /* Показать диалог выбора РЗУ */
        data_set_byte( var_select_rzu, 1 );
        if ( rzu_count ) {
            if ( !dlgSelectRZU ) dlgSelectRZU = new SelectRZUDialog( this );
            dlgSelectRZU->exec();
        } else {
            msg = QString( M_RZU_NOT_FOUND ).arg( ini_dir_rzu ).arg(rzu_error);
            /* Записать в журнал */
            journal_freeze_time();
            journal_print( "", msg.toUtf8());
            /* Сообщение */
            QMessageBox::critical( this, M_DLG_ERROR, msg, QMessageBox::Ok );
        }
        break;

    case EVENT_RZU_CHANGED:
        /* Изменился РЗУ */
        rzu_ind = rzu_search( datn_get_dword( varn_verify, st_tst ));
        if ( rzu_ind < rzu_count ) {

//QMessageBox::critical( this, M_DLG_ERROR, rzu_dir[ rzu_ind ], QMessageBox::Ok );

            st_list_check( rzu_dir[ rzu_ind ] + "/" FILE_ST_LIST );
            bl_list_read( rzu_dir[ rzu_ind ] + "/" FILE_BL_LIST );
            lbRZU->setText( decode_cp1251(( char *) rzu_passport[ rzu_ind ].szRemark ));
        } else {
            lbRZU->setText( M_NO_RZU );
        }
        break;

    case EVENT_MI_FILE_ERROR:
        /* Ошибка при чтении файла коррект. кодов МИ */
        msg = QString( M_MI_FILE_ERROR ).arg( ini_file_mi_comp );
        /* Записать в журнал */
        journal_freeze_time();
        journal_print( "", msg.toUtf8());
        /* Сообщение */
        QMessageBox::critical( this, M_DLG_ERROR, msg, QMessageBox::Ok );
        break;

    case EVENT_BL_PROG_OK:
        if ( timerBlockProgAll == NULL ) {  /* Запись ПО одного блока */
            /* Завершена передача ПО блока */
            QMessageBox::information( this, M_DLG_MESSAGE, M_BL_PROG_OK, QMessageBox::Ok );
        }
        break;

    case EVENT_BL_PROG_ERROR:
        QMessageBox::critical( this, M_DLG_ERROR, M_BL_PROG_ERROR, QMessageBox::Ok );
        break;

    case EVENT_REPORT:
        /* Сообщение для журнала */
        mg = __queue_front();
        while ( mg ) {
            switch ( mg->kind ) {

            case REPORT_JOURNAL:
                setJournal( mg->ind, mg->s, mg->col );
                break;

            case REPORT_JOURNAL2:
                setJournal2( mg->ind, mg->s, mg->col );
                break;

            case REPORT_SCROLL:
                scrollSummary( mg->ind );
                break;

            case REPORT_SUMMARY:
            case REPORT_SUMMARY_FOCUS:
                setSummary( mg->ind, mg->s, mg->col, mg->kind == REPORT_SUMMARY_FOCUS );
                break;
            }
            __queue_pop();
            mg = __queue_front();
        }
        break;

    case EVENT_TD_VERIFY:
        /* Обработка проверки ТД */
        if ( data_get_changed( peek_gui, var_td_check )) {
            data_reset( peek_gui, var_td_check );
            if ( data_get_byte( var_td_check ) == TEST_FAILED ) {
                /* Записать в журнал */
                journal_freeze_time();
                journal_print( "", M_TD_ERROR );
                /* Сообщение */
                QString msg = QString(M_TD_ERROR).arg(dbg_td);
                QMessageBox::critical( this, M_DLG_ERROR, msg, QMessageBox::Ok );
            }
        }
        break;

    case EVENT_TD_OPERATE:
        if ( data_get_changed( peek_gui, var_td_operate )) {
            QAbstractButton *b1 = grPrepButton->button( PREP_TD_LOAD );
            QAbstractButton *b2 = grPrepButton->button( PREP_TD_CHECK );

            switch ( data_get_byte( var_td_operate )) {
            case TD_WRITE:
                b1->setUpdatesEnabled( false );
                b1->setStyleSheet( "color: #695c7d;" );
                b1->setUpdatesEnabled( true );
                b1->repaint();
                break;

            case TD_CHECK:
                b2->setUpdatesEnabled( false );
                b2->setStyleSheet( "color: #695c7d;" );
                b2->setText( M_PREP_TD_CHECK M_STOP );
                b2->setUpdatesEnabled( true );
                b2->repaint();
                break;

            case TD_NONE:
                b1->setUpdatesEnabled( false );
                b1->setStyleSheet( "color: black;" );
                b1->setUpdatesEnabled( true );
                b1->repaint();
                b2->setUpdatesEnabled( false );
                b2->setStyleSheet( "color: black;" );
                b2->setText( M_PREP_TD_CHECK );
                b2->setUpdatesEnabled( true );
                b2->repaint();
                break;
            }
            data_reset( peek_gui, var_td_operate );
            calcRunning();
        }
        break;

    case EVENT_DATN_RECEIVED ... EVENT_DATN_RECEIVED + DATN_DEPTH - 1:
        /* Принято состояние ВУ */
        st = _type - EVENT_DATN_RECEIVED;
        if ( datn_get_changed_any( peekn_gui, st )) {
            int online = _OLD_;
            int fail = _OLD_;
            /* Что-то изменилось */
            datn_clear_updated( st );
            ind = st_list_ex[ st ].ind;
            if ( datn_get_changed( peekn_gui, varn_state, st )) {
                /* Изменилось состояние */
                uint8_t state = datn_get_byte( varn_state, st );

                cell[ ind ]->setState( state );
                online = 1;
                switch ( state ) {

                case TURN_ON:
                case TEST_STATE:
                    cell[ ind ]->setEnabled( true );
                    break;

                case TURN_OFF:
                    online = 0;
                    /* Сброс значений */
                    for ( int i = 0; i < ini_net_count; i++ ) datn_set_byte( varn_link + i, LINK_NONE, st );
                    if ( st_list_ex[ st ].gr < GR_LIMIT ) {
                        datb_clean_vector( st );
                        gui_send_event( EVENT_DATB_RECEIVED );
                    }
                    /* fallthrough */

                case PKF:
                case WORK:
                    cell[ ind ]->setEnabled( false );
                    /* Сброс значений */
                    datn_set_dword( varn_date, 0, st );
                    datn_set_dword( varn_time, 0, st );
                    datn_set_word( varn_test_perform, PERFORM_SET( TEST, NO_TEST ) | PERFORM_SET( PACK, PACK_NONE ), st );
                    for ( int i = varn_test_; i < varn_test_ + TEST_COUNT; i++ ) datn_set_byte( i, TEST_NONE, st );
                    if( datn_get_byte(varn_failure, st)) // все if
                        datn_set_byte(varn_failure, st, 0);
                    cell[ind]->setFail( false );
                }
            }
            if ( map[ ind ].kind == MAP_ST ) {
                /* Станция отображается на схеме */
                if ( datn_get_changed( peekn_gui, varn_test_percent, st )) {
                    /* Изменились процент выполнения теста */
                    char s[ 16 ];
                    cell[ ind ]->setText( datn_get_string_byte( s, varn_test_percent, st ));
                }
                if ( datn_get_changed( peekn_gui, varn_test_perform, st )) {
                    /* Изменились признаки выполнения тестов */
                    cell[ ind ]->setText(( PERFORM_GET( datn_get_word( varn_test_perform, st ), TEST ) != NO_TEST ) ?
                        M_PERFORM_TEST : "" );
                }
                if ( datn_get_changed( peekn_gui, varn_failure, st )) {
                    /* Изменился показатель неисправностей */

//printf("cell mask %X \n", data_get_dword( var_onelink_mask ) & BIT( st ));


                    fail = ( datn_get_word( varn_failure, st ) || ( data_get_dword( var_onelink_mask ) & BIT( st )));

                    cell[ ind ]->setFail( fail );
                }
                /* Состояние сетей */
                for ( int i = 0; i < ini_net_count; i++ ) {
                    if ( datn_get_changed( peekn_gui, varn_link + i, st )) {
                        cell[ ind ]->setConnectState( i, datn_get_byte( varn_link + i, st ));
                    }
                }
                if ( ind == selected ) {
                    /* Изменилось состояние выбранной станции */
                    DATN_VECTOR *d = datn_get_vector( st );
                    char s[ 256 ];
                    int col;

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif
                    switch ( online ) {
                    /* goto offline */
                    case 0:

                        lbStationTitle->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
                        tabStationDatn->item( varn_##var, 0 )->setText( "-" ); \
                        tabStationDatn->item( varn_##var, 0 )->setForeground( QColor( COLOR_DEFAULT ));

#define T( N, len, var, type, title, string ) \
                        tabStationDatn->item( varn_##var##_##N, 0 )->setText( "-" ); \
                        tabStationDatn->item( varn_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT ));

#define DATN_ARRAY( len, var, type, title, string ) \
                        UNFOLD( len, T, T, len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */
                        break;
                    /* goto online */
                    case 1:
                        lbStationTitle->setStyleSheet( STYLE_TITLE_GREEN );
                        datn_set_all_changed( peekn_gui, st );
                        /* fallthrough */

                    case _OLD_:
                        if ( datn_get_byte( varn_state, st ) == TURN_OFF ) break;

                        if ( fail != _OLD_ ) {
                            lbStationTitle->setStyleSheet( fail ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
                        }
                        datn_lock();
/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
                        if ( datn_get_changed_wo_lock( peekn_gui, varn_##var, st )) { \
                            col = COLOR_DEFAULT; \
                            tabStationDatn->item( varn_##var, 0 )->setText( string( s, d->var, &col )); \
                            tabStationDatn->item( varn_##var, 0 )->setForeground( QColor( col )); \
                        }

#define T( N, len, var, type, title, string ) \
                        if ( datn_get_changed_wo_lock( peekn_gui, varn_##var##_##N, st )) { \
                            col = COLOR_DEFAULT; \
                            tabStationDatn->item( varn_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
                            tabStationDatn->item( varn_##var##_##N, 0 )->setForeground( QColor( col )); \
                        }

#define DATN_ARRAY( len, var, type, title, string ) \
                        UNFOLD( len, T, T, len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */
                        datn_unlock();
                        break;
                    }

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif
                }
            }
            /* Проверка изменения состояния тестов */
            if ( datn_get_changed( peekn_gui, varn_test_perform, st )) {

//printf("\ntesting change \n");

                if ( st_mask & BIT( st )) calcRunning();
            }
            if ( !datn_check_updated( st )) {
                /* Сброс признаков изменения */
                datn_reset_all( peekn_gui, st );
            }
        }

        break;

    case EVENT_STAT_RECEIVED ... EVENT_STAT_RECEIVED + DATN_DEPTH - 1:
        /* Принята сетевая статистика */
        st = _type - EVENT_STAT_RECEIVED;
        ind = st_list_ex[ st ].ind;

        if (( ind == selected ) && ( map[ ind ].kind == MAP_ST )) {
            NET_STAT *stat = net_stat + st;
            pthread_mutex_lock( &test_net_mutex_result );

            for ( int i = 0; i < ( int ) NET_STAT_COUNT; i++ ) {
                for ( int n = 0; n < ini_net_count; n++ ) {
                    dword val = stat->data[ n ][ i ];
                    tabStationNet->item(( i * ini_net_count ) + n, 0 )->setText(
                        ( val == _MAXDWORD ) ? "-" : QString::number( val )
                    );
                }
            }
            pthread_mutex_unlock( &test_net_mutex_result );
        }
        break;

    case EVENT_DATB_RECEIVED:
        /* Принято состояние блоков */
        for ( int i = 0; i < bl_count; i++ ) {
            if ( datb_get_changed_any( i )) {
                uint8_t ind = bl_list[ i ].ind;
                /* Что-то изменилось */
                datb_clear_updated( i );
                if ( map[ ind ].kind == MAP_BL ) {
                    /* Блок отображается на схеме */
                    if ( datb_get_changed_bl( varb_state, i )) {
                        /* Изменилось состояние блока */
printf("state changed new:%d (%d)\n", datb_get_bl( varb_state, i, &voteb_state ), i);
                        cell[ ind ]->setState( datb_get_bl( varb_state, i, &voteb_state ));
                    }
                    if ( datb_get_changed_bl( varb_red, i )) {
                        /* Изменился показатель неисправностей */
printf("error changed new:%d (%d)\n", datb_get_bl( varb_red, i, &voteb_failure ), i);
                        cell[ ind ]->setFail( datb_get_bl( varb_red, i, &voteb_failure ) != 0 );
                    }
#if 0
/* Отладочная запись изменения состояния блоков в log */
                    for ( uint8_t ch = 0; ch < BL_CH_COUNT; ch++ ) {
                        int col = COLOR_DEFAULT;
/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
                        if ( datb_get_changed( varb_##var, i, ch )) { \
                            log_printf( "WARN", "%s К%d %s\t" title "\t%s" \
                            , bl_list[ i ].name_ch \
                            , ch + 1 \
                            , datb_get_host_string( varb_##var, i, ch ) \
                            , datb_get_string( varb_##var, i, ch, &col ) \
                            ); \
                        }
#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */
                    }
#endif
                    if ( ind == selected ) {
                        /* Изменилось состояние выбранного блока */
                        for ( uint8_t ch = 0; ch < BL_CH_COUNT; ch++ ) {
                            int col;
                            uint8_t l = 0;
                            valb_t state = datb_get( varb_state, i, ch, 0 );

                            if ( state == REGWORK_VU_OFF ) {
                                if ( datb_get_changed( varb_state, i, ch )) {
                                    lbBlockTitle[ ch ]->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
                                    tabBlockDatb[ ch ]->item( l, 0 )->setText( "-" ); \
                                    tabBlockDatb[ ch ]->item( l, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
                                    tabBlockDatb[ ch ]->item( l++, 2 )->setText( "" );

#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */
                                }
                            } else {
                                if ( datb_get_changed( varb_state, i, ch )) datb_set_all_changed( i, ch );
                                if ( datb_get_changed( varb_red, i, ch )) {
                                    /* Изменилось состояние канала */
                                    lbBlockTitle[ ch ]->setStyleSheet(
                                    datb_get( varb_red, i, ch, 0 ) ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
                                }
/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
                                if ( datb_get_changed( varb_##var, i, ch )) { \
                                    col = COLOR_DEFAULT; \
                                    tabBlockDatb[ ch ]->item( l, 0 )->setText( datb_get_string( varb_##var, i, ch, &col )); \
                                    tabBlockDatb[ ch ]->item( l, 0 )->setForeground( QColor( col )); \
                                    tabBlockDatb[ ch ]->item( l, 2 )->setText( datb_get_host_string( varb_##var, i, ch )); \
                                } \
                                ++l;

#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */
                            }
                        }
                    }
                }
                if ( !datb_check_updated( i )) {
                    /* Сброс признаков изменения */
                    datb_reset_all_bl( i );
                }
            }
        }
        break;

    case EVENT_DATM_RECEIVED ... EVENT_DATM_RECEIVED + DATMCP_DEPTH - 1:  // TYT
           /* Принято состояние MCP */
           st = _type - EVENT_DATM_RECEIVED;

           if ( datmcp_get_changed_any( peekmcp_gui, st )) {

               int online = _OLD_;
               int fail = _OLD_;
               /* Что-то изменилось */
               datmcp_clear_updated( st );
               ind = mcp_list[ st ].ind;
               if ( datmcp_get_changed( peekmcp_gui, varmcp_stateM, st )) {
                   /* Изменилось состояние */
                   uint8_t state = datmcp_get_byte( varmcp_stateM, st );

                   cell[ ind ]->setState( state );
                   cell[ ind ]->setConnectState( 0, ( state == MCP_OFF ) ? LINK_NONE : LINK_CONNECTED );
                   online = 1;
                   switch ( state ) {

                   case MCP_ON:
                   case MCP_ON_RESET:
                       cell[ ind ]->setEnabled( true );
                       break;

                   case MCP_OFF:

                       online = 0;
                       /* Сброс значений */
                       datmcp_set_dword( varmcp_sn, 0, st );
                       datmcp_set_dword( varmcp_col_sn, COLOR_DEFAULT, st );
                       datmcp_set_dword( varmcp_col_verify_c, COLOR_DEFAULT, st );
                       /* fallthrough */

                   case MCP_WORK:
                   case MCP_WORK_RESET:
                       cell[ ind ]->setEnabled( false );
                       /* Сброс значений */
                       datmcp_set_byte( varmcp_test_1, TEST_NONE, st );
                       datmcp_set_byte( varmcp_test_net, TEST_NONE, st );
                       datmcp_set_byte( varmcp_test_percent, 0, st );
                   }
               }
               if ((datmcp_get_byte(varmcp_net0,st)==0) && (datmcp_get_byte(varmcp_net1,st)==0)){
                   datmcp_set_byte(varmcp_red,0, st);
               }
               if ( map[ ind ].kind == MAP_MCP ) {
                   /* MCP отображается на схеме */
                   if ( datmcp_get_changed( peekmcp_gui, varmcp_test_percent, st )) {
                       /* Изменились процент выполнения теста */
                       char s[ 16 ];
                       cell[ ind ]->setText( datmcp_get_string_byte( s, varmcp_test_percent, st ));
                   }
                   if ( datmcp_get_changed( peekmcp_gui, varmcp_red, st )) {
                       /* Изменился признак неисправности */
                       fail = datmcp_get_byte( varmcp_red, st );
                       cell[ ind ]->setFail( fail );
                   }
                   /* Состояние сети */

//                   if ( datmcp_get_changed( peekmcp_gui, varmcp_net0, st )) {
//                       if ( online != 0 ) {
//                           cell[ ind ]->setConnectState( 0, datmcp_get_byte( varmcp_net0, st ) ? LINK_FAILED : LINK_CONNECTED );
//                       }
//                   }
//                   if ( datmcp_get_changed( peekmcp_gui, varmcp_net1, st )) {
//                       if ( online != 0 ) {
//                           cell[ ind ]->setConnectState( 0, datmcp_get_byte( varmcp_net1, st ) ? LINK_FAILED : LINK_CONNECTED );
//                       }
//                   }




                   if ( ind == selected ) {
                       /* Изменилось состояние выбранного МИ */
                       DATMCP_VECTOR *d = datmcp_get_vector( st );
                       char s[ 256 ];
                       int col;

   #ifdef    LINK_UT_DATMCP
   #define __LINK_UT_DATMCP__
   #undef    LINK_UT_DATMCP
   #endif
                       switch ( online ) {
                       /* goto offline */
                       case 0:
                           lbMCPTitle->setStyleSheet( STYLE_TITLE_GRAY );


   /** \cond */

   #define DATMCP_THREAD( thread )
   #define DATMCP_TYPE( T, format, strto )
   #define DATMCP_VAR( var, type, title, string ) \
               if ( sizeof( title ) > 1 ) { \
                   tabMCPDatmcp->item( varmcp_##var, 0 )->setText( "-" ); \
                   tabMCPDatmcp->item( varmcp_##var, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
               }

   #define T( N, len, var, type, title, string ) \
               if ( sizeof( title ) > 1 ) { \
                   tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setText( "-" ); \
                   tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
               }

   #define DATMCP_ARRAY( len, var, type, title, string ) \
               UNFOLD( len, T, T, len, var, type, title, string )

   #include "datmcp-dict.h"

   #undef DATMCP_THREAD
   #undef DATMCP_TYPE
   #undef DATMCP_VAR
   #undef DATMCP_ARRAY
   #undef T

   /** \endcond */

                           break;

                       /* goto online */
                       case 1:
                           lbMCPTitle->setStyleSheet( STYLE_TITLE_GREEN );
                           datmcp_set_all_changed( peekmcp_gui, st );
                           /* fallthrough */

                       case _OLD_:
                           if ( datmcp_get_byte( varmcp_stateM, st ) == MCP_OFF ) break;

                           if ( fail != _OLD_ ) {
                               lbMCPTitle->setStyleSheet( fail ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
                           }
                           datmcp_lock();
   /** \cond */

   #define DATMCP_THREAD( thread )
   #define DATMCP_TYPE( T, format, strto )
   #define DATMCP_VAR( var, type, title, string ) \
          if ( sizeof( title ) > 1 ) { \
          col = COLOR_DEFAULT; \
          tabMCPDatmcp->item( varmcp_##var, 0 )->setText( string( s, d->var, &col )); \
          tabMCPDatmcp->item( varmcp_##var, 0 )->setForeground( QColor( col )); \
           }

   #define T( N, len, var, type, title, string ) \
           if ( sizeof( title ) > 1 ) { \
           col = COLOR_DEFAULT; \
           tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
           tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setForeground( QColor( col )); \
           }

   #define DATMCP_ARRAY( len, var, type, title, string ) \
           UNFOLD( len, T, T, len, var, type, title, string )

   #include "datmcp-dict.h"
   #undef DATMCP_THREAD
   #undef DATMCP_TYPE
   #undef DATMCP_VAR
   #undef DATMCP_ARRAY
   #undef T
   /** \endcond */

   datmcp_unlock();

             break;
       }

   #ifdef    LINK_UT_DATMCP
   #define __LINK_UT_DATMCP__
   #undef    LINK_UT_DATMCP
   #endif
                       /* Проверка изменения цвета серийного номера */
                       if ( datmcp_get_changed( peekmcp_gui, varmcp_col_sn, st )) {
                           tabMCPDatmcp->item( varmcp_sn, 0 )->setForeground( QColor( datmcp_get_dword( varmcp_col_sn, st )));
                       }
                       /* Проверка изменения цвета верификатора коррект. кодов */
                       if ( datmcp_get_changed( peekmcp_gui, varmcp_verify_c, st )) {
                           tabMCPDatmcp->item( varmcp_verify_c, 0 )->setForeground( QColor( datmcp_get_dword( varmcp_col_verify_c, st )));
                       }
                   }
               }
               /* Проверка изменения состояния тестов */
               if ( datmcp_get_changed( peekmcp_gui, varmcp_test_1, st ) // это тест мцп должен быть
                 //|| datm_get_changed( peekm_gui, varm_test_2, st )  tyt
                 || datmcp_get_changed( peekmcp_gui, varmcp_test_net, st )) {
                   if ( mcp_mask & BIT( st )) calcRunning3();   // tyt тест ми добавить
               }
               if ( !datmcp_check_updated( st )) {
                   /* Сброс признаков изменения */
                   datmcp_reset_all( peekmcp_gui, st );
               }
           }

           /* MI */


           abonent = st * MI_MAX_COUNT;
           for ( int i = 0; i < MI_MAX_COUNT; i++){

           if ( datm_get_changed_any(peekm_gui, abonent)){
               int onlineMi = _OLD_;
               int failMi = _OLD_;
               /* изменения */
               datm_clear_updated(abonent);
               ind = mi_list[abonent].ind;
               if ( datm_get_changed(peekm_gui, varm_state, abonent)){
                   uint8_t state = datm_get_byte(varm_state, abonent);
                   cell[ ind ]->setState( state );
                   onlineMi = 1;
                   switch ( state ) {

                   case MI_ON:
                   case MI_ON_RESET:
                       cell[ ind ]->setEnabled( true );
//                       if( datm_get_byte(varm_red, abonent)!=0){
//                           cell[ ind ]->setFail( true );
//                       }
                       break;

                   case MI_OFF:

                       onlineMi = 0;
                       /* Сброс значений */
                       datm_set_dword(varm_test_2, TEST_NONE, abonent);
                       datm_set_dword( varm_sn_mi, 0, abonent );
                       datm_set_dword( varm_col_sn, COLOR_DEFAULT, abonent );
                       datm_set_dword( varm_col_verify_c, COLOR_DEFAULT, abonent );
                       datm_set_byte( varm_red, 0, abonent);
                       /* fallthrough */

                   case MI_WORK:
                   case MI_WORK_RESET:
                       cell[ ind ]->setEnabled( false );
                       /* Сброс значений */
                       datm_set_byte( varm_test_2, TEST_NONE, abonent );
                       //datm_set_byte( varm_test_percent, 0, abonent );
                       datm_set_dword( varm_date_c, 0, abonent );
                       datm_set_dword( varm_time_c, 0, abonent );
                                              if( datm_get_byte(varm_red, abonent)!=0){
//                                                  cell[ ind ]->setFail( 1 );
                       cell[ind]->setFail(true);
                       cell[ind]->setState(1);
                                              }
                   }
               }


               if ( map[ ind ].kind == MAP_MI ) {
                   /* MI отображается на схеме */
//                   if ( datm_get_changed( peekm_gui, varm_test_percent, abonent )) {
//                       /* Изменились процент выполнения теста */
//                       char s[ 16 ];
//                       cell[ ind ]->setText( datm_get_string_byte( s, varm_test_percent, abonent ));
//                   }
                   if ( datm_get_changed( peekm_gui, varm_red, abonent )) {
                       /* Изменился признак неисправности */
                       failMi = datm_get_byte( varm_red, abonent );
                       cell[ ind ]->setFail( failMi );
                   }
                   /* Состояние сети */

                   if ( ind == selected ) {
                       /* Изменилось состояние выбранного МИ */
                       DATM_VECTOR *d = datm_get_vector( abonent );
                       char s[ 256 ];
                       int col;

   #ifdef    LINK_UT_DATM
   #define __LINK_UT_DATM__
   #undef    LINK_UT_DATM
   #endif
                       switch ( onlineMi ) {
                       /* goto offline */
                       case 0:
                           lbMeterTitle->setStyleSheet( STYLE_TITLE_GRAY );

   /** \cond */

   #define DATM_THREAD( thread )
   #define DATM_TYPE( T, format, strto )
   #define DATM_VAR( var, type, title, string ) \
               if ( sizeof( title ) > 1 ) { \
                   tabMeterDatm->item( varm_##var, 0 )->setText( "-" ); \
                   tabMeterDatm->item( varm_##var, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
               }

   #define T( N, len, var, type, title, string ) \
               if ( sizeof( title ) > 1 ) { \
                   tabMeterDat->item( varm_##var##_##N, 0 )->setText( "-" ); \
                   tabMeterDat->item( varm_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
               }

   #define DATM_ARRAY( len, var, type, title, string ) \
               UNFOLD( len, T, T, len, var, type, title, string )

   #include "datm-dict.h"

   #undef DATM_THREAD
   #undef DATM_TYPE
   #undef DATM_VAR
   #undef DATM_ARRAY
   #undef T

   /** \endcond */

                           break;

                       /* goto online */
                       case 1:
                           lbMeterTitle->setStyleSheet( STYLE_TITLE_GREEN );
                           datm_set_all_changed( peekm_gui, abonent );
                           /* fallthrough */

                       case _OLD_:
                           if ( datm_get_byte( varm_state, abonent ) == MI_OFF ) break;

                           if ( failMi != _OLD_ ) {
                               lbMeterTitle->setStyleSheet( failMi ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
                           }
                           datm_lock();
   /** \cond */

   #define DATM_THREAD( thread )
   #define DATM_TYPE( T, format, strto )
   #define DATM_VAR( var, type, title, string ) \
          if ( sizeof( title ) > 1 ) { \
          col = COLOR_DEFAULT; \
          tabMeterDatm->item( varm_##var, 0 )->setText( string( s, d->var, &col )); \
          tabMeterDatm->item( varm_##var, 0 )->setForeground( QColor( col )); \
           }

   #define T( N, len, var, type, title, string ) \
           if ( sizeof( title ) > 1 ) { \
           col = COLOR_DEFAULT; \
           tabMeterDatm->item( varm_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
           tabMeterDatm->item( varm_##var##_##N, 0 )->setForeground( QColor( col )); \
           }

   #define DATM_ARRAY( len, var, type, title, string ) \
           UNFOLD( len, T, T, len, var, type, title, string )

   #include "datm-dict.h"
   #undef DATM_THREAD
   #undef DATM_TYPE
   #undef DATM_VAR
   #undef DATM_ARRAY
   #undef T
   /** \endcond */

   datm_unlock();

                   break;
                   }

   #ifdef    LINK_UT_DATM
   #define __LINK_UT_DATM__
   #undef    LINK_UT_DATM
   #endif
                       /* Проверка изменения цвета серийного номера */
                       if ( datm_get_changed( peekm_gui, varm_col_sn, abonent )) {
                           tabMeterDatm->item( varm_sn_mi, 0 )->setForeground( QColor( datm_get_dword( varm_col_sn, abonent )));
                       }
                       /* Проверка изменения цвета верификатора коррект. кодов */
                       if ( datm_get_changed( peekm_gui, varm_verify_c, abonent )) {
                           tabMeterDatm->item( varm_verify_c, 0 )->setForeground( QColor( datm_get_dword( varm_col_verify_c, abonent )));
                       }
                   }
               }


               /* Проверка изменения состояния тестов */
               if
                (datm_get_changed( peekm_gui, varm_test_2, abonent )) {
                   if ( mi_mask & BIT( abonent )) {
                       calcRunning2();   // tyt тест ми добавить
               }}
               if ( !datm_check_updated( abonent )) {
                   /* Сброс признаков изменения */
                   datm_reset_all( peekm_gui, abonent );
               }
           }
           abonent++;
           }
           break;

    case EVENT_STAT2_RECEIVED ... EVENT_STAT2_RECEIVED + DATM_DEPTH - 1:
        /* Принята сетевая статистика */
        st = _type - EVENT_STAT2_RECEIVED;
        ind = mcp_list[st].ind;
        if (( ind == selected ) && ( map[ ind ].kind == MAP_MCP )) { // тут изменения
            NET_STAT *stat;
            stat = net_stat + st;
            pthread_mutex_lock( &test_net_mutex_result );
            for ( int i = 0; i < ( int ) NET_STAT_COUNT; i++ ) {
                for ( int n = 0; n < ini_net_count; n++){
                dword val = stat->data[ n ][ i ];
                tabMCPNet->item( ( i * ini_net_count ) + n, 0  )->setText(( val == _MAXDWORD ) ? "-" : QString::number( val ));
            }
          }
            pthread_mutex_unlock( &test_net_mutex_result );
        }
        break;

   // here
    case EVENT_SNMP_MESS:

        int state1=0;
        for (int i=0; i<ups_count; i++){

        if (upsParametrs2[i].upsBatSt != 0){
            state1 = TURN_ON;
        }
        else if (upsParametrs2[i].upsBatSt == 0){
            state1 = TURN_OFF;
        }

        lbUpsTitle[i]->setText( QString( "%1 - %2" ).arg( st_list_ex1[ upsParametrs2[i].upsInx ].name ).arg( map[ upsParametrs2[i].upsMap ].mark ));
          if(upsParametrs2[i].upsBatSt!=0){
              state1=TURN_ON;

          if(upsParametrs2[i].upsBatSt==1){
          tabUpsDatn[i]->item(0,0)->setText("Неизветсно");
          }
          else if(upsParametrs2[i].upsBatSt==2){
          tabUpsDatn[i]->item(0,0)->setText("Нормальное");
          }
          else if(upsParametrs2[i].upsBatSt==3){
          tabUpsDatn[i]->item(0,0)->setText("Низкий заряд");
          }
          else if(upsParametrs2[i].upsBatSt == 4){
          tabUpsDatn[i]->item(0,0)->setText("Нет батареи");
          }
          tabUpsDatn[i]->item(1,0)->setText(string_int(s,upsParametrs2[i].upsCap));
          tabUpsDatn[i]->item(2,0)->setText(string_int(s,upsParametrs2[i].upsloadP));
          tabUpsDatn[i]->item(3,0)->setText(string_int(s,upsParametrs2[i].upsinVolt));
           if (upsParametrs2[i].upsOutStat==1){
               tabUpsDatn[i]->item(4,0)->setText("Неизвестно");
           }
           else if (upsParametrs2[i].upsOutStat==2) {
               tabUpsDatn[i]->item(4,0)->setText("В сети");
           }
           else if (upsParametrs2[i].upsOutStat==3) {
               tabUpsDatn[i]->item(4,0)->setText("От батареи");
           }
           else if (upsParametrs2[i].upsOutStat==4) {
               tabUpsDatn[i]->item(4,0)->setText("onBoost");
           }
           else if (upsParametrs2[i].upsOutStat==5) {
               tabUpsDatn[i]->item(4,0)->setText("Сон");
           }
           else if (upsParametrs2[i].upsOutStat==6) {
               tabUpsDatn[i]->item(4,0)->setText("Выключен");
           }
           else if (upsParametrs2[i].upsOutStat==7) {
               tabUpsDatn[i]->item(4,0)->setText("Перезагрузка");
           }
           else if (upsParametrs2[i].upsOutStat==8) {
               tabUpsDatn[i]->item(4,0)->setText("onECO");
           }
           else if (upsParametrs2[i].upsOutStat==9) {
               tabUpsDatn[i]->item(4,0)->setText("onByPass");
           }
           else if (upsParametrs2[i].upsOutStat==10) {
               tabUpsDatn[i]->item(4,0)->setText("onBuck");
           }
           else if (upsParametrs2[i].upsOutStat==11) {
               tabUpsDatn[i]->item(4,0)->setText("Перегрузка");
           }

           if (upsParametrs2[i].upsBatSt==1 || upsParametrs2[i].upsBatSt == 4 || upsParametrs2[i].upsCap < 30 ||
                    upsParametrs2[i].upsinVolt<200 || upsParametrs2[i].upsinVolt>240 ||
                    upsParametrs2[i].upsOutStat==1 || upsParametrs2[i].upsOutStat==5 ||
                    upsParametrs2[i].upsOutStat==7 || upsParametrs2[i].upsOutStat==9){
                    lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_RED );
           }
           else if (upsParametrs2[i].upsBatSt==3 || (upsParametrs2[i].upsCap>=30 && upsParametrs2[i].upsCap<50) ||
                   //upsParametrs2[i].upsinVolt<200 || upsParametrs2[i].upsinVolt>240 ||
                   upsParametrs2[i].upsOutStat==3 || upsParametrs2[i].upsOutStat==6 ||
                   upsParametrs2[i].upsOutStat==11 ){
                   lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_YELLOW );
           }
           else {
               lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_GREEN );
           }
                 }
          else {



          lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_GRAY );
//#ifdef    LINK_UT_DATU
//#define __LINK_UT_DATU__
//#undef    LINK_UT_DATU
//#endif
		  
//          /** \cond */

//#define DATU_THREAD( thread )
//#define DATU_TYPE( T, format, strto )
//#define DATU_VAR( var, type, title, string ) \
//           tabUpsDatn[i]->item( varu_##var, 0 )->setText( "*" ); \
//           tabUpsDatn[i]->item( varu_##var, 0 )->setForeground( QColor( COLOR_DEFAULT ));

//#define T( N, len, var, type, title, string ) \
//            tabUpsDatn[i]->item( varu_##var##_##N, 0 )->setText( "-" ); \
//            tabUpsDatn[i]->item( varu_##var##_##N, 0 )->setForeground( QColor(COLOR_DEFAULT ));

//#define DATU_ARRAY( len, var, type, title, string ) \    source/xdo/xdo.c

//            UNFOLD( len, T, T, len, var, type, title, string )

//#include "datu-dict.h"

//#undef DATU_THREAD
//#undef DATU_TYPE
//#undef DATU_VAR
//#undef DATU_ARRAY
//#undef T

///** \endcond */
		  

          }
    }
        for (uint var = 0; var < ups_count; var++) {
            for( int var1 = 0; var1<map_count; var1++){
                    if (map[var1].ind == upsParametrs2[var].upsInx){


                        // gray
                        if (upsParametrs2[var].upsBatSt == 0){
                            //&& upsParametrs2[var].upsCap ==0  && upsParametrs2[var].upsloadP==0
                            //&& upsParametrs2[var].upsinVolt == 0 && upsParametrs2[var].upsOutStat==0 ){
                            cell[var1]->setState(0);
                            continue;
                        }
			
                        //red
                        else if (upsParametrs2[var].upsBatSt == 1 || upsParametrs2[var].upsBatSt == 4 || upsParametrs2[var].upsCap < 30  ||
                                //(upsParametrs2[var].upsinVolt<200 || upsParametrs2[var].upsinVolt>240)|| //вот эта фигня не работает, так же continue можно убрать
                                (upsParametrs2[var].upsOutStat==1 || upsParametrs2[var].upsOutStat==5
                                 || upsParametrs2[var].upsOutStat==7 || upsParametrs2[var].upsOutStat==9)){

                            cell[var1]->setFail(true);
                            cell[var1]->setState(1);
                            continue;
                        }
			
                        //yellow
                        else if (upsParametrs2[var].upsBatSt == 3 ||
                                (upsParametrs2[var].upsCap < 50 &&  upsParametrs2[var].upsCap > 30 ) ||
                                (upsParametrs2[var].upsOutStat==3 || upsParametrs2[var].upsOutStat==6 ||
                                 upsParametrs2[var].upsOutStat==11) || upsParametrs2[var].upsloadP<100 ){
                            cell[var1]->setState(161);
                            //cell[var1]->setEnabled(true);
                            continue;
                        }
			
                        //green
                        else if(upsParametrs2[var].upsBatSt == 2 && upsParametrs2[var].upsCap >= 50 &&
                                //(upsParametrs2[var].upsinVolt>200 && upsParametrs2[var].upsinVolt<240) &&
                (upsParametrs2[var].upsOutStat==2 || upsParametrs2[var].upsOutStat==4
                                || upsParametrs2[var].upsOutStat==8 || upsParametrs2[var].upsOutStat==10)
                                ){
                            cell[var1]->setState(8);
                            cell[var1]->setFail(false);
                            //cell[var1]->setEnabled(true);
                            //cell[var1]->setConnectState(0, 1);
                            continue;
                    }
                }
            }
    }
        break;
 }
//
    /* Считывание новых протоколов */
    if ( ini_prot_check_new && ( rzu_ind < rzu_count ) && idle ) {
        QProcess::startDetached( ini_prot_reader_check );
        ini_prot_check_new = false;
    }
}



void MainWindow::onMapClicked( int inx )
{
    int st;
    int state;
    NET_STAT *stat;
    uint8_t newCellKind;
    char arr[250];
    char * s = &arr[0];

    if (( inx >= map_count ) || ( inx == selected )) return;
//	selected = inx;
//	st = map[ selected ].ind;

    newCellKind = map[inx].kind;

    if ((newCellKind != MAP_ST)&&(newCellKind != MAP_MI)&&(newCellKind != MAP_BL)&&(newCellKind != MAP_UPS)&&(newCellKind!=MAP_MCP))
        return;

//	printf( "\nMAP: %d", inx );

    // Отмена предыдущего выбора
    if (selected < map_count)
      cell[selected]->setSelected(false);

    // Новый выбор
    selected = inx;
    st = map[ selected ].ind;
    cell[selected]->setSelected(true);

    switch ( map[ selected ].kind ) {

    case MAP_MCP:
        pgTopRight->setCurrentIndex(4);
        state = datmcp_get_byte(varmcp_stateM, st);
        lbMCPTitle->setText( QString( "%1 - %2" ).arg( mcp_list[ st ].name ).arg( map[ selected ].mark ));
        lbMCPTitle->setStyleSheet( STYLE_TITLE_GRAY );

#ifdef    LINK_UT_DATMCP
#define __LINK_UT_DATMCP__
#undef    LINK_UT_DATMCP
#endif
            /* Состояние */
                 if (state == TURN_OFF){


            lbMCPTitle->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATMCP_THREAD( thread )
#define DATMCP_TYPE( T, format, strto )
#define DATMCP_VAR( var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMCPDatmcp->item( varmcp_##var, 0 )->setText( "-" ); \
                tabMCPDatmcp->item( varmcp_##var, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
            }

#define T( N, len, var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setText( "-" ); \
                tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
            }

#define DATMCP_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datmcp-dict.h"

#undef DATMCP_THREAD
#undef DATMCP_TYPE
#undef DATMCP_VAR
#undef DATMCP_ARRAY
#undef T

/** \endcond */
            }else{

                 int col;
                 char s[256];
                 DATMCP_VECTOR *d = datmcp_get_vector(st);
                 lbMCPTitle->setStyleSheet( datmcp_get_byte( varmcp_red, st ) ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
                 datmcp_lock();

      /** \cond */

    #define DATMCP_THREAD( thread )
    #define DATMCP_TYPE( T, format, strto )
    #define DATMCP_VAR( var, type, title, string ) \
           if ( sizeof( title ) > 1 ) { \
              col = COLOR_DEFAULT; \
              tabMCPDatmcp->item( varmcp_##var, 0 )->setText( string( s, d->var, &col )); \
              tabMCPDatmcp->item( varmcp_##var, 0 )->setForeground( QColor( col )); \
           }

           #define T( N, len, var, type, title, string ) \
           if ( sizeof( title ) > 1 ) { \
               col = COLOR_DEFAULT; \
               tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
               tabMCPDatmcp->item( varmcp_##var##_##N, 0 )->setForeground( QColor( col )); \
           }

          #define DATM_ARRAY( len, var, type, title, string ) \
              UNFOLD( len, T, T, len, var, type, title, string )

            #include "datmcp-dict.h"
          #undef DATMCP_THREAD
          #undef DATMCP_TYPE
          #undef DATMCP_VAR
          #undef DATMCP_ARRAY
          #undef T
          /** \endcond */

                 datmcp_unlock();
  /* Цвет серийного номера */
tabMCPDatmcp->item( varmcp_sn, 0 )->setForeground( QColor( datmcp_get_dword( varmcp_col_sn, st )));
  /* Цвет верификатора коррект. кодов */
tabMCPDatmcp->item( varmcp_verify_c, 0 )->setForeground( QColor( datmcp_get_dword( varmcp_col_verify_c, st )));
                 }

#ifdef __LINK_UT_DATMCP__
#define  LINK_UT_DATMCP
#undef __LINK_UT_DATMCP__
#endif

                 stat = net_stat + st;   // было MI_ID_MIN
                 pthread_mutex_lock( &test_net_mutex_result );
                 for ( int i = 0; i < ( int ) NET_STAT_COUNT; i++ ) {
                 for ( int n = 0; n < ini_net_count; n++){
                     dword val = stat->data[ n ][ i ];
                     tabMCPNet->item( ( i * ini_net_count ) + n, 0  )->setText(( val == _MAXDWORD ) ? "-" : QString::number( val ));

                    }
                 }
                 pthread_mutex_unlock( &test_net_mutex_result);
        /* Журнал */
        for ( int i = 0; i < st_count; i++ ) {
            if ( st_list[ i ].TypeMRMO == MCP_TYPE ) {
                lstJournal[ i ]->setVisible( i-4==st );
            }
        }
        break;


     case MAP_UPS:
        //snmp_button();
        pgTopRight->setCurrentIndex( 3 );
        if(upsParametrs2.size()==0){        // если кнопка не нажата, подготовить вектор ups
        for ( int i = 0; i < map_count; i++ ){
        if (map[i].kind==MAP_UPS){

            upsParametrs2.push_back({0,0,0,0,0,0,map[i].ind,i});

            }
        }

        for (int i=0; i<ups_count; i++){
            //if(upsParametrs2[i].upsInx==st){
                oidznachint=upsParametrs2[i].upsBatSt;
                oidznachint1=upsParametrs2[i].upsCap;
                oidznachint2=upsParametrs2[i].upsloadP;
                oidznachint3=upsParametrs2[i].upsinVolt;
                oidznachint4=upsParametrs2[i].upsOutStat;
                oidznachint5=upsParametrs2[i].upsInx;

            //}
          if(oidznachint!=0){
              state=TURN_ON;
          }
          else if (oidznachint == 0){
             state=TURN_OFF;
          }
//          else {
//              state=TURN_OFF;
//          }

        //state = 0;//datn_get_byte( varn_state, st );
        /* Заголовок */

        lbUpsTitle[i]->setText( QString( "%1 - %2" ).arg( st_list_ex1[ upsParametrs2[i].upsInx ].name ).arg( map[ upsParametrs2[i].upsMap].mark ));

#ifdef    LINK_UT_DATU
#define __LINK_UT_DATU__
#undef    LINK_UT_DATU
#endif
        /* Состояние */
        lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_GRAY );
        if ( state == TURN_OFF ) {
            lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_GRAY );

/** \cond */

#define DATU_THREAD( thread )
#define DATU_TYPE( T, format, strto )
#define DATU_VAR( var, type, title, string ) \
           tabUpsDatn[i]->item( varu_##var, 0 )->setText( "*" ); \
           tabUpsDatn[i]->item( varu_##var, 0 )->setForeground( QColor( COLOR_DEFAULT ));

#define T( N, len, var, type, title, string ) \
            tabUpsDatn[i]->item( varu_##var##_##N, 0 )->setText( "-" ); \
            tabUpsDatn[i]->item( varu_##var##_##N, 0 )->setForeground( QColor(COLOR_DEFAULT ));

#define DATU_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datu-dict.h"

#undef DATU_THREAD
#undef DATU_TYPE
#undef DATU_VAR
#undef DATU_ARRAY
#undef T

/** \endcond */
        } else {

            //int col;
            char s[ 256 ];
            //DATU_VECTOR *d = datu_get_vector( st );

//            lbStationTitle->setStyleSheet(
//                ( datu_get_word( varu_failure, st ) || ( data_get_dword( var_onelink_mask ) & BIT( st ))) ?
//                STYLE_TITLE_RED : STYLE_TITLE_GREEN
//                                         );

//datn_lock();

            if(oidznachint==1){
            tabUpsDatn[i]->item(0,0)->setText("Неизветсно");
            }
            else if(oidznachint==2){
            tabUpsDatn[i]->item(0,0)->setText("Нормальное");
            }
            else if(oidznachint==3){
            tabUpsDatn[i]->item(0,0)->setText("Низкий заряд");
            }
            else if(oidznachint == 4){
            tabUpsDatn[i]->item(0,0)->setText("Нет батареи");
            }
            tabUpsDatn[i]->item(1,0)->setText(string_int(s,oidznachint1));
            tabUpsDatn[i]->item(2,0)->setText(string_int(s,oidznachint2));
            tabUpsDatn[i]->item(3,0)->setText(string_int(s,oidznachint3));
             if (oidznachint4==1){
                 tabUpsDatn[i]->item(4,0)->setText("Неизвестно");
             }
             else if (oidznachint4==2) {
                 tabUpsDatn[i]->item(4,0)->setText("В сети");
             }
             else if (oidznachint4==3) {
                 tabUpsDatn[i]->item(4,0)->setText("От батареи");
             }
             else if (oidznachint4==4) {
                 tabUpsDatn[i]->item(4,0)->setText("onBoost");
             }
             else if (oidznachint4==5) {
                 tabUpsDatn[i]->item(4,0)->setText("Сон");
             }
             else if (oidznachint4==6) {
                 tabUpsDatn[i]->item(4,0)->setText("Выключен");
             }
             else if (oidznachint4==7) {
                 tabUpsDatn[i]->item(4,0)->setText("Перезагрузка");
             }
             else if (oidznachint4==8) {
                 tabUpsDatn[i]->item(4,0)->setText("onECO");
             }
             else if (oidznachint4==9) {
                 tabUpsDatn[i]->item(4,0)->setText("onByPass");
             }
             else if (oidznachint4==10) {
                 tabUpsDatn[i]->item(4,0)->setText("onBuck");
             }
             else if (oidznachint4==11) {
                 tabUpsDatn[i]->item(4,0)->setText("Перегрузка");
             }

             if (oidznachint==1 || oidznachint == 4 || oidznachint1 < 30 ||
                      oidznachint2<200 || oidznachint2>240 ||
                      oidznachint4==1 || oidznachint4==5 ||
                      oidznachint4==7 || oidznachint4==9){
                      lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_RED );
             }
             else if (oidznachint==3 || (oidznachint1>30 && oidznachint1<50) ||
                     //oidznachint2<200 || oidznachint2>240 ||
                     oidznachint4==3 || oidznachint4==6 ||
                     oidznachint4==11 ){
                     lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_YELLOW );
             }
             else {
                 lbUpsTitle[i]->setStyleSheet( STYLE_TITLE_GREEN );
             }

             //tabUpsDatn->item(4,0)->setForeground(QColor(COLOR_PROBLEM));

//** \cond */

//#define DATU_THREAD( thread )
//#define DATU_TYPE( T, format, strto )
//#define DATU_VAR( var, type, title, string ) \
//            col = COLOR_DEFAULT; \
//            tabUpsDatn->item( varu_##var, 0 )->setText( string( s, d->var, &col )); \
//            tabUpsDatn->item( varu_##var, 0 )->setForeground( QColor( col ));

//#define T( N, len, var, type, title, string ) \
//            col = COLOR_DEFAULT; \
//            tabUpsdatn->item( varu_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
//            tabUpsdatn->item( varu_##var##_##N, 0 )->setForeground( QColor( col ));

//#define DATU_ARRAY( len, var, type, title, string ) \
//            UNFOLD( len, T, T, len, var, type, title, string )

//#include "datu-dict.h"

//#undef DATU_THREAD
//#undef DATU_TYPE
//#undef DATU_VAR
//#undef DATU_ARRAY
//#undef T

//** \endcond */

 }           //datu_unlock();
}
}


#ifdef __LINK_UT_DATU__
#define  LINK_UT_DATU
#undef __LINK_UT_DATU__
#endif

        /* Сетевая статистика */
        stat = net_stat + st;
        pthread_mutex_lock( &test_net_mutex_result );

        for ( int i = 0; i < ( int ) NET_STAT_COUNT; i++ ) {
            for ( int n = 0; n < ini_net_count; n++ ) {
                dword val = stat->data[ n ][ i ];
                tabStationNet->item(( i * ini_net_count ) + n, 0 )->setText(
                    ( val == _MAXDWORD ) ? "-" : QString::number( val )
                );
            }
        }
        pthread_mutex_unlock( &test_net_mutex_result );

//        /* Журнал */
//        for ( int i = 0; i < st_count; i++ ) {
//            if ( st_list[ i ].TypeMRMO != MI_TYPE ) lstJournal[ i ]->setVisible( i == st );
//        }
        break;

    case MAP_ST:
        pgTopRight->setCurrentIndex( 0 );

        state = datn_get_byte( varn_state, st );
        /* Заголовок */
        lbStationTitle->setText( QString( "%1 - %2" ).arg( st_list_ex[ st ].name ).arg( map[ selected ].mark ));

#ifdef    LINK_UT_DATN
#define __LINK_UT_DATN__
#undef    LINK_UT_DATN
#endif

        /* Состояние */
        if ( state == TURN_OFF ) {
            lbStationTitle->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
            tabStationDatn->item( varn_##var, 0 )->setText( "-" ); \
            tabStationDatn->item( varn_##var, 0 )->setForeground( QColor( COLOR_DEFAULT ));

#define T( N, len, var, type, title, string ) \
            tabStationDatn->item( varn_##var##_##N, 0 )->setText( "-" ); \
            tabStationDatn->item( varn_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT ));

#define DATN_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */
        } else {
            int col;
            char s[ 256 ];
            DATN_VECTOR *d = datn_get_vector( st );

            lbStationTitle->setStyleSheet(
                ( datn_get_word( varn_failure, st ) || ( data_get_dword( var_onelink_mask ) & BIT( st ))) ?
                STYLE_TITLE_RED : STYLE_TITLE_GREEN
            );
            datn_lock();
/** \cond */

#define DATN_THREAD( thread )
#define DATN_TYPE( T, format, strto )
#define DATN_VAR( var, type, title, string ) \
            col = COLOR_DEFAULT; \
            tabStationDatn->item( varn_##var, 0 )->setText( string( s, d->var, &col )); \
            tabStationDatn->item( varn_##var, 0 )->setForeground( QColor( col ));

#define T( N, len, var, type, title, string ) \
            col = COLOR_DEFAULT; \
            tabStationDatn->item( varn_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
            tabStationDatn->item( varn_##var##_##N, 0 )->setForeground( QColor( col ));

#define DATN_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datn-dict.h"

#undef DATN_THREAD
#undef DATN_TYPE
#undef DATN_VAR
#undef DATN_ARRAY
#undef T

/** \endcond */
            datn_unlock();
        }

#ifdef __LINK_UT_DATN__
#define  LINK_UT_DATN
#undef __LINK_UT_DATN__
#endif

        /* Сетевая статистика */
        stat = net_stat + st;
        pthread_mutex_lock( &test_net_mutex_result );

        for ( int i = 0; i < ( int ) NET_STAT_COUNT; i++ ) {
            for ( int n = 0; n < ini_net_count; n++ ) {
                dword val = stat->data[ n ][ i ];
                tabStationNet->item(( i * ini_net_count ) + n, 0 )->setText(
                    ( val == _MAXDWORD ) ? "-" : QString::number( val )
                );
            }
        }
        pthread_mutex_unlock( &test_net_mutex_result );

        /* Журнал */
        for ( int i = 0; i < st_count; i++ ) {
            if ( st_list[ i ].TypeMRMO != MI_TYPE ) lstJournal[ i ]->setVisible( i == st );
        }
        break;

    case MAP_MI:
        pgTopRight->setCurrentIndex( 2 );

        state = datm_get_byte( varm_state, st );
        /* Заголовок */
        lbMeterTitle->setText( QString( "%1 - %2" ).arg( mi_list[ st ].name ).arg( map[ selected ].mark ));

#ifdef    LINK_UT_DATM
#define __LINK_UT_DATM__
#undef    LINK_UT_DATM
#endif

        /* Состояние */
        if ( state == MI_OFF ) {
            lbMeterTitle->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMeterDatm->item( varm_##var, 0 )->setText( "-" ); \
                tabMeterDatm->item( varm_##var, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
            }

#define T( N, len, var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                tabMeterDatm->item( varm_##var##_##N, 0 )->setText( "-" ); \
                tabMeterDatm->item( varm_##var##_##N, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
            }

#define DATM_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

/** \endcond */
        } else {
            int col;
            char s[ 256 ];
            DATM_VECTOR *d = datm_get_vector( st );

            lbMeterTitle->setStyleSheet( datm_get_byte( varm_red, st ) ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );

            datm_lock();
/** \cond */

#define DATM_THREAD( thread )
#define DATM_TYPE( T, format, strto )
#define DATM_VAR( var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                col = COLOR_DEFAULT; \
                tabMeterDatm->item( varm_##var, 0 )->setText( string( s, d->var, &col )); \
                tabMeterDatm->item( varm_##var, 0 )->setForeground( QColor( col )); \
            }

#define T( N, len, var, type, title, string ) \
            if ( sizeof( title ) > 1 ) { \
                col = COLOR_DEFAULT; \
                tabMeterDatm->item( varm_##var##_##N, 0 )->setText( string( s, d->var[ N ], &col )); \
                tabMeterDatm->item( varm_##var##_##N, 0 )->setForeground( QColor( col )); \
            }

#define DATM_ARRAY( len, var, type, title, string ) \
            UNFOLD( len, T, T, len, var, type, title, string )

#include "datm-dict.h"

#undef DATM_THREAD
#undef DATM_TYPE
#undef DATM_VAR
#undef DATM_ARRAY
#undef T

/** \endcond */
            datm_unlock();

            /* Цвет серийного номера */
            tabMeterDatm->item( varm_sn_mi, 0 )->setForeground( QColor( datm_get_dword( varm_col_sn, st )));
            /* Цвет верификатора коррект. кодов */
            tabMeterDatm->item( varm_verify_c, 0 )->setForeground( QColor( datm_get_dword( varm_col_verify_c, st )));
        }

#ifdef __LINK_UT_DATM__
#define  LINK_UT_DATM
#undef __LINK_UT_DATM__
#endif


//		/* Журнал */
//		for ( int i = 0; i < st_count; i++ ) {
//			if ( st_list[ i ].TypeMRMO == MI_TYPE ) lstJournal[ i ]->setVisible( i == st + MI_ID_MIN );
//		}
        break;

    case MAP_BL:
        pgTopRight->setCurrentIndex( 1 );

        for ( uint8_t ch = 0; ch < BL_CH_COUNT; ch++ ) {
            int col;
            uint8_t l = 0;
            valb_t state = datb_get( varb_state, st, ch, 0 );

            /* Заголовок */
            lbBlockTitle[ ch ]->setText( QString( "%1 К%2 - %3 - %4" ).arg( bl_list[ st ].name ).arg( ch + 1 )
            .arg( map[ selected ].mark ).arg( bl_list[ st ].addr ));
            if ( state == REGWORK_VU_OFF ) {
                // Блок не в сети

                lbBlockTitle[ ch ]->setStyleSheet( STYLE_TITLE_GRAY );
/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
                tabBlockDatb[ ch ]->item( l, 0 )->setText( "-" ); \
                tabBlockDatb[ ch ]->item( l, 0 )->setForeground( QColor( COLOR_DEFAULT )); \
                tabBlockDatb[ ch ]->item( l++, 2 )->setText( "" );

#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */
            } else {
                // Блок в сети
                
                lbBlockTitle[ ch ]->setStyleSheet( datb_get( varb_red, st, ch, 0 ) ? STYLE_TITLE_RED : STYLE_TITLE_GREEN );
/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
                col = COLOR_DEFAULT; \
                tabBlockDatb[ ch ]->item( l, 0 )->setText( datb_get_string( varb_##var, st, ch, &col )); \
                tabBlockDatb[ ch ]->item( l, 0 )->setForeground( QColor( col )); \
                tabBlockDatb[ ch ]->item( l++, 2 )->setText( datb_get_host_string( varb_##var, st, ch ));

#define DATB_VAR3( var, pos, mask, offset, vote )

#include "datb-dict.h"

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */

                // Вывод строки с CRC

                int flagErrorCRC = datb_get(varb_chsum, st, ch, 0);
                if (flagErrorCRC){
                    //int iCRC = datb_get(varb_chsum, st, ch, 0);
                    int iCRC = bl_chsum[ st ][ ch ].app;
        //printf( "\n-----------------\nblock %d CRC: %x", st, iCRC );
                    sprintf(s, "%08X", iCRC);
                    QString sInfo = QString("Контрольная сумма ПО: %1").arg(s);
                    tabBlockDatb[ ch ]->item( 2, 1 )->setText(sInfo);
                }
                else tabBlockDatb[ ch ]->item( 2, 1 )->setText(tr("Контрольная сумма"));

            }
        }
        break;
    }
}

void MainWindow::onTestClicked( int id )
{
    uint16_t par = 0;

    if ( id == TEST_UPS){

            snmp_button();
            return;
    }

    if ( data_get_byte( var_operate )) return;

    if ( id < PACK_BASE ) {

        switch ( id ) {
        case TEST_MCP_1:
            handleTest3( id, edRepValue->value(), par );
            break;
        case TEST_FULL:
            handleTest3( 0, edRepValue->value(), par );
            break;
        case TEST_MI_2:
            handleTest2( id, edRepValue->value(), par );
            break;

        case TEST_HDD:
        case TEST_HDD_EXP:
            par = ( uint16_t ) ini_test_hdd_mb;
            goto L_HANDLE_TEST;

        case TEST_CMOS:
            par = ( uint16_t ) ini_test_timer_sec;
            goto L_HANDLE_TEST;

        case TEST_KBD:
        case TEST_RAM_EXP:
        case TEST_RAM:
        case TEST_PIC:
        case TEST_MOUSE:
        case TEST_SPEAKER:
        case TEST_VIDEO_EXP:
        case TEST_VIDEO:
        L_HANDLE_TEST:
            handleTest( id, edRepValue->value(), par );
            break;

        case TEST_ETHERNET:
            if ( idle || data_get_byte( var_test_net_command )) {
                if ( !dlgTestNet ) dlgTestNet = new TestNetDialog( this );

                dlgTestNet->show();
            }
            break;


        }
    }
    else {
        handlePack( id - PACK_BASE);
    }

}

void MainWindow::onCtrlClicked( int id )
{
    if (( id < 0 ) || ( id >= ( int ) CTRL_BUTTON_COUNT )) return;

    switch ( id ) {
    case CTRL_TIME:
        if ( !idle || !idle2 ) break;
        if ( rzu_ind >= rzu_count ) break;

        if ( !dlgClock ) dlgClock = new ClockDialog( this );

        dlgClock->show();
        break;

    case CTRL_START:
    case CTRL_REPAIR:

        if ( !idle || !idle2 ) break;
        if ( rzu_ind >= rzu_count ) break;

        if ( !dlgStartRZU ) dlgStartRZU = new StartRZUDialog( this );

        dlgStartRZU->setRZU(( id == CTRL_START ) ? NEW_WORK : CONTINUE_WORK );
        dlgStartRZU->show();
        break;

    case CTRL_PROT:
        QProcess::startDetached( ini_prot_reader );
        break;
    }
}

void MainWindow::onPrepClicked( int id )
{
    if (( id < 0 ) || ( id >= ( int ) PREP_BUTTON_COUNT )) return;

    switch ( id ) {

    case PREP_TD_CHECK:
    case PREP_TD_LOAD:
        handleDisk( id );
        break;

    case PREP_ST_NAME:
        handleSetName();
        break;

    case PREP_ST_REBOOT:
        if ( !idle ) break;

        if ( QMessageBox::question( this, M_DLG_REBOOT, M_DLG_REBOOT_MSG
        , QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {
            /* Перезагрузка ВУ */
            if ( data_get_byte( var_operate ) == 0 ) {
                message_transmit( SEND_ALL, CMD_EX, st_mask, CMD_REBOOT );
            }
        }
        break;

    case PREP_TST_CLEAR:
        if ( !idle ) break;

        if ( QMessageBox::question( this, M_DLG_JOURNAL_CLEAR, M_DLG_JOURNAL_CLEAR_MSG
        , QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {
            /* Очистка журеала и лога */
            clearJournal();
            clearLog();
        }
        break;

    case PREP_MI_ID:
    case PREP_MI_GRAD:
        if ( !idle2 || !idle3 ) break;

        handleMeter( id );
        break;

    case PREP_MI_REPLACE:
        if ( !idle2 ) break;

        handleMeterReplace();
        break;

    case PREP_BL_ADDR:
        handleBlockAddr();
        break;

    case PREP_BL_PROG:
        handleBlockProg();
        break;

    case PREP_BL_INIT:
        handleBlockInit();
        break;

    case PREP_BL_RESERVE:
        handleBlockReserve();
        break;
    }
}

void MainWindow::onClearAllClicked( void )
{
    for ( int i = 0; i < cell.size(); i++ ) cell[ i ]->uncheck();
}

void MainWindow::onCheckAllClicked( void )
{
    for ( int i = 0; i < cell.size(); i++ ) cell[ i ]->check();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MainWindow::setupPack( void )
{
    for ( int  i = 0; i < PACK_COUNT; i++ ) {
        if ( pack_title[ i + 1 ]) {
            int l = strlen( pack_title[ i + 1 ]);

            M_TEST[ PACK_FIRST + i ] = pack_title[ i + 1 ];
            M_TEST_STOP[ PACK_FIRST + i ] = ( char *) malloc( l + strlen( M_STOP ) + 1 );
            strcpy(( char *) M_TEST_STOP[ PACK_FIRST + i ], M_TEST[ PACK_FIRST + i ]);
            strcpy(( char *) M_TEST_STOP[ PACK_FIRST + i ] + l, M_STOP );
        }
    }
}

void MainWindow::setMask( uint8_t st, int state )
{
    if ( state ) {
        SET_VAR( st_mask, BIT( st ));
    } else {
        CLEAR_VAR( st_mask, BIT( st ));
    }
    calcRunning();
}

void MainWindow::setMask2( uint8_t st, int state )
{
    if ( state ) {
        SET_VAR( mi_mask, BIT( st ));
    } else {
        CLEAR_VAR( mi_mask, BIT( st ));
    }
    calcRunning2();
}

void MainWindow::setMask3( uint8_t st, int state )
{
    if ( state ) {
        SET_VAR( mcp_mask, BIT( st ));
    } else {
        CLEAR_VAR( mcp_mask, BIT( st ));
    }
    calcRunning3();
}

 // !!!
 // тут нужно будет сделать под mcp или не нужно (посмотрим)
 // !!!

void MainWindow::calcRunning( void )
{
    uint32_t t = testRunning;
    uint32_t p = packRunning;

    testRunning = 0;
    packRunning = 0;

    for ( int i = 0; i < st_count; i++ ) {
        if ( st_mask & BIT( i )) {
            word perform = datn_get_word( varn_test_perform, i );
            testRunning |= BIT( PERFORM_GET( perform, TEST ));
            packRunning |= BIT( PERFORM_GET( perform, PACK ));
        }
    }
    idle = ( testRunning == 1 ) && ( packRunning == 1 ) && ( data_get_byte( var_td_operate ) == TD_NONE );

    if ( testRunning != t ) {
        for ( int i = TEST__FIRST_; i <= TEST__LAST_; i++ ) {
            setTestButtonRunning( i, testRunning & BIT( i ));
        }
    }
    if ( packRunning != p ) {
        for ( int i = 1; i < PACK_COUNT + 1; i++ ) {
            setTestButtonRunning( PACK_BASE + i, packRunning & BIT( i ));
        }
    }
}

void MainWindow::calcRunning2( void )
{
    uint32_t t = testRunning2;

    testRunning2 = 0;

    for ( int i = 0; i < mi_count; i++ ) {
        if ( mi_mask & BIT( i )) {
            //if ( datmcp_get_byte( varmcp_test_1, i ) == TEST_PERFORM ) testRunning2 |= BIT( TEST_FULL );
            if ( datm_get_byte( varm_test_2, i ) == TEST_PERFORM ) testRunning2 |= BIT( TEST_MI_2 );
            //if ( datmcp_get_byte( varmcp_test_1, i ) == TEST_PERFORM ) testRunning2 |= BIT( TEST_MCP_1 );

        }
    }

    idle2 = ( !testRunning2 );
    if ( testRunning2 != t ) {
        //setTestButtonRunning( TEST_FULL, testRunning2 & BIT( TEST_FULL ));
        setTestButtonRunning( TEST_MI_2, testRunning2 & BIT( TEST_MI_2 ));
        //setTestButtonRunning( TEST_MCP_1, testRunning2 & BIT( TEST_MCP_1 ));

    }
}

void MainWindow::calcRunning3( void )
{
    uint32_t t = testRunning3;

    testRunning3 = 0;

    for ( int i = 0; i < mcp_count; i++ ) {
        if ( mcp_mask & BIT( i )) {
            if ( datmcp_get_byte( varmcp_test_1, i ) == TEST_PERFORM ) testRunning3 |= BIT( TEST_MCP_1 );
            //if ( datm_get_byte( varm_test_2, i ) == TEST_PERFORM ) testRunning3 |= BIT( TEST_MI_2 );
            if ( datmcp_get_byte( varmcp_test_1, i ) == TEST_PERFORM ) testRunning3 |= BIT( TEST_FULL);
        }
    }
    idle3 = ( !testRunning3 );

    if ( testRunning3 != t ) {
        setTestButtonRunning( TEST_MCP_1, testRunning3 & BIT( TEST_MCP_1 ));
        //setTestButtonRunning( TEST_MI_2, testRunning3 & BIT( TEST_MI_2 ));
        setTestButtonRunning( TEST_FULL, testRunning3 & BIT( TEST_FULL ));

    }
}


void MainWindow::updatePageTitleColor( uint8_t page )
{
    QColor col = Qt::black;

    if ( page >= page_count ) return;

    for ( int i = 0; i < cell.size(); i++ ) {
        if ( cell[ i ]->page < page ) continue;
        if ( cell[ i ]->page > page ) break;

        if ( cell[ i ]->getFail()) {
            col = Qt::red;
            break;
        }
    }
    pgMap->tabBar()->setTabTextColor( page, col );
}

void MainWindow::startTestMessage( uint16_t test, uint16_t rep, uint16_t par )
{
//	unsigned inx = getTestButtonInx( test );
//	char s[ 32 ];
//	QString t = M_TEST[ inx ];
//	printf( "\nSTART: %s %s", encode_console( s, &t, sizeof( s )), ( EXPRESS_TEST[ inx ]) ? "(E)" : "" );

    cmd_test.wTestNum = test;
    cmd_test.wRepeat = rep;
    cmd_test.wParam[ 0 ] = par;

    message_transmit( SEND_ALL, CMD_EX, st_mask, EX_START_TEST, sizeof( TEST ), &cmd_test );
}

void MainWindow::stopTestMessage( uint16_t test )
{
//	unsigned inx = getTestButtonInx( test );
//	char s[ 32 ];
//	QString t = M_TEST[ inx ];
//	printf( "\nSTOP: %s %s", encode_console( s, &t, sizeof( s )), ( EXPRESS_TEST[ inx ]) ? "(E)" : "" );

    message_transmit( SEND_ALL, CMD_EX, st_mask, EX_STOP_TEST, sizeof( word ), &test );
}

void MainWindow::startTestMessage2( uint16_t test, uint16_t rep, uint16_t par )
{
    cmd_test.wTestNum = test;
    cmd_test.wRepeat = rep;
    cmd_test.wParam[ 0 ] = par;

    for (int i = 0; i < MCP_COUNT; i++){
       uint32_t MCP = BIT(SEND_MCP1 + i);
       uint32_t MI  = (mi_mask >> (MI_MAX_COUNT * i)) & 0xFF;
       if ( (mcp_mask & BIT(i)) && MI )
         message_transmit2( SEND_MCP1 + i, CMD_EX, (MI << 8) | MCP, EX_START_TEST_MI, sizeof( TEST ), &cmd_test );
    }
}

void MainWindow::stopTestMessage2( uint16_t test )
{
    cmd_test.wTestNum = test;
    cmd_test.wRepeat = 0;
    cmd_test.wParam[ 0 ] = 0;

    for (int i = 0; i < MCP_COUNT; i++){
       uint32_t MCP = BIT(SEND_MCP1 + i);
       uint32_t MI  = (mi_mask >> (MI_MAX_COUNT * i)) & 0xFF;
       if ( (mcp_mask & BIT(i)) && MI )
         message_transmit2( SEND_MCP1 + i, CMD_EX, (MI << 8) | MCP, EX_STOP_TEST_MI, sizeof( TEST ), &cmd_test );
    }
}

void MainWindow::startTestMessage3( uint16_t test, uint16_t rep, uint16_t par )
{
    cmd_test.wTestNum = test;
    cmd_test.wRepeat = rep;
    cmd_test.wParam[ 0 ] = par;

    //message_transmit2( SEND_BI, CMD_EX, (mcp_mask << SEND_MCP1),
    //                   EX_START_TEST_MI, sizeof( TEST ), &cmd_test );

    for (int i = 0; i < MCP_COUNT; i++){
       uint32_t MCP = BIT(SEND_MCP1 + i);
       uint32_t MI  = (mi_mask >> (MI_MAX_COUNT * i)) & 0xFF;
       if ( mcp_mask & BIT(i) )
         message_transmit2( SEND_MCP1 + i, CMD_EX, (MI << 8) | MCP, EX_START_TEST_MI, sizeof( TEST ), &cmd_test );
    }
}

void MainWindow::stopTestMessage3( uint16_t test )
{
    cmd_test.wTestNum = test;
    cmd_test.wRepeat = 0;
    cmd_test.wParam[ 0 ] = 0;

    for (int i = 0; i < MCP_COUNT; i++){
       uint32_t MCP = BIT(SEND_MCP1 + i);
       uint32_t MI  = (mi_mask >> (MI_MAX_COUNT * i)) & 0xFF;
       if ( mcp_mask & BIT(i) )
         message_transmit2( SEND_MCP1 + i, CMD_EX, (MI << 8) | MCP, EX_STOP_TEST_MI, sizeof( TEST ), &cmd_test );
    }
}

void MainWindow::handleTest( uint8_t test, uint16_t rep, uint16_t par )
{
    if ( testRunning & BIT( test )) {
        /* Останов теста */
        stopTestMessage( test );
    } else {
        /* Запуск теста */
        if ( idle ) startTestMessage( test, rep, par );
    }
}

void MainWindow::handleTest2( uint8_t test, uint16_t rep, uint16_t par )
{
    if ( testRunning2 & BIT( test )) {
        /* Останов теста */
        stopTestMessage2( test );
    } else {
        /* Запуск теста */
        if ( idle2 || idle3 ) startTestMessage2( test, rep, par );
    }
}

void MainWindow::handleTest3( uint8_t test, uint16_t rep, uint16_t par )
{
    if ( testRunning3 & BIT( test )) {
        /* Останов теста */
        stopTestMessage3( test );
    } else {
        /* Запуск теста */
        if ( idle3 || idle2 ) startTestMessage3( test, rep, par );
    }
}

void MainWindow::startPackMessage( uint16_t pack )
{
//	unsigned inx = getTestButtonInx( PACK_BASE + pack );
//	char s[ 32 ];
//	QString t = M_TEST[ inx ];
//	printf( "\nSTART: %s", encode_console( s, &t, sizeof( s )));

    message_transmit( SEND_ALL, CMD_EX, st_mask, EX_START_PACK, sizeof( PACK_TEST ), pack_list + pack - 1 );
}

void MainWindow::stopPackMessage( void )
{
//	printf( "\nSTOP: Test Pack" );

    message_transmit( SEND_ALL, CMD_EX, st_mask, EX_ABORT_TESTING );
}

void MainWindow::handlePack( uint8_t pack )
{
    if ( packRunning & BIT( pack )) {
        /* Останов пакета */

printf( "\nSTOP: RTO" );
        stopPackMessage();
    } else {
        /* Запуск пакета */

printf( "\nSTart RTO" );
        if ( idle ){
            startPackMessage( pack );
        }
    }
}

void MainWindow::handleDisk( uint8_t op )
{
    if ( st_mask == 0 ) return;

    if (( op == PREP_TD_CHECK ) && ( data_get_byte( var_td_operate ) == TD_CHECK )) {
        /* Прервать проверку ТД */
        data_set_byte( var_td_operate, TD_NONE );
//		printf( "\nSTOP: Check TD" );

        return;
    }
    if ( !idle ) return;

    data_set_dword( var_td_mask, st_mask );
    data_set_int( var_td_percent, 0 );
    data_set_byte( var_td_operate, ( op == PREP_TD_LOAD ) ? TD_WRITE : TD_CHECK );
//	printf( "\nSTART: %s", ( op == PREP_TD_LOAD ) ? "Load TD" : "Check TD" );
}

bool MainWindow::handleSetName( void )
{
    if ( map[ selected ].kind != MAP_ST ) return ( false );
    /* Выбрана станция... */
    dword ind = map[ selected ].ind;
    uint32_t turnon = CLEAR_VALUE( data_get_dword( var_online_mask )
    , data_get_dword( var_operate_mask )) & ( ~MI_MASK_ALL );

    /* if ( datn_get_byte( varn_state, ind ) != TURN_OFF ) break;
     */
    if ( popcount( turnon ) > 1 ) return ( false );

    /* Не более одной станции сети */
    QString msg = QString( M_DLG_NAME_MSG )
    .arg( st_list_ex[ ind ].name )
    .arg(( turnon ) ? st_list_ex[ __builtin_ffs( turnon ) - 1 ].name : M_DLG_NAME_NO );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_NAME, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        /* Задать имя */
        //message_transmit( SEND_ALL, CMD_NAME, 0, 0, sizeof( dword ), &ind );
        message_transmit( SEND_ALL, CMD_NAME, st_mask, 0, sizeof( dword ), &ind );
        /* Записать в журнал */
        journal_freeze_time();
        journal_printf( "", M_SET_NAME_ST
        , ( turnon ) ? st_list_ex[ __builtin_ffs( turnon ) - 1 ].name_ch : M_DLG_NAME_NO
        , st_list_ex[ ind ].name_ch );
        return ( true );
    }
    return ( false );
}

void MainWindow::handleMeter( uint8_t op )  // тут
{
    for ( int i = 0; i < cell.size(); i++ ) {
        if ( cell[ i ]->kind == MAP_MCP ) {
            cell[ i ]->check();
        }
    }
    if ( mcp_mask == 0 ) return;

    if ( !idle2 ) return;
    if ( !idle3 ) return; // nik

    data_set_dword( var_mcp_mask, mcp_mask );
    data_set_int( var_mcp_percent, 0 );
    data_set_byte( var_mcp_operate, ( op == PREP_MI_ID ) ? MCP_WRITE_ID : MCP_WRITE_GRAD );
    //data_set_byte( var_mcp_operate, ( op == PREP_MI_ID ) ? MCP_READ_ID : MCP_READ_GRAD );
//	printf( "\nSTART: %s", ( op == PREP_MI_ID ) ? "Write ID" : "Write GRAD" );
}

void MainWindow::handleMeterReplace( void )
{
    char s[ 256 ];

    if ( !idle2 ) return;
    if ( map[ selected ].kind != MAP_MI ) return;
    /* Выбран МИ */

    int st = map[ selected ].ind;

    if ( datm_get_byte( varm_state, st ) == MI_OFF ) return;
    /* МИ в сети */

    int sn_new = datm_get_dword( varm_sn_mi, st );

    if (( sn_new == 0 ) || ( sn_new == mi_list[ st ].sn )) return;
    /* Изменился заводской номер */

    QString msg = QString( M_DLG_REPLACE_MSG ).arg( mi_list[ st ].name ).arg( sn_new );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_NAME, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        /* Записать в mi.ini */
        QSettings ini_mi( ini_at_home( encode_system( s, &ini_file_mi, sizeof( s ))), QSettings::IniFormat );
        ini_mi.setIniCodec( "UTF-8" );
        ini_mi.beginGroup( "sn" );
        sprintf( s, "%d", st + MI_ID_MIN );  /* addr */
        ini_mi.setValue( s, sn_new );
        ini_mi.endGroup();
        /* Записать в журнал */
        journal_freeze_time();
        journal_printf( "", "Замена модуля %s заводской номер %d на %d.", mi_list[ st ].name_ch, mi_list[ st ].sn
        , sn_new );

        mi_list[ st ].sn = sn_new;
    }
}

bool MainWindow::handleBlockAddr( void )
{
    struct SET_UPO_ADR d;
    int ind, i;
    int blMode;

    if ( !idle ) return ( false );

    if ( map[ selected ].kind != MAP_BL ) return ( false );
    /* Выбран блок */
    ind = map[ selected ].ind;
    d.typeBlock = bl_list[ ind ].type;
    d.adrCANnew = bl_list[ ind ].addr;
    d.bParam = 0;
    blMode = datb_get_bl( varb_state, ind, &voteb_state );

printf("Set Addr (%d) %d MODE=%d \n", selected, bl_list[ ind ].addr, blMode);

    if (( bl_list[ ind ].addr >= CAN_ADDR_STANDBY_MIN )
    &&  ( bl_list[ ind ].addr <= CAN_ADDR_STANDBY_MAX )) {
        /* Проверочный адрес */

printf("Set -a \n");

        if ( blMode != REGWORK_VU_OFF ) return ( false );
        /* Блок не в сети - поиск блока */
        for ( i = 0; i < bl_count; i++ ) {
            if (( bl_list[ i ].gr == GR_LIMIT ) && ( bl_list[ i ].type == d.typeBlock )) {
                /* Блок перемещается в ЗИП */
                d.adrCANold = bl_list[ i ].addr;
                message_transmit( SEND_ALL, CMD_NAME_BLOCK, 0, 0, sizeof( dword ), &d );
                printf("Set -aa \n");
            }
        }
        return ( true );
    }


    /* Штатный адрес */
    //if ( datb_get_bl( varb_state, ind, &voteb_state ) != REGWORK_VU_OFF ) {  /* Блок в сети */
    if (blMode != REGWORK_VU_OFF){ // Блок в сети
        /* Блок перемещается в ЗИП */

//printf("Set -b \n");

        d.adrCANold = bl_list[ ind ].addr;
        d.adrCANnew = CAN_ADDR_STANDBY_MIN + d.typeBlock;
    } else {  /* Не в сети */
        /* Блок из ЗИПа */

//printf("Set -c \n");

        d.adrCANold = CAN_ADDR_STANDBY_MIN + d.typeBlock;
    }
    QString msg = QString( M_DLG_ADDR_MSG ).arg( d.adrCANnew ).arg( d.adrCANold );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_NAME, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        // Задать адрес

//printf("Set send %d >> %d\n", d.adrCANold, d.adrCANnew);

        message_transmit( SEND_ALL, CMD_NAME_BLOCK, 0, 0, sizeof( dword ), &d );
        // Записать в журнал
        journal_freeze_time();
        journal_printf( "", M_SET_ADDR_BL, d.adrCANold, d.adrCANnew );
        return ( true );
    }

    return ( false );
}

bool MainWindow::handleBlockProg( void )
{
    int ind;

    if ( !idle ) return ( false );

    if ( map[ selected ].kind != MAP_BL ) return ( false );
    /* Выбран блок */
    ind = map[ selected ].ind;

/*	if (( bl_list[ ind ].addr < CAN_ADDR_STANDBY_MIN )
    ||  ( bl_list[ ind ].addr > CAN_ADDR_STANDBY_MAX )) return ( false );
 */
    if ( datb_get_bl( varb_state, ind, &voteb_state ) == REGWORK_VU_OFF ) return ( false );
    /* Блок в сети */
    QString msg = QString( M_DLG_PROG_MSG ).arg( bl_list[ ind ].name );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_PROG, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        /* Передать ПО блока */
        if ( data_get_byte( var_bl_addr ) == 0 ) {
            data_set_byte( var_bl_addr, bl_list[ ind ].addr );
            return ( true );
        }else
            QMessageBox::information( this, M_DLG_MESSAGE, M_BL_PROG_NOT, QMessageBox::Ok );
    }
    return ( false );
}

void MainWindow::onBlockProgAll( void )
{
    if ( !ini_gui_bl_prog_all ) return;

    if ( !idle ) return;

    if ( timerBlockProgAll != NULL ) return;

    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_PROG, M_DLG_PROG_ALL_MSG
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        blockProg = 0;
        timerBlockProgAll = new QTimer( this );
        connect( timerBlockProgAll, SIGNAL( timeout()), this, SLOT( onTimerBlockProgAll()));
        timerBlockProgAll->start( BLOCK_PROG_PERIOD );
    }
}

void MainWindow::onTimerBlockProgAll( void )
{
    static int to = 0;
    ++to;

    if ( data_get_byte( var_bl_addr ) != 0 ) return;  /* Выполняется передача ПО блока */

    if ( blockProg & BLOCK_PROG_WAIT ) {  /* Ожидание завершения загрузки ПО в блок */
        /* Таймаут записи */
        if ( to > ( BLOCK_PROG_TIMEOUT / BLOCK_PROG_PERIOD )) {
            CLEAR_VAR( blockProg, BLOCK_PROG_WAIT );
            journal_freeze_time();
            journal_printf( "", "Запись ПО блока %s завершена (таймаут).", bl_list[ blockProg ].name_ch );
            /* Следующий блок */
            ++blockProg;
            return;
        }
        int chsum = bl_chsum[ CLEAR_VALUE( blockProg, BLOCK_PROG_WAIT )][ 0 ].app;
        if ( blockChsum && chsum && ( blockChsum != chsum )) {  /* Изменилась контрольная сумма */
            CLEAR_VAR( blockProg, BLOCK_PROG_WAIT );
            journal_freeze_time();
            journal_printf( "", "Запись ПО блока %s завершена (контр. сумма).", bl_list[ blockProg ].name_ch );
            /* Следующий блок */
            ++blockProg;
            return;
        }
    } else {
        /* Пропустить блоки без группы */
        while (( blockProg < bl_count ) && ( bl_list[ blockProg ].gr == GR_LIMIT )) ++blockProg;

        if ( blockProg >= bl_count ) {  /* Все блоки записаны */
            /* Остановить таймер */
            timerBlockProgAll->stop();
            delete timerBlockProgAll;
            timerBlockProgAll = NULL;

            journal_freeze_time();
            journal_print( "", "Запись ПО блоков УПО завершена." );
            QMessageBox::information( this, M_DLG_MESSAGE, M_BL_PROG_ALL_END, QMessageBox::Ok );
            return;
        }

        if ( datb_get_bl( varb_state, blockProg, &voteb_state ) == REGWORK_VU_OFF ) {  /* Блок недоступен */
            journal_freeze_time();
            journal_printf( "", "Блок %s выключен, запись ПО невозможна.", bl_list[ blockProg ].name_ch );
            /* Следующий блок */
            ++blockProg;
            return;
        }
        /* Передать ПО блока */
        data_set_byte( var_bl_addr, bl_list[ blockProg ].addr );
        blockChsum = bl_chsum[ blockProg ][ 0 ].app;
        to = 0;
        /* Установить признак ожидания */
        SET_VAR( blockProg, BLOCK_PROG_WAIT );
    }
}

bool MainWindow::handleBlockInit( void )
{
    struct INIT_UPO d;
    int ind;

    //printf("Init1 \n");

    if ( !idle ) return ( false );

    //printf("Init2  \n");


    if ( map[ selected ].kind != MAP_BL ) return ( false );
    /* Выбран блок */
    ind = map[ selected ].ind;
    d.typeBlock = bl_list[ ind ].type;
    d.adrCAN = bl_list[ ind ].addr;

    if (( bl_list[ ind ].addr < CAN_ADDR_STANDBY_MIN )
    ||  ( bl_list[ ind ].addr > CAN_ADDR_STANDBY_MAX )) return ( false );

    //printf("Init4 (%d) %d MODE=%d \n", selected, bl_list[ ind ].addr);


    /* Проверочный адрес */
    if ( datb_get_bl( varb_state, ind, &voteb_state ) == REGWORK_VU_OFF ) return ( false );
    /* Блок в сети */
    QString msg = QString( M_DLG_INIT_MSG ).arg( bl_list[ ind ].name );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_INIT, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        /* Инициализировать блок */
        message_transmit( SEND_ALL, CMD_INIT_BLOCK, 0, 0, sizeof( word ), &d );
        /* Записать в журнал */
        journal_freeze_time();
        journal_printf( "", M_INIT_BLOCK, bl_list[ ind ].name_ch );
        return ( true );
    }
    return ( false );
}

bool MainWindow::handleBlockReserve( void )
{
    struct SET_UPO_ADR d;
    int ind;

    if ( !idle ) return ( false );

    if ( map[ selected ].kind != MAP_BL ) return ( false );
    /* Выбран блок */
    ind = map[ selected ].ind;
    d.typeBlock = bl_list[ ind ].type;
    d.bParam = 0;

    if (( bl_list[ ind ].addr < CAN_ADDR_STANDBY_MIN )
    ||  ( bl_list[ ind ].addr > CAN_ADDR_STANDBY_MAX )) return ( false );
    /* Проверочный адрес */
    if ( datb_get_bl( varb_state, ind, &voteb_state ) != REGWORK_VU_OFF ) {  /* Блок в сети */
        /* Блок перемещается в резерв */
        d.adrCANold = bl_list[ ind ].addr;
        d.adrCANnew = CAN_ADDR_RESERVE;
    } else {  /* Не в сети */
        /* Блок из резерва в ЗИП */
        d.adrCANold = CAN_ADDR_RESERVE;
        d.adrCANnew = bl_list[ ind ].addr;
    }
    QString msg = QString( M_DLG_ADDR_MSG ).arg( d.adrCANnew ).arg( d.adrCANold );
    QMessageBox::StandardButton reply = QMessageBox::question( this, M_DLG_NAME, msg
    , QMessageBox::Yes | QMessageBox::No );
    if ( reply == QMessageBox::Yes ) {
        /* Задать адрес */
        message_transmit( SEND_ALL, CMD_NAME_BLOCK, 0, 0, sizeof( dword ), &d );
        /* Записать в журнал */
        journal_freeze_time();
        journal_printf( "", M_SET_ADDR_BL, d.adrCANold, d.adrCANnew );
        return ( true );
    }
    return ( false );
}

void MainWindow::setSummary( uint8_t ind, const char *s, int col, bool focus )
{
    if ( ind >= SUMMARY_COUNT ) return;

    lstSummary->item( ind )->setText( s );
    lstSummary->item( ind )->setTextColor( QColor( col ));
    if ( focus ) setBottomPage( PAGE_SUMMARY );
}

void MainWindow::scrollSummary( uint8_t ind )
{
    if (( ind == 0 ) || ( ind >= SUMMARY_COUNT )) return;

    lstSummary->item( ind - 1 )->setText( lstSummary->item( ind )->text());
    lstSummary->item( ind - 1 )->setTextColor( lstSummary->item( ind )->textColor());
}

void MainWindow::setJournal( byte st, const char *s, int col )
{
    char b[ 256 ];

    if ( st == st_tst ) return;  /* Журнал ПКФ не отображается */

    snprintf( b, sizeof( b ), "%s %s", journal_get_time(), s );
    lstJournal[ st ]->addItem( b );
    lstJournal[ st ]->item( lstJournal[ st ]->count() - 1 )->setTextColor( QColor( col ));
    lstJournal[ st ]->scrollToBottom();

}

void MainWindow::setJournal2( byte st, const char *s, int col )
{
    char b[ 256 ];

    snprintf( b, sizeof( b ), "%s %s", journal_get_time(), s );
    lstJournal[ st + MI_ID_MIN ]->addItem( b );
    lstJournal[ st + MI_ID_MIN ]->item( lstJournal[ st + MI_ID_MIN ]->count() - 1 )->setTextColor( QColor( col ));
    lstJournal[ st + MI_ID_MIN ]->scrollToBottom();
}

void MainWindow::setJournal3( byte st, const char *s, int col ) // tyt
{
    char b[ 256 ];
    snprintf( b, sizeof( b ), "%s %s", journal_get_time(), s );
    lstJournal[ st + MCP_ID_MIN ]->addItem( b );
    lstJournal[ st + MCP_ID_MIN ]->item( lstJournal[ st + MCP_ID_MIN ]->count() - 1 )->setTextColor( QColor( col ));
    lstJournal[ st + MCP_ID_MIN ]->scrollToBottom();
}

void MainWindow::clearJournal( void )
{
    char s[ 256 ];

    journal_clear( ini_at_home( encode_system( s, &ini_journal_path, sizeof( s ))));
}

void MainWindow::clearLog( void )
{
#ifdef LOGGING
    log_clear();

#endif
}

void MainWindow::onStopTesting( void )
{
    ++cntBackspace;
}

void MainWindow::minimizeWindow( void )
{
    setWindowState( windowState() | Qt::WindowMinimized );
}

void MainWindow::restoreWindow( void )
{
    setWindowState(( windowState() & ~Qt::WindowMinimized ) | Qt::WindowActive );
}

void MainWindow::onInstanceStarted( void )
{
    restoreWindow();
}
