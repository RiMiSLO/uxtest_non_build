/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb.c
 *  \brief  Массив состояния ячеек УПО.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define DATB_IMPLEMENT

#include "include.h"

/**
 *  \addtogroup datb
 *  \{
 */

#define DATB_HOST   4 /* BL_HOST_COUNT + 1 */
#define DATB_CH     BL_CH_COUNT
#define DATB_DEPTH  BL_LIMIT

typedef uint8_t __chngb_t;
typedef char *( *strb_t )( char *, valb_t, int * );
typedef uint8_t ( *voteh_t )( valb_t *, valb_t, valb_t, valb_t );

/**
 *  \brief Дескриптор элемента массива состояния.
 */
struct DATB_DESCRY {
	/**  Описание элемента */
	const char *title;
	/**  Функция голосования значений от хостов */
	void *vote;
	/**  Функция преобразования значения в строку */
	void *string;
};

typedef const struct DATB_DESCRY *datb_descry_t;

static pthread_mutex_t datb_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 *  \brief Массив состояния.
 */
static valb_t datb_vector[ DATB_DEPTH ][ DATB_CH ][ DATB_HOST ][ varb_COUNT ];

static uint8_t __updated[ DATB_DEPTH ];
static char __str[ 100 ];

/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) { title, ( void *) vote, ( void *) string },
#define DATB_VAR3( var, pos, mask, offset, vote ) { "", ( void *) vote, NULL },

/**
 *  \brief Дескриптор вектора состояния.
 */
static const struct DATB_DESCRY datb_descry[] = {
#include "datb-dict.h"
	{ "none", NULL, NULL },
};

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */

static __chngb_t datb_changed[ DATB_DEPTH ][ DATB_CH ][ varb_COUNT ];
static uint8_t datb_host[ DATB_DEPTH ][ DATB_CH ][ varb_COUNT ];

#define DATB_CHK_VAR( item, R ) \
	do { \
		if ( item >= varb_COUNT ) { \
			assert_fail( "invalid varb (%d)", item ); \
			return R; \
		} \
	} while ( 0 )

#define DATB_CHK_BL( bl, R ) \
	do { \
		if ( bl >= DATB_DEPTH ) { \
			assert_fail( "invalid block (%d)", bl ); \
			return R; \
		} \
	} while ( 0 )

#define DATB_CHK_CH( ch, R ) \
	do { \
		if ( ch >= DATB_CH ) { \
			assert_fail( "invalid channel (%d)", ch ); \
			return R; \
		} \
	} while ( 0 )

#define DATB_CHK_HOST( h, R ) \
	do { \
		if ( h >= DATB_HOST ) { \
			assert_fail( "invalid host (%d)", h ); \
			return R; \
		} \
	} while ( 0 )

#define DATB_LOCK() \
	do { \
		pthread_mutex_lock( &datb_mutex ); \
	} while ( 0 )

#define DATB_UNLOCK() \
	do { \
		pthread_mutex_unlock( &datb_mutex ); \
	} while ( 0 )

valb_t datb_get( const varb_t item, const int bl, const int ch, const uint8_t host )
{
	valb_t result;
	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_CHK_CH( ch, 0 );
	DATB_CHK_HOST( host, 0 );
	DATB_LOCK();
	result = datb_vector[ bl ][ ch ][ host ][ item ];
	DATB_UNLOCK();
	return ( result );
}

valb_t datb_get_bl( const varb_t item, const int bl, valb_t ( *vote )( valb_t, valb_t, valb_t ))
{

    //return (vote(datb_get( item, bl, 0, 0 ), datb_get( item, bl, 1, 0 ), datb_get( item, bl, 2, 0 )));

#define T0( N )	  datb_get( item, bl, 0, 0 )
#define T( N )	, datb_get( item, bl, N, 0 )

	return ( vote( UNFOLD( DATB_CH, T0, T )));

#undef T0
#undef T
}

char *datb_get_string( const varb_t item, const int bl, const int ch, int *col )
{
	char *result;
	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_CHK_CH( ch, 0 );
	DATB_LOCK();
	result = (( strb_t ) datb_descry[ item ].string )( __str, datb_vector[ bl ][ ch ][ 0 ][ item ], col );
	DATB_UNLOCK();
	return ( result );
}

void datb_set( const varb_t item, valb_t value, const int bl, const int ch, const uint8_t host )
{
	valb_t *varh, *var;
	DATB_CHK_VAR( item, );
	DATB_CHK_BL( bl, );
	DATB_CHK_CH( ch, );
	DATB_CHK_HOST( host, );
	DATB_LOCK();
	varh = datb_vector[ bl ][ ch ][ host ] + item;
	if ( *varh != value ) {
		*varh = value;
		var = datb_vector[ bl ][ ch ][ 0 ] + item;
		value = *var;

#define T0( N )	var
#define T( N )	, datb_vector[ bl ][ ch ][ N ][ item ]

		if ( datb_descry[ item ].vote ) {
			datb_host[ bl ][ ch ][ item ] = (( voteh_t ) datb_descry[ item ].vote )( UNFOLD( DATB_HOST, T0, T ));
		}

#undef T0
#undef T

		if ( *var != value ) datb_changed[ bl ][ ch ][ item ] = 1;
	}
	DATB_UNLOCK();
}

int datb_get_changed( const varb_t item, const int bl, const int ch )
{
	int result;

	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_CHK_CH( ch, 0 );
	DATB_LOCK();
	result = datb_changed[ bl ][ ch ][ item ];
	DATB_UNLOCK();

	return ( result );
}

int datb_get_changed_bl( const varb_t item, const int bl )
{
	int ch;

	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_LOCK();

	for ( ch = 0; ch < DATB_CH; ch++ ) {
		if ( datb_changed[ bl ][ ch ][ item ]) {
			DATB_UNLOCK();
printf("datb_get_changed_bl item:%d (%d)\n",item, bl);
			return ( 1 );
		}
	}
	DATB_UNLOCK();

	return ( 0 );
}

int datb_get_changed_any( const int bl )
{
	varb_t item;
	int ch;

	DATB_CHK_BL( bl, 0 );
	DATB_LOCK();
	for ( ch = 0; ch < DATB_CH; ch++ ) {
		for ( item = 0; item < varb_COUNT; item++ ) {
			if ( datb_changed[ bl ][ ch ][ item ]) {
				DATB_UNLOCK();

printf("datb_get_changed_any ch:%d item:%d (%d)\n", ch, item, bl);

				return ( 1 );
			}
		}
	}
	DATB_UNLOCK();

	return ( 0 );
}

void datb_set_all_changed( const int bl, const int ch )
{
	DATB_CHK_BL( bl, );
	DATB_CHK_CH( ch, );
	DATB_LOCK();
	memset( datb_changed[ bl ][ ch ], 1, sizeof( datb_changed[ bl ][ ch ]));
	DATB_UNLOCK();
}

void datb_reset( const varb_t item, const int bl, const int ch )
{
	DATB_CHK_VAR( item, );
	DATB_CHK_BL( bl, );
	DATB_CHK_CH( ch, );
	DATB_LOCK();
	datb_changed[ bl ][ ch ][ item ] = 0;
	DATB_UNLOCK();
}

void datb_reset_all_bl( const int bl )
{
	DATB_CHK_BL( bl, );
	DATB_LOCK();
    memset( datb_changed[ bl ], 0, sizeof( datb_changed[ bl ]));
	DATB_UNLOCK();
}

uint8_t datb_get_host( const varb_t item, const int bl, const int ch )
{
	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_CHK_CH( ch, 0 );

	return datb_host[ bl ][ ch ][ item ];
}

char *datb_get_host_string( const varb_t item, const int bl, const int ch )
{
	DATB_CHK_VAR( item, 0 );
	DATB_CHK_BL( bl, 0 );
	DATB_CHK_CH( ch, 0 );

	switch ( datb_host[ bl ][ ch ][ item ]) {

#define T0( N ) \
	case 0: return ( "" );
#define T( N ) \
	case N: return __stringify( N );

	UNFOLD( DATB_HOST, T0, T );

#undef T0
#undef T

	}
	return ( "" );
}

char *datb_get_title( const varb_t item )
{
	DATB_CHK_VAR( item, 0 );

	return (( char *) datb_descry[ item ].title );
}

void datb_clear_updated( const int bl )
{
    DATB_CHK_BL( bl, );

	DATB_LOCK();
	__updated[ bl ] = 0;
	DATB_UNLOCK();
}

void datb_set_updated( const int bl )
{
    DATB_CHK_BL( bl, );

	DATB_LOCK();
	__updated[ bl ] = 1;
	DATB_UNLOCK();
}

uint8_t datb_check_updated( int bl )
{
	uint8_t result;

    DATB_CHK_BL( bl, 0 );

	DATB_LOCK();
	result = __updated[ bl ];
	DATB_UNLOCK();

	return ( result );
}

/** \} */
