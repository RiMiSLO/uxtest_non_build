/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-start.cpp
 *  \brief  Форма запуска РЗУ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include "include.h"
#include "gui-rzu.h"
#include "gui-start.h"
#include <QThread>

#define MARGIN            8
#define W_BTN_BOTTOM      180
#define H_LABEL           28

#ifdef TITLE_RU
#define M_DLG_TITLE       "Запуск РЗУ"

#else
#define M_DLG_TITLE       "Start"

#endif

#define M_START           "Запуск"
#define M_CONTINUE        "Продолжение"
#define M_CLOSE           "Закрыть [X]"
#define M_DATETIME        "<font color=#474747>Дата и время запуска</font>"

#define M_RZU_START       "Запуск РЗУ."

#define STYLE_TABLE       "QTableWidget { border: 0px; }"

#define PASSPORT_COUNT	  8

static const char * const M_PASSPORT[ PASSPORT_COUNT ] = {
	"  Название", "", "  Дата создания", "", "  Время создания", "", "  Верификатор", ""
};

static QColor colLight = QColor( 0x695c7d );

static struct START start;

StartRZUDialog::StartRZUDialog( QWidget *parent )
	: QDialog( parent ),
	mode( NEW_WORK )
{
	QTableWidgetItem *tcell;

	QVBoxLayout *loMain = new QVBoxLayout( this );
	QWidget *wgBottom = new QWidget( this );
	QHBoxLayout *loBottom = new QHBoxLayout( wgBottom );
	btnStart = new QPushButton( this );
    QPushButton *btnClose = new QPushButton( this );
	tabRZU = new QTableWidget( this );
	QLabel *lbDateTime = new QLabel( this );
	edDateTime = new QDateTimeEdit( this );

	loMain->setContentsMargins( MARGIN, MARGIN, MARGIN, MARGIN );
	loMain->setSpacing( 0 );

	lbDateTime->setAlignment( Qt::AlignCenter );
	lbDateTime->setText( M_DATETIME );
	lbDateTime->setMinimumHeight( H_LABEL );
	edDateTime->setAlignment( Qt::AlignCenter );

	edDateTime->setDisplayFormat( "dd.MM.yyyy hh:mm:ss" );
	edDateTime->setDateTimeRange(
	  QDateTime( QDate( 2000, 1, 1 ), QTime( 0, 0, 0 ))
	, QDateTime( QDate( 2100, 1, 1 ), QTime( 0, 0, 0 ))
	);
	btnStart->setText( M_START );
	btnStart->setFixedWidth( W_BTN_BOTTOM );

	btnClose->setText( M_CLOSE );
	btnClose->setStyleSheet( "color: #695c7d;" );
	btnClose->setFixedWidth( W_BTN_BOTTOM );

	tabRZU->setStyleSheet( STYLE_TABLE );
	tabRZU->verticalHeader()->setVisible( false );
	tabRZU->horizontalHeader()->setVisible( false );
	tabRZU->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tabRZU->setSelectionMode( QAbstractItemView::NoSelection );
	tabRZU->setFocusPolicy( Qt::NoFocus );
	tabRZU->setColumnCount( 1 );
	tabRZU->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
	tabRZU->setHorizontalHeaderItem( 0, new QTableWidgetItem());

	tabRZU->setRowCount( PASSPORT_COUNT );
	for ( int i = 0; i < PASSPORT_COUNT; i++ ) {
		tcell = new QTableWidgetItem( M_PASSPORT[ i ]);
		if ( i % 2 ) {
			tcell->setTextAlignment( Qt::AlignCenter );
		} else {
			tcell->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
			tcell->setForeground( colLight );
		}
		tabRZU->setItem( i, 0, tcell );
	}
	tabRZU->setMinimumHeight( tabRZU->rowCount() * tabRZU->rowHeight( 0 ) + tabRZU->horizontalHeader()->height());

	loBottom->setContentsMargins( 0, MARGIN, 0, 0 );
	loBottom->setSpacing( 10 );

	btnStart->setDefault( true );
	connect( btnStart, SIGNAL( clicked()), this, SLOT( onStartClicked()));
	connect( btnClose, SIGNAL( clicked()), this, SLOT( close()));

	loBottom->addWidget( btnStart );
	loBottom->addWidget( btnClose );
	wgBottom->setLayout( loBottom );
	loMain->addWidget( tabRZU );
	loMain->addWidget( lbDateTime );
	loMain->addWidget( edDateTime );
	loMain->addWidget( wgBottom );
	setLayout( loMain );

	setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
	setWindowTitle( M_DLG_TITLE );
}

void StartRZUDialog::onStartClicked( void )
{
	if ( rzu_ind >= rzu_count ) return;

	QDateTime dt = QDateTime::currentDateTime().addSecs( 1 );

	start.byCurDay   = dt.date().day();
	start.byCurMonth = dt.date().month();
	start.wCurYear   = dt.date().year();
	start.CurTime    = dt.time().hour() * 60 * 60 + dt.time().minute() * 60 + dt.time().second();
	if ( mode == NEW_WORK ) {
		/* Текущее время */
		start.byDay      = start.byCurDay;
		start.byMonth    = start.byCurMonth;
		start.wYear      = start.wCurYear;
		start.StartTime  = start.CurTime * 100;
	} else {
		/* Время и дата из редактора */
		dt = edDateTime->dateTime();
		start.byDay      = dt.date().day();
		start.byMonth    = dt.date().month();
		start.wYear      = dt.date().year();
		start.StartTime  = ( dt.time().hour() * 60 * 60 + dt.time().minute() * 60 + dt.time().second()) * 100;
	}
	start.NumNZU = rzu_passport[ rzu_ind ].Veryfi;
	start.Regime = mode;
	/* Ожидание смены секунды */
	while ( dt > QDateTime::currentDateTime()) QThread::msleep( 10 );
	/* Запуск РЗУ */
	message_transmit( SEND_ALL, CMD_START, 0, 0, sizeof( struct START ), &start );
	/* Записать в журнал */
	journal_freeze_time();
	journal_print( "", M_RZU_START );

	close();
}

void StartRZUDialog::setRZU( int _mode )
{
	char s[ 128 ];

	mode = _mode;
	btnStart->setText(( mode == NEW_WORK ) ? M_START : M_CONTINUE );

	if ( rzu_ind >= rzu_count ) return;

	tabRZU->item( 1, 0 )->setText( decode_cp1251(( char *) rzu_passport[ rzu_ind ].szRemark ));
	snprintf( s, sizeof( s ), "%02u.%02u.%u"
	, rzu_passport[ rzu_ind ].byDay
	, rzu_passport[ rzu_ind ].byMonth
	, rzu_passport[ rzu_ind ].wYear
	);
	tabRZU->item( 3, 0 )->setText( s );
	tabRZU->item( 5, 0 )->setText( SelectRZUDialog::time2str( s, rzu_passport[ rzu_ind ].Time ));
	tabRZU->item( 7, 0 )->setText( strn_crc32( s, rzu_passport[ rzu_ind ].Veryfi, NULL ));

	tabRZU->resizeColumnsToContents();
	tabRZU->setMinimumWidth( tabRZU->columnWidth( 0 ) + 40 );

	QDateTime dt = QDateTime::currentDateTime();
	/* Начало следующей минуты */
	dt = dt.addSecs( 60 - dt.time().second());
	edDateTime->setDateTime( dt );
	edDateTime->setCurrentSection( QDateTimeEdit::MinuteSection );
}
