/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-clock.h
 *  \brief  Форма установки и проверки времени.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_CLOCK_H
#define GUI_CLOCK_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QProcess>

#include "include.h"

#include "gui-time.h"

class ClockDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ClockDialog( QWidget *parent = 0 );

private:
	QTableWidget *tabClock;
    QLabel *lbCheck;
	QTimer *timer;

    SetTimeDialog *dlgSetTime;

protected:
	void showEvent( QShowEvent *e );
	void hideEvent( QHideEvent *e );

private slots:
    void onClockClicked( void );
    void onSetClicked( void );
    void onAdjustClicked( void );
    void onTimer( void );
};

#endif
