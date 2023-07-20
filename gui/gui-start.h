/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-start.h
 *  \brief  Форма запуска РЗУ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_START_H
#define GUI_START_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QDateTimeEdit>

#include "include.h"

class StartRZUDialog : public QDialog
{
    Q_OBJECT

public:
	explicit StartRZUDialog( QWidget *parent = 0 );
	void setRZU( int _mode );

private:
	int mode;

	QTableWidget *tabRZU;
	QPushButton *btnStart;
	QDateTimeEdit *edDateTime;

private slots:
    void onStartClicked( void );
};

#endif
