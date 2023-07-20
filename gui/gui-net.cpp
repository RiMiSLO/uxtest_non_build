/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-net.cpp
 *  \brief  Форма теста сети.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include "gui-net.h"

#define SHOW_COLS         9
#define SHOW_ROWS         30

#define MARGIN            8

#define W_COL             120
#define H_COL             28
#define W_BTN_BOTTOM      180

#ifdef TITLE_RU
#define M_DLG_TITLE       "Тест сетей Ethernet"

#else
#define M_DLG_TITLE       "Ethernet"

#endif

#define M_SHORT           "Кратко"
#define M_DETAIL          "Детально"
#define M_RATE            "Частота"
#define M_START           "Старт"
#define M_STOP            "Стоп"
#define M_RESET_STAT      "Сброс стат."
#define M_CLOSE           "Закрыть [X]"

#define M_ST              ""
#define M_TRANSMIT        "Передано %1"
#define M_RECEIVE         "Принято %1"
#define M_MISSED          "Потеряно %1"

#define STYLE_TABBAR      "QTabBar::tab { min-width: 120px; }"
#define STYLE_TABLE       "QTableWidget { border: 1px solid #bab9b9; margin-top: -1px; }"

TestNetDialog::TestNetDialog( QWidget *parent )
	: QDialog( parent )
{
	QTableWidgetItem *tcell;
	QFont fontSmall;
	QFont fontBold;
	QColor colTableBlack = QColor( COLOR_DEFAULT );
	int col;

	QVBoxLayout *loMain = new QVBoxLayout( this );
	QTabWidget *pgTab = new QTabWidget( this );
	QWidget *wgBottom = new QWidget( this );
	QHBoxLayout *loBottom = new QHBoxLayout( wgBottom );
	QWidget *wgShort = new QWidget( this );
	QVBoxLayout *loShort = new QVBoxLayout( wgShort );
	tabShort = new QTableWidget( this );
	QWidget *wgDetail = new QWidget( this );
	QVBoxLayout *loDetail = new QVBoxLayout( wgDetail );
	tabDetail = new QTableWidget( this );
	btnStart = new QPushButton( this );
	QPushButton *btnReset = new QPushButton( this );
	QPushButton *btnClose = new QPushButton( this );
	QLabel *lbValue = new QLabel( this );
	edValue = new QSpinBox( this );

	fontSmall = tabShort->font();
	fontSmall.setPointSize( fontSmall.pointSize() - 2 );
	fontBold = fontSmall;
	fontBold.setBold( true );

	loMain->setContentsMargins( MARGIN, MARGIN / 2, MARGIN, MARGIN );
	loMain->setSpacing( 0 );
	pgTab->setStyleSheet( STYLE_TABBAR );
	pgTab->setDocumentMode( true );

	loShort->setContentsMargins( 0, 0, 0, 0 );
	loShort->setSpacing( 0 );

	tabShort->setStyleSheet( STYLE_TABLE );
	tabShort->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tabShort->setSelectionMode( QAbstractItemView::NoSelection );
	tabShort->setFocusPolicy( Qt::NoFocus );
	tabShort->setFont( fontSmall );
	tabShort->setColumnCount( ini_net_count * 3 );

	for ( int i = 0; i < ini_net_count; i++ ) {
		tabShort->setColumnWidth( i * 3 + 0, W_COL );
		tcell = new QTableWidgetItem( QString( M_TRANSMIT ).arg( i + 1 ));
		tcell->setForeground( colTableBlack );
		tabShort->setHorizontalHeaderItem( i * 3 + 0, tcell );
		tabShort->setColumnWidth( i * 3 + 1, W_COL );
		tcell = new QTableWidgetItem( QString( M_RECEIVE ).arg( i + 1 ));
		tcell->setForeground( colTableBlack );
		tabShort->setHorizontalHeaderItem( i * 3 + 1, tcell );
		tabShort->setColumnWidth( i * 3 + 2, W_COL );
		tcell = new QTableWidgetItem( QString( M_MISSED ).arg( i + 1 ));
		tcell->setForeground( colTableBlack );
		tabShort->setHorizontalHeaderItem( i * 3 + 2, tcell );
	}
    tabShort->setRowCount( st_count );
	tabShort->verticalHeader()->setFixedWidth( W_COL );
	tabShort->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	tabShort->verticalHeader()->setDefaultSectionSize( H_COL );
	tabShort->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    for ( int i = 0; i < st_count; i++ ) {
		tcell = new QTableWidgetItem( st_list_ex[ i ].name );
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground( colTableBlack );
		tabShort->setVerticalHeaderItem( i, tcell );

		for ( int ii = 0; ii < ini_net_count; ii++ ) {
			tcell = new QTableWidgetItem( "-" );
			tcell->setTextAlignment( Qt::AlignCenter );
			tcell->setForeground( colTableBlack );
			tabShort->setItem( i, ii * 3 + 0, tcell );

			tcell = new QTableWidgetItem( "-" );
			tcell->setTextAlignment( Qt::AlignCenter );
			tcell->setForeground( colTableBlack );
			tabShort->setItem( i, ii * 3 + 1, tcell );

			tcell = new QTableWidgetItem( "-" );
			tcell->setTextAlignment( Qt::AlignCenter );
			tcell->setForeground( colTableBlack );
			tabShort->setItem( i, ii * 3 + 2, tcell );
		}
	}
	col = ( st_count < SHOW_COLS ) ? st_count : SHOW_COLS;
	tabShort->setMinimumHeight( col * tabShort->rowHeight( 0 ) + tabShort->horizontalHeader()->height());

	loDetail->setContentsMargins( 0, 0, 0, 0 );
	loDetail->setSpacing( 0 );

	tabDetail->setStyleSheet( STYLE_TABLE );
	tabDetail->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tabDetail->setSelectionMode( QAbstractItemView::NoSelection );
	tabDetail->setFocusPolicy( Qt::NoFocus );
	tabDetail->setColumnCount( st_count );
	tabDetail->setFont( fontSmall );

	for ( int i = 0; i < st_count; i++ ) {
		tabDetail->setColumnWidth( i, W_COL );
		tcell = new QTableWidgetItem( st_list_ex[ i ].name );
		tcell->setForeground( colTableBlack );
		tabDetail->setHorizontalHeaderItem( i, tcell );
	}
    tabDetail->setRowCount( st_count * ini_net_count );
	tabDetail->verticalHeader()->setFixedWidth( W_COL );
	tabDetail->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	tabDetail->verticalHeader()->setDefaultSectionSize( H_COL );
	tabDetail->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    for ( int i = 0; i < st_count; i++ ) {
		for ( int ii = 0; ii < ini_net_count; ii++ ) {
			tcell = new QTableWidgetItem( QString( "%1 %2  " ).arg( st_list_ex[ i ].name ).arg( ii + 1 ));
			tcell->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
			tcell->setForeground( colTableBlack );
			tabDetail->setVerticalHeaderItem( i * ini_net_count + ii, tcell );
            for ( int j = 0; j < st_count; j++ ) {
				tcell = new QTableWidgetItem( "-" );
				tcell->setTextAlignment( Qt::AlignCenter );
				tcell->setForeground( colTableBlack );
				if ( i == j ) tcell->setFont( fontBold );
				tabDetail->setItem( i * ini_net_count + ii, j, tcell );
			}
		}
	}
	col = ( st_count * ini_net_count < SHOW_COLS ) ? st_count * ini_net_count : SHOW_COLS;
	tabDetail->setMinimumWidth( col * tabDetail->columnWidth( 0 ) + tabDetail->verticalHeader()->width());
	col = ( st_count * ini_net_count < SHOW_ROWS ) ? st_count * ini_net_count : SHOW_ROWS;
	tabDetail->setMinimumHeight( col * tabDetail->rowHeight( 0 ) + tabDetail->horizontalHeader()->height()
	+ qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) + 4 );

	loBottom->setContentsMargins( 0, MARGIN, 0, 0 );
	loBottom->setSpacing( 10 );

	lbValue->setText( M_RATE );
	lbValue->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
	edValue->setFixedWidth( W_BTN_BOTTOM / 2 );
	edValue->setRange( TEST_NET_RATE_MIN, TEST_NET_RATE_MAX );
	connect( edValue, SIGNAL( valueChanged( int )), this, SLOT( onValueChanged( int )));
	btnStart->setText( M_START );
	btnStart->setDefault( true );
	btnStart->setFixedWidth( W_BTN_BOTTOM );
	connect( btnStart, SIGNAL( clicked()), this, SLOT( onStartClicked()));
	btnReset->setText( M_RESET_STAT );
	btnReset->setFixedWidth( W_BTN_BOTTOM );
	connect( btnReset, SIGNAL( clicked()), this, SLOT( onResetClicked()));
	btnClose->setText( M_CLOSE );
	btnClose->setStyleSheet( "color: #695c7d;" );
	btnClose->setFixedWidth( W_BTN_BOTTOM );
	connect( btnClose, SIGNAL( clicked()), this, SLOT( close()));

	loShort->addWidget( tabShort );
	wgShort->setLayout( loShort );
	loDetail->addWidget( tabDetail );
	wgDetail->setLayout( loDetail );
	pgTab->addTab( wgShort, M_SHORT );
	pgTab->addTab( wgDetail, M_DETAIL );
	loBottom->addWidget( lbValue );
	loBottom->addWidget( edValue );
	loBottom->addWidget( btnStart );
	loBottom->addWidget( btnReset );
	loBottom->addWidget( btnClose );
	wgBottom->setLayout( loBottom );
	loMain->addWidget( pgTab );
	loMain->addWidget( wgBottom );
	setLayout( loMain );

	setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
	setWindowTitle( M_DLG_TITLE );

	timer = new QTimer( this );
	connect( timer, SIGNAL( timeout()), this, SLOT( onTimer()));

	data_set_byte( var_test_net_rate, ini_test_net_rate );
	edValue->setValue( ini_test_net_rate );
}

TestNetDialog::~TestNetDialog( void )
{
	timer->stop();
}

void TestNetDialog::onStartClicked( void )
{
	if ( data_get_byte( var_test_net_command )) {
		/* Останов теста */
		data_set_byte( var_test_net_command, 0 );
		btnStart->setText( M_START );
	} else {
		/* Запуск теста */
		data_set_byte( var_test_net_command, 1 );
		clearTables();
		btnStart->setText( M_STOP );
		timer->start( 1000 );
	}
}

void TestNetDialog::onResetClicked( void )
{
	/* Сброс сетевой статистики */
	message_transmit( SEND_ALL, CMD_STAT_RESET );
	if ( ini_net_mi ) {
        message_transmit2( SEND_BI, CMD_STAT_RESET );
	}
}

void TestNetDialog::onValueChanged( int value )
{
	data_set_byte( var_test_net_rate, ( byte ) value );
}

void TestNetDialog::onTimer( void )
{
	static uint32_t col_ok = 0;
	static uint32_t col_fail = COLOR_FAIL;

	if ( data_get_byte( var_test_net_command ) == 0 ) timer->stop();

	if ( this->isHidden()) return;

	if ( col_ok ) {
		col_ok = 0;
		col_fail = COLOR_FAIL;
	} else {
		col_ok = 0x404040;
		col_fail = 0xFF4040;
	}

	pthread_mutex_lock( &test_net_mutex_result );
	for ( int i = 0; i < st_count; i++ ) {
		if ( !test_net_renewed[ i ]) {

			continue;
		}
		/* Краткий результат */
		for ( int ii = 0; ii < ini_net_count; ii++ ) {
			dword miss = test_net_result_short[ i ].miss[ ii ];
			tabShort->item( i, ii * 3 + 0 )->setText( QString::number( test_net_result[ i ].st[ i ].num[ ii ]));
			tabShort->item( i, ii * 3 + 0 )->setForeground( QColor( col_ok ));
			tabShort->item( i, ii * 3 + 1 )->setText( QString::number( test_net_result_short[ i ].num[ ii ] - miss ));
			tabShort->item( i, ii * 3 + 1 )->setForeground( QColor( col_ok ));
			tabShort->item( i, ii * 3 + 2 )->setText( QString::number( miss ));
			tabShort->item( i, ii * 3 + 2 )->setForeground(( miss ) ? QColor( col_fail ) : QColor( col_ok ));
		}
		/* Детальный результат */
		for ( int j = 0; j < st_count; j++ ) {
			TEST_NET_DATA_ITEM *st = test_net_result[ i ].st + j;

			for ( int ii = 0; ii < ini_net_count; ii++ ) {
				if ( st->num[ ii ] || st->miss[ ii ]) {
					if ( i == j ) {
						tabDetail->item( i * ini_net_count + ii, j )->setText( QString( "%1" )
						.arg( st->num[ ii ]));
						tabDetail->item( i * ini_net_count + ii, j )
						->setForeground( QColor( col_ok ));
					} else {
						tabDetail->item( i * ini_net_count + ii, j )->setText( QString( "%1 (%2)" )
						.arg( st->num[ ii ]).arg( st->miss[ ii ]));
						tabDetail->item( i * ini_net_count + ii, j )
						->setForeground(( st->miss[ ii ]) ? QColor( col_fail ) : QColor( col_ok ));
					}
				} else {
					tabDetail->item( i * ini_net_count + ii, j )->setText( "-" );
				}
			}
		}
		test_net_renewed[ i ] = false;
	}
	pthread_mutex_unlock( &test_net_mutex_result );
}

void TestNetDialog::clearTables( void )
{
	for ( int i = 0; i < st_count; i++ ) {
		for ( int ii = 0; ii < ini_net_count; ii++ ) {
			tabShort->item( i, ii * 3 + 0 )->setText( "-" );
			tabShort->item( i, ii * 3 + 1 )->setText( "-" );
			tabShort->item( i, ii * 3 + 2 )->setText( "-" );
			for ( int j = 0; j < st_count; j++ ) {
				tabDetail->item( i * ini_net_count + ii, j )->setText( "-" );
			}
		}
	}
}
