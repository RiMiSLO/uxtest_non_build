/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-time.cpp
 *  \brief  Форма установки времени.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#include "include.h"
#include "gui-time.h"
#include <QProcess>

#define MARGIN            8
#define W_BTN_BOTTOM      180

#ifdef TITLE_RU
#define M_DLG_TITLE       "Время ПКФ"

#else
#define M_DLG_TITLE       "Time"

#endif

#define M_OK              "ОК"
#define M_TIME            "<font color=#474747>Установить время</font>"

#define M_SET_TIME        "Коррекция времени ПКФ."

SetTimeDialog::SetTimeDialog( QWidget *parent )
	: QDialog( parent )
{
	QVBoxLayout *loMain = new QVBoxLayout( this );
	QPushButton *btnOk = new QPushButton( this );
	QLabel *lbTime = new QLabel( this );
	edTime = new QTimeEdit( this );

	loMain->setContentsMargins( MARGIN, MARGIN, MARGIN, MARGIN );
	loMain->setSpacing( 10 );

	lbTime->setAlignment( Qt::AlignCenter );
	lbTime->setText( M_TIME );
	edTime->setDisplayFormat( "hh:mm:ss" );
	edTime->setAlignment( Qt::AlignCenter );

	btnOk->setText( M_OK );
	btnOk->setFixedWidth( W_BTN_BOTTOM );

	btnOk->setDefault( true );
	connect( btnOk, SIGNAL( clicked()), this, SLOT( onOkClicked()));

	loMain->addWidget( lbTime );
	loMain->addWidget( edTime );
	loMain->addWidget( btnOk );
	setLayout( loMain );

	setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
	setWindowTitle( M_DLG_TITLE );
}

void SetTimeDialog::showEvent( QShowEvent *e )
{
	QTime t = QTime::currentTime();
	/* Начало следующей минуты */
	t = t.addSecs( 60 - t.second());
	edTime->setTime( t );
	edTime->setCurrentSection( QDateTimeEdit::MinuteSection );

	QDialog::showEvent( e );
}

void SetTimeDialog::onOkClicked( void )
{
	QTime t = edTime->time();
	QString cmd = ini_set_time.arg( t.hour()).arg( t.minute()).arg( t.second());

	QProcess::startDetached( ini_shell, QStringList() << ini_shell_key << cmd );
	/* Записать в журнал */
	journal_freeze_time();
	journal_print( "", M_SET_TIME );

	close();
}
