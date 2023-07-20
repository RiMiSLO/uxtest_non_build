/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-clock.cpp
 *  \brief  Форма установки и проверки времени.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include "include.h"
#include "gui-clock.h"
#include <QDateTime>
#include <QThread>

#define MARGIN            8
#define W_COL             140
#define H_COL             28
#define W_BTN_BOTTOM      180

#ifdef TITLE_RU
#define M_DLG_TITLE       "Часы"

#else
#define M_DLG_TITLE       "Clock"

#endif

#define M_DATE            "Дата"
#define M_TIME            "Время"
#define M_CLOCK           "Часы ПКФ"
#define M_ADJUST          "Коррекция времени"
#define M_SET             "Синхронизация ВУ"
#define M_CLOSE           "Закрыть [X]"

#define M_SET_CLOCK       "Синхронизация часов ВУ."

#define STYLE_TITLE       "QLabel { background-color: #e3dfde; }"
#define STYLE_TABLE       "QTableWidget { border: 1px solid #bab9b9; }"

ClockDialog::ClockDialog( QWidget *parent )
	: QDialog( parent ),
	dlgSetTime( NULL )
{
	QTableWidgetItem *tcell;
	QFont fontSmall;
	QColor colTableBlack = QColor( COLOR_DEFAULT );
	QColor colTableLight = QColor( 0x695c7d );

	QVBoxLayout *loMain = new QVBoxLayout( this );
	QWidget *wgBottom = new QWidget( this );
	QGridLayout *loBottom = new QGridLayout( wgBottom );
    //QPushButton *btnClock = new QPushButton( wgBottom );
	QPushButton *btnAdjust = new QPushButton( wgBottom );
	QPushButton *btnSet = new QPushButton( wgBottom );
	QPushButton *btnClose = new QPushButton( wgBottom );
	QLabel *lbTitle = new QLabel( wgBottom );
	lbCheck = new QLabel( wgBottom );
	tabClock = new QTableWidget( this );

	fontSmall = tabClock->font();
	fontSmall.setPointSize( fontSmall.pointSize() - 1 );

	loMain->setContentsMargins( MARGIN, MARGIN, MARGIN, MARGIN );
	loMain->setSpacing( 0 );

	lbTitle->setFixedWidth( W_BTN_BOTTOM );
	lbTitle->setAlignment( Qt::AlignCenter );
	lbTitle->setStyleSheet( STYLE_TITLE );
	lbTitle->setFont( fontSmall );
	lbTitle->setText( "Расхождение часов" );

	lbCheck->setFixedWidth( W_BTN_BOTTOM );
	lbCheck->setAlignment( Qt::AlignCenter );
	lbCheck->setFont( fontSmall );

	btnSet->setText( M_SET );
	btnSet->setFixedWidth( W_BTN_BOTTOM );
    //btnClock->setText( M_CLOCK );
    //btnClock->setFixedWidth( W_BTN_BOTTOM );
	btnAdjust->setText( M_ADJUST );
	btnAdjust->setFixedWidth( W_BTN_BOTTOM );
	btnClose->setText( M_CLOSE );
	btnClose->setStyleSheet( "color: #695c7d;" );
	btnClose->setFixedWidth( W_BTN_BOTTOM );

	tabClock->setStyleSheet( STYLE_TABLE );
	tabClock->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tabClock->setSelectionMode( QAbstractItemView::NoSelection );
	tabClock->setFocusPolicy( Qt::NoFocus );
	tabClock->setFont( fontSmall );
	tabClock->setColumnCount( 2 );

	tabClock->setColumnWidth( 0, W_COL );
	tcell = new QTableWidgetItem( M_DATE );
	tcell->setForeground( colTableBlack );
	tabClock->setHorizontalHeaderItem( 0, tcell );
	tabClock->setColumnWidth( 1, W_COL );
	tcell = new QTableWidgetItem( M_TIME );
	tcell->setForeground( colTableBlack );
	tabClock->setHorizontalHeaderItem( 1, tcell );

	tabClock->verticalHeader()->setFixedWidth( W_COL );
	tabClock->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	tabClock->verticalHeader()->setDefaultSectionSize( H_COL );
	tabClock->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	tabClock->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	tabClock->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	int n = 0;
	for ( int i = 0; i < st_count; i++ ) {
        if ( st_list[ i ].TypeMRMO == MCP_TYPE ) continue; // было MI_TYPE
		tabClock->setRowCount( n + 1 );

		tcell = new QTableWidgetItem( st_list_ex[ i ].name );
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground( colTableBlack );
		tabClock->setVerticalHeaderItem( n, tcell );

		tcell = new QTableWidgetItem( "-" );
        tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground(( i == st_tst ) ? colTableLight : colTableBlack );
		tabClock->setItem( n, 0, tcell );

		tcell = new QTableWidgetItem( "-" );
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground(( i == st_tst ) ? colTableLight : colTableBlack );
		tabClock->setItem( n, 1, tcell );
		++n;
	}
	tabClock->setMinimumHeight( tabClock->rowCount() * tabClock->rowHeight( 0 ) + tabClock->horizontalHeader()->height());
	tabClock->setMinimumWidth( tabClock->columnCount() * tabClock->columnWidth( 0 ) + tabClock->verticalHeader()->width());

	loBottom->setContentsMargins( 0, MARGIN, 0, 0 );
	loBottom->setSpacing( 10 );

    //btnClock->setDefault( true );
    //connect( btnClock, SIGNAL( clicked()), this, SLOT( onClockClicked()));

    btnAdjust->setDefault( true );

	connect( btnAdjust, SIGNAL( clicked()), this, SLOT( onAdjustClicked()));
	connect( btnSet, SIGNAL( clicked()), this, SLOT( onSetClicked()));
	connect( btnClose, SIGNAL( clicked()), this, SLOT( close()));

    //loBottom->addWidget( btnClock, 0, 0 );
	loBottom->addWidget( lbTitle, 0, 1 );
	loBottom->addWidget( btnAdjust, 1, 0 );
	loBottom->addWidget( lbCheck, 1, 1 );
	loBottom->addWidget( btnSet, 2, 0 );
	loBottom->addWidget( btnClose, 2, 1 );
	wgBottom->setLayout( loBottom );
	loMain->addWidget( tabClock );
	loMain->addWidget( wgBottom );
	setLayout( loMain );

	setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
	setWindowTitle( M_DLG_TITLE );

	timer = new QTimer( this );
	connect( timer, SIGNAL( timeout()), this, SLOT( onTimer()));
}

void ClockDialog::showEvent( QShowEvent *e )
{
	timer->start( 100 );

	QDialog::showEvent( e );
}

void ClockDialog::hideEvent( QHideEvent *e )
{
	timer->stop();

	QDialog::hideEvent( e );
}

void ClockDialog::onClockClicked( void )
{
	QProcess::startDetached( ini_clock );
}

void ClockDialog::onAdjustClicked( void )
{
	if ( !dlgSetTime ) dlgSetTime = new SetTimeDialog( this );
	dlgSetTime->show();
}

void ClockDialog::onSetClicked( void )
{
	PTIMEDATE td;
	QDateTime dt = QDateTime::currentDateTime().addSecs( 1 );

printf("\nSET TIME 1\n");

	if (( td = ( PTIMEDATE ) malloc( sizeof( struct TIMEDATE ))) != NULL ) {

printf("\nSET TIME 2\n");
		td->byCurDay   = dt.date().day();
		td->byCurMonth = dt.date().month();
		td->wCurYear   = dt.date().year();
		td->CurTime    = dt.time().hour() * 60 * 60 + dt.time().minute() * 60 + dt.time().second();
		/* Ожидание смены секунды */
		while ( dt > QDateTime::currentDateTime()) QThread::msleep( 10 );
		/* Установка времени */

printf("\nSET TIME 3\n");

		message_transmit( SEND_ALL, CMD_SET_DATETIME, 0, 0, sizeof( struct TIMEDATE ), td, true );
		journal_freeze_time();
		journal_printf( "", M_SET_CLOCK );
	}
}

void ClockDialog::onTimer( void )
{
	time_t t, t_min, t_max;
	double diff;
	char sd[ 16 ], st[ 16 ];
	int n = 0;
	static uint32_t counter = 0;

	t_min = LONG_MAX;
	t_max = 0;
	for ( int i = 0; i < st_count; i++ ) {
        if ( st_list[ i ].TypeMRMO == MCP_TYPE ||  st_list[ i ].TypeMRMO == MI_TYPE ) continue;  // было MI_TYPE

		if ( i == st_tst ) {
			t = clock_stamp_datetime( sd, st );
			tabClock->item( n, 0 )->setText( sd );
			tabClock->item( n, 1 )->setText( st );
			if ( t > t_max ) t_max = t;
			if ( t < t_min ) t_min = t;
		} else {
			dword _d = datn_get_dword( varn_date, i );
			if ( _d ) {
				dword _t = datn_get_dword( varn_time, i );
				tabClock->item( n, 0 )->setText( strn_date( sd, _d, NULL ));
				tabClock->item( n, 1 )->setText( strn_time( st, _t, NULL ));
				t = dt2time_t( _d, _t );
				if ( t > t_max ) t_max = t;
				if ( t < t_min ) t_min = t;
			} else {
				tabClock->item( n, 0 )->setText( "-" );
				tabClock->item( n, 1 )->setText( "-" );
			}
		}
		++n;
	}
	if ( ++counter % 10 ) return;

	diff = fabs( difftime( t_max, t_min ));
	if ( diff <= ini_time_diff ) {
		/* Норма */
		lbCheck->setText( QString( "<font color=darkgreen>НОРМА (%1 сек)</font>" ).arg( diff ));
	} else {
		/* Ненорма */
		if ( diff > 59.0 ) {
			lbCheck->setText( QString( "<font color=red>НЕНОРМА (>1 мин)</font>" ));
		} else {
			lbCheck->setText( QString( "<font color=red>НЕНОРМА (%1 сек)</font>" ).arg( diff ));
		}
	}
}
