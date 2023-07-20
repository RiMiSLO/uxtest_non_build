/*   _______ ___ ___      _______ _______ _______ _______      _______
	|   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
	|   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
	|_______|___|___|      |___| |_______|_______| |___|      |_______|
 */

#include "include.h"
#include "gui-main.h"
#include "singleapplication.h"

#include <QApplication>
#include <pthread.h>
#include <signal.h>

typedef struct {
	int argc;
	char **argv;
} args_t;

MainWindow *dlgMain = NULL;

void gui_send_event( int type )
{
	if ( dlgMain ) {
	   QApplication::postEvent( dlgMain, new QEvent( QEvent::Type(( int ) QEvent::User + type )));
	}
}

static void *predict( void *args )
{
	_main((( args_t *) args )->argc, (( args_t *) args )->argv );
	/* finish gui */
	QApplication::quit();
	pthread_exit( NULL );

	return ( NULL );
}

extern void udp_thread( void );
extern void udp_init( void );
extern void udp_uninit( void );

static void *udp( void *p )
{
	( void ) p;

	udp_init();
	udp_thread();
	udp_uninit();
	pthread_exit( NULL );

	return ( NULL );
}



void quitHandle( int ) {
	app_alive = 0;
	QApplication::quit();
}

int main( int argc, char *argv[])
{
    int result;

	SingleApplication gui( argc, argv );

	app_init( argc, argv );

	QApplication::setStyle( "fusion" );
	QApplication::setFont( QFont( ini_font_family, ini_font_size ));
	QApplication::setApplicationName( __PROGRAM__ );

	gui.connect( &gui, SIGNAL( lastWindowClosed()), &gui, SLOT( quit()));
	signal( SIGINT, quitHandle );

	/* start predict */
	pthread_t t_predict;
	args_t args = { argc: argc, argv: argv };
	result = pthread_create( &t_predict, NULL, &predict, (void *) &args );
	if ( result != 0 ) {
		__echo( "ERROR", "pthread_create() = %d" __CRLF__, result );

		return ( result );
	}



	/* start udp */
	pthread_t t_udp;
    result = pthread_create( &t_udp, NULL, &udp, NULL );
    if ( result != 0 ) {
		__echo( "ERROR", "pthread_create() = %d" __CRLF__, result );

        return ( result );
	}

	/* start gui */
	dlgMain = new MainWindow();
	dlgMain->show();
	/* just restore main window if second instance started */
	dlgMain->connect( &gui, &SingleApplication::instanceStarted, dlgMain, &MainWindow::onInstanceStarted );

	result = gui.exec();
	delete dlgMain;
    dlgMain = nullptr;

	/* finish predict */
	app_alive = 0;
	usleep( 100 );

	app_uninit();
	return ( result );
}
