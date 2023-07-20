/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-time.h
 *  \brief  Форма установки времени.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_TIME_H
#define GUI_TIME_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QTimeEdit>

#include "include.h"

class SetTimeDialog : public QDialog
{
    Q_OBJECT

public:
	explicit SetTimeDialog( QWidget *parent = 0 );

private:
	QTimeEdit *edTime;

protected:
	void showEvent( QShowEvent *e );

private slots:
    void onOkClicked( void );
};

#endif
