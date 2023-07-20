/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-net.h
 *  \brief  Форма теста сети.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_NET_H
#define GUI_NET_H

#include <QApplication>
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QTimer>

#include "include.h"

class TestNetDialog : public QDialog
{
    Q_OBJECT

public:
	explicit TestNetDialog( QWidget *parent = 0 );
	~TestNetDialog();

private:
	QTableWidget *tabShort;
	QTableWidget *tabDetail;
	QSpinBox *edValue;
	QPushButton *btnStart;
	QTimer *timer;
	void clearTables( void );

private slots:
	void onStartClicked( void );
	void onResetClicked( void );
	void onValueChanged( int value );
	void onTimer( void );
};

#endif
