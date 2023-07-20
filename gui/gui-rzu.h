/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  gui-rzu.h
 *  \brief  Форма выбора РЗУ.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef GUI_RZU_H
#define GUI_RZU_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>

#include "include.h"

class SelectRZUDialog : public QDialog
{
    Q_OBJECT

public:
	explicit SelectRZUDialog( QWidget *parent = 0 );
	static char * time2str( char *s, _DWORD t );

private:
	QTableWidget *tabRZU;

private slots:
    void onSelectClicked( void );
	void onFinished( int result );
};

#endif
