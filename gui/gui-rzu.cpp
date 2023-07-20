/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-rzu.cpp
 *  \brief  Форма выбора РЗУ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include "gui-rzu.h"

#define W_COL_DATE    120
#define W_COL_TIME    120

#ifdef TITLE_RU
#define M_DLG_TITLE       "Выбор РЗУ"

#else
#define M_DLG_TITLE       "RZU"

#endif

#define M_COL_NAME        "Название"
#define M_COL_DATE        "Дата"
#define M_COL_TIME        "Время"
#define M_BTN_SELECT      "Выбрать"

#define M_RZU_SELECTED    "РЗУ: %s"

#define STYLE_TABLE  "QTableWidget { border: 0px; }"

static QColor colTableBlack = QColor( COLOR_DEFAULT );

SelectRZUDialog::SelectRZUDialog( QWidget *parent )
	: QDialog( parent )
{
	char s[ 128 ];
	QTableWidgetItem *tcell;

	QVBoxLayout *loMain = new QVBoxLayout( this );
	QPushButton *btnSelect = new QPushButton( this );
	tabRZU = new QTableWidget( this );

	btnSelect->setText( M_BTN_SELECT );

	tabRZU->setStyleSheet( STYLE_TABLE );
	tabRZU->verticalHeader()->setVisible( false );
	tabRZU->horizontalHeader()->setVisible( false );
	tabRZU->horizontalHeader()->setHighlightSections( false );
	tabRZU->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tabRZU->setSelectionBehavior( QAbstractItemView::SelectRows );
	tabRZU->setSelectionMode( QAbstractItemView::SingleSelection );

	tabRZU->setColumnCount( 3 );
	tabRZU->setHorizontalHeaderItem( 0, new QTableWidgetItem( M_COL_NAME ));
	tabRZU->setHorizontalHeaderItem( 1, new QTableWidgetItem( M_COL_DATE ));
	tabRZU->setHorizontalHeaderItem( 2, new QTableWidgetItem( M_COL_TIME ));

	tabRZU->setRowCount( rzu_count );
	for ( int i = 0; i < rzu_count; i++ ) {
		tcell = new QTableWidgetItem( decode_cp1251(( char *) rzu_passport[ i ].szRemark ));
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground( colTableBlack );
		tabRZU->setItem( i, 0, tcell );

		snprintf( s, sizeof( s ), "%02u.%02u.%u"
		, rzu_passport[ i ].byDay
		, rzu_passport[ i ].byMonth
		, rzu_passport[ i ].wYear
		);
		tcell = new QTableWidgetItem( s );
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground( colTableBlack );
		tabRZU->setItem( i, 1, tcell );

		tcell = new QTableWidgetItem( time2str( s, rzu_passport[ i ].Time ));
		tcell->setTextAlignment( Qt::AlignCenter );
		tcell->setForeground( colTableBlack );
		tabRZU->setItem( i, 2, tcell );
	}
	tabRZU->resizeColumnsToContents();
	tabRZU->setColumnWidth( 0, tabRZU->columnWidth( 0 ) + 20 );
	tabRZU->setColumnWidth( 1, W_COL_DATE );
	tabRZU->setColumnWidth( 2, W_COL_TIME );
	tabRZU->setMinimumHeight( tabRZU->rowCount() * tabRZU->rowHeight( 0 ) + tabRZU->horizontalHeader()->height());
	tabRZU->setMinimumWidth( tabRZU->columnWidth( 0 ) + tabRZU->columnWidth( 1 ) + tabRZU->columnWidth( 2 ));
	tabRZU->setCurrentCell( 0, 0 );

	btnSelect->setDefault( true );
	connect( btnSelect, SIGNAL( clicked()), this, SLOT( onSelectClicked()));

	loMain->addWidget( tabRZU );
	loMain->addWidget( btnSelect );
	setLayout( loMain );

	connect( this, SIGNAL( finished( int )), this, SLOT( onFinished( int )));

	setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint );
	setWindowTitle( M_DLG_TITLE );
}

char * SelectRZUDialog::time2str( char *s, _DWORD t )
{
	uint32_t sec;

	t /= 100;
	sec = t % 60;
	t /= 60;
	sprintf( s, "%02u:%02u:%02u", t / 60, t % 60, sec );

	return ( s );
}

void SelectRZUDialog::onSelectClicked( void )
{
	int ind = tabRZU->currentIndex().row();
	char s[ 256 ];

	if (( ind >= 0 ) && ( ind < rzu_count )) {
		datn_set_dword( varn_verify, rzu_passport[ ind ].Veryfi, st_tst );
		gui_send_event( EVENT_RZU_CHANGED );
		/* Записать в журнал */
		QString a = decode_cp1251(( char *) rzu_passport[ ind ].szRemark );
		journal_freeze_time();
		journal_printf( "", M_RZU_SELECTED, encode_utf8( s, &a, sizeof( s )));
		close();
	}
}

void SelectRZUDialog::onFinished( int result )
{
	( void ) result;
	data_set_byte( var_select_rzu, 0 );
}
