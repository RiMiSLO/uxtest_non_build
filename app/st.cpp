﻿/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  st.cpp
 *  \brief  Чтение конфигурации рабочих мест.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#define ST_IMPLEMENT
#include "include.h"
#include <QSettings>
#include <QDirIterator>
#include <QDebug>


/**
 *  \addtogroup st
 *  \{
 */

/**
 *  \brief Количество групп блоков.
 */
static uint8_t gr_count = 0;

/**
 *  \brief Контрольные суммы программ блоков.
 */
static uint32_t map_app[ MAP_HEX_COUNT ];

int st_list_check( QString fn )
{
	FILE *f;
	LISTMRMO st;
	int i;
	char s[ 256 ];
	int c = 0;
	int result = 1;

	/* Открыть файл mrmolist.dat */
    encode_utf8( s, &fn, sizeof( s ));
	f = fopen( s, "rb" );
	if ( f == NULL ) {
        encode_utf8( s, &fn, sizeof( s ));
		ERROR( file_open, s, strerror( errno ));

		return ( 0 );
	}
	/* Чтение */
	while ( fread( &st, sizeof( LISTMRMO ), 1, f )) {
		strncpy(( char* ) st_list[ c ].NameMRMO, ( char* ) st.NameMRMO, MAX_MRMO_NAMELEN );
		st_list[ c ].byNetNumsMask = st.byNetNumsMask;
		/* Сверка */
        if ( st.TypeMRMO == MI_TYPE ) {
            if (( c < MI_ID_MIN ) || ( c > MI_ID_MAX )) {
				/* Несовпадение данных */
				ERROR( st_list_data );
				result = 0;
			}
		} else {
            encode_cp1251( s, &st_list_ex[ c ].name, sizeof( s )); // было utf8
//printf("\nST=%s\n", s);
			if (( strcmp( s, ( const char* ) st.NameMRMO ) != 0 )
             || ( st_list[ c ].TypeMRMO != st.TypeMRMO )
             || ( st_list[ c ].byChannel != st.byChannel )) {
				/* Несовпадение данных */
				ERROR( st_list_data );
				result = 0;
			}
		}
		if ( ++c == st_count ) {
			/* Считано достаточно записей */
			if ( fread( &i, 1, 1, f )) {
				/* НЕ конец файла */
				result = 0;
				ERROR( st_list_len );
			}
			break;
		}
	}
	/* Считано НЕ достаточно записей */
	if ( c != st_count ) {
		result = 0;
		ERROR( st_list_len );
	}
	/* Проверить ошибку чтения */
	if ( ferror( f )) {
		result = 0;
		ERROR( st_list_read );
	}

	fclose( f );

	return ( result );
}

#if 0

int st_list_write( void )
{
	FILE *f;
	int i, ii;
	char s[ 256 ];

	/* Запись файла mrmolist.dat */
	f = fopen( FILE_ST_LIST, "wb" );
	if ( f == NULL ) {
		ERROR( file_open, FILE_ST_LIST, strerror( errno ));

		return ( 0 );
	}
	fwrite( st_list, sizeof( LISTMRMO ), st_count, f );
	fclose( f );

	/* Запись файла upo_init.dat */
	f = fopen( FILE_BL_LIST, "w" );
	if ( f == NULL ) {
		ERROR( file_open, FILE_BL_LIST, strerror( errno ));

		return ( 0 );
	}

	for ( i = 0; i < gr_count; i++ ) {
		int cnt = (( gr_list[ i ].bl_count ) ? gr_list[ i ].bl_count : gr_list[ i ].bl_limit );

		fprintf( f, "[%s\t%s\t%s]\n"
		, st_list[ gr_list[ i ].st[ 0 ]].NameMRMO
		, st_list[ gr_list[ i ].st[ 1 ]].NameMRMO
		, st_list[ gr_list[ i ].st[ 2 ]].NameMRMO
		);
		for ( ii = 0; ii < cnt; ii++ ) {
			uint8_t bl = gr_list[ i ].bl[ ii ];
			if ( bl == BL_LIMIT ) continue;

            encode_cp1251( s, &bl_list[ bl ].name, sizeof( s ));
			fprintf( f, "%d\t%s\t%s\n"
			, bl_list[ bl ].addr
			, s
			, BL_TYPE[ bl_list[ bl ].type ]
			);
		}
	}
	fclose( f );

	return ( 1 );
}
#endif

int bl_list_read( QString fn )
{
	FILE *f;
	QString bl_name;
	char s[ 256 ];
    char s1[ 256 ];
    char s2[ 256 ];
	char line[ 128 ];
    char _name[ 64 ];
	int i, ii;
	int addr = 0;
	int prev = 0;
	bool cap = true;
	uint8_t gr = gr_count;
	int result = 1;

	/* Открыть файл upo_init.dat */
    encode_utf8( s, &fn, sizeof( s ));
	f = fopen( s, "r" );
	if ( f == NULL ) {
        encode_utf8( s, &fn, sizeof( s ));
		ERROR( file_open, s, strerror( errno ));

		return ( 0 );
	}
	/* Чтение построчно */
	while ( fgets( line, sizeof( line ), f ) != NULL ) {
		switch ( cap ) {
		/* Описание блока */
		case false:
			addr = 0;
			*_name = '\0';
			sscanf( line, "%d %s", &addr, _name );
			if ( addr > 0 ) {
				/* Одноканальное описание системы - используем только первый канал */
				if ( addr == prev ) continue;
				prev = addr;

				/* Поиск блока */
				for ( i = 0; i < bl_count; i++ ) {
					if (( bl_list[ i ].addr == addr ) && ( bl_list[ i ].gr == gr )) {
						gr_list[ gr ].bl[ gr_list[ gr ].bl_count ] = i;
						break;
					}
				}
				if ( i == bl_count ) {
					/* Ошибка - Блок не найден! */
					gr_list[ gr ].bl[ gr_list[ gr ].bl_count ] = BL_LIMIT;
					ERROR( bl_list_addr, addr, gr );
				} else {
					/* Проверка имени блока - [К1_]ВКО_1 */
					bl_name = bl_list[ i ].name;
					bl_name.replace( "  ", "_" );
					bl_name.replace( " ", "_" );
                    encode_cp1251( s, &bl_name, sizeof( s ));
encode_utf8(s1, &bl_name, sizeof( s1 ));
//printf("\nblock=%s <> %s\n", s1, _name);

                    if (( strcmp( s, _name ) != 0 ) && (( strlen( _name ) < 4 ) || ( strcmp( s, _name + 3 ) != 0 ))) {
						/* Ошибка - Несовпадение имени! */
						/* gr_list[ gr ].bl[ gr_list[ gr ].bl_count ] = BL_LIMIT; */
                    //    encode_console( s, &bl_name, sizeof( s ));
                        encode_utf8(s, &bl_name, sizeof( s ));
						ERROR( bl_list_name, addr, s );
					}
				}
				++gr_list[ gr ].bl_count;
				/* Следующий блок */
				continue;
			}
			/* fallthrough */

		/* Заголовок группы */
		case true:
			/* Разбор заголовка */
			if ( *line != '[' ) {
				/* Ошибка - Неверный формат заголовка! */
				ERROR( bl_list_gr );
				break;
			}
			char *p = strchr( line + 1, ']' );
			if ( p ) *p = '\0';
			/* Поиск группы */
			for ( i = 0; i < gr_count; i++ ) {
				for ( ii = 0; ii < BL_HOST_COUNT; ii++ ) {
					if ( strcmp(( const char *) st_list[ gr_list[ i ].st[ ii ]].NameMRMO, line + 1 ) == 0 ) {
						gr = i;
						gr_list[ gr ].bl_count = 0;
						break;
					}
				}
				if ( ii != BL_HOST_COUNT ) break;
			}
			if ( i == gr_count ) {
				/* Ошибка - Группа не найдена! */
				ERROR( bl_list_gr );
				break;
			}
			cap = false;
			/* Первый блок */
			continue;
		}
		/* Ошибка */
		result = 0;
        break;
	}
	fclose( f );

	return ( result );
}

void map_read( void )
{
	int i;
	char s[ 256 ];
    uint8_t gr, ch, addr, a, type, ty;
	QByteArray b;

	if ( ini_net_count > NET_COUNT ) ini_net_count = NET_COUNT;
    st_tst = ST_TST_DEFAULT;
	st_host_mask = 0;
	st_count = 0;
	bl_count = 0;
    gr_count = 0;
    mcp_count = 0;
    page_count = 1;
	memset( st_list, 0, sizeof( st_list ));
	memset( gr_list, 0, sizeof( gr_list ));
    memset( bl_chsum, 0, sizeof( bl_chsum ));


	/* upo_prog.ini */
	QSettings ini_upo( ini_at_home( encode_system( s, &ini_file_upo_prog, sizeof( s ))), QSettings::IniFormat );
	ini_upo.setIniCodec( "UTF-8" );

	ini_upo.beginGroup( "hex" );
	/* Число hex файлов */
	map_hex_count = ini_upo.value( "count", 0 ).toInt();
	/* Имена hex файлов */
	for ( i = 0; i < map_hex_count; i++ ) {
		sprintf( s, "%d", i + 1 );
		map_hex[ i ] = ini_upo.value( s, "" ).toString();
	}
	ini_upo.endGroup();

	ini_upo.beginGroup( "crc" );
	memset( map_app, 0, sizeof( map_app ));
	/* Контрольные суммы */
	for ( i = 0; i < map_hex_count; i++ ) {
		bool ok;
		sprintf( s, "%d", i + 1 );
        map_app[ i ] = ( uint32_t ) ini_upo.value( s, "" ).toString().toUInt( &ok, 0 );
	}
	ini_upo.endGroup();

	/* mi.ini */
	QSettings ini_mi( ini_at_home( encode_system( s, &ini_file_mi, sizeof( s ))), QSettings::IniFormat );
	ini_mi.setIniCodec( "UTF-8" );
	ini_mi.beginGroup( "sn" );

	/* map.ini */
    QSettings ini( ini_at_home( encode_system( s, &ini_file_map, sizeof( s ))), QSettings::IniFormat ); // класс берет файл в 1 парам файл 2парам формат
	ini.setIniCodec( "UTF-8" );
    printf(__CRLF__ "Map file:%s",( encode_system( s, &ini_file_map, sizeof( s ))) ) ; // see file map.ini
	/* Число страниц и элементов схемы */
	ini.beginGroup( "map" );
	map_count = ini.value( "count", 0 ).toInt();
	page_count = ini.value( "page_count", 1 ).toInt();
	if ( page_count > PAGE_COUNT ) page_count = PAGE_COUNT;
	ini.endGroup();

	/* Страницы */
	ini.beginGroup( "page" );
	for ( i = 0; i < page_count; i++ ) {
		sprintf( s, "%d", i + 1 );
		page_list[ i ].name = ini.value( s, "" ).toString();
		page_list[ i ].first = 0;
	}
	ini.endGroup();

    for ( i = 0; i < map_count; i++ ) {
		size_t sz;
		int kind;
        sprintf( s, "%d", i + 1 );
		ini.beginGroup( s );

		/* Общие данные */
		kind = ini.value( "kind", MAP_NONE ).toInt();
		map[ i ].pic  = ini.value( "pic", 0 ).toInt();
		map[ i ].pico = ini.value( "pico", 0 ).toInt();
		map[ i ].pice = ini.value( "pice", 0 ).toInt();
		map[ i ].picw = ini.value( "picw", 0 ).toInt();
        map[ i ].picp = ini.value( "picp", 0 ).toInt();
        map[ i ].x    = ini.value( "x", 0 ).toInt();
        map[ i ].y    = ini.value( "y", 0 ).toInt();
		map[ i ].perx = ini.value( "perx", 0 ).toInt();
		map[ i ].br   = ini.value( "br", 0 ).toInt();
		map[ i ].mark = ini.value( ( kind == MAP_IMAGE ) ? "name" : "mark", "" ).toString();
        map[ i ].mask = ini.value("mask", 0 ).toInt();
        map[ i ].position = ini.value("position", 0 ).toInt();
        map[ i ].pic  = ini.value( "pic", 0 ).toInt();
        map[ i ].type = ini.value("type", 0 ).toInt();
        map[ i ].label_x    = ini.value( "label_x", 0 ).toInt();
        map[ i ].label_y    = ini.value( "label_y", 0 ).toInt();


		switch ( kind ) {


		case MAP_ST:  /* Станция */
			/* Позиция в mrmolist.dat */
            addr = ini.value( "addr", DATN_DEPTH ).toInt();
			if ( addr >= DATN_DEPTH ) {
				/* Недопустимый адрес станции */
				ERROR( map_addr, addr );
				break;
            }
			if ( st_list_ex[ addr ].name != "" ) {
				/* Не уникальный адрес узла */
				ERROR( map_addr, addr );
				break;
            }
            st_list_ex[ addr ].name = ini.value( "name", "" ).toString();
			b = st_list_ex[ addr ].name.toUtf8();
			sz = b.size();
			if ( sz ) memcpy( st_list_ex[ addr ].name_ch, b.data(), sz );
			st_list_ex[ addr ].name_ch[ sz ] = '\0';
			st_list[ addr ].TypeMRMO = ini.value( "type", 0 ).toInt();
			ch = ini.value( "ch", 0 ).toInt();
			st_list[ addr ].byChannel = ( ch ) ? ch - 1 : 0;
			st_list[ addr ].byNetNumsMask = 0x03;  /* По умолчанию первые две */
			st_list_ex[ addr ].gr = GR_LIMIT;

			switch ( st_list[ addr ].TypeMRMO ) {

			case MRMOKPA:
				st_tst = addr;
                //map[ i ].kind = MAP_NONE;
                map[ i ].kind = MAP_ST;
				break;

			case MRMOY:
			case MRMOY_KPA:
				st_host_mask |= BIT( addr );
				/* fallthrough */

			case IOCONT:
				/* Группа подключения блоков */
				gr = ini.value( "gr", 0 ).toInt();
				if (( gr >= 1 ) && ( gr <= GR_LIMIT ) && ( ch >= 1 ) && ( ch <= BL_HOST_COUNT )) {
					if ( gr_count < gr ) gr_count = gr;
					gr_list[ --gr ].st[ --ch ] = addr;
					st_list_ex[ addr ].gr = gr;
				}
				/* fallthrough */

			case MRMOT:
			case SSBVS:
				map[ i ].kind = MAP_ST;
				break;
			}
			if ( st_count <= addr ) st_count = addr + 1;
			map[ i ].ind = addr;
			st_list_ex[ addr ].ind = i;
			break;

		case MAP_BL:  /* Блок */
			if ( bl_count == BL_LIMIT ) {
				/* Слишком много блоков */
				ERROR( _BL_LIMIT );
				break;
			}
			bl_list[ bl_count ].name = ini.value( "name", "" ).toString();
			b = bl_list[ bl_count ].name.toUtf8();
			sz = b.size();
			if ( sz ) memcpy( bl_list[ bl_count ].name_ch, b.data(), sz );
			bl_list[ bl_count ].name_ch[ sz ] = '\0';
			type = ini.value( "type", 0 ).toInt();
			bl_list[ bl_count ].type = type;
			bl_list[ bl_count ].addr = ini.value( "addr", 0 ).toInt();
			bl_list[ bl_count ].rs   = ini.value( "rs", 0 ).toInt();
			bl_list[ bl_count ].hex  = ini.value( "hex", type + 1 ).toInt() - 1;
			bl_list[ bl_count ].app  = map_app[ bl_list[ bl_count ].hex ];
			gr = ini.value( "gr", 0 ).toInt();

			/* Группа подключения блоков */
			if (( gr >= 1 ) && ( gr <= GR_LIMIT )) {
				if ( gr_count < gr ) gr_count = gr;
				bl_list[ bl_count ].gr = --gr;
				gr_list[ gr ].bl[ gr_list[ gr ].bl_limit++ ] = bl_count;
			} else {
				bl_list[ bl_count ].gr = GR_LIMIT;
			}
			map[ i ].kind = MAP_BL;
			map[ i ].ind = bl_count;
			bl_list[ bl_count ].ind = i;

			++bl_count;
			break;

        case MAP_MCP:
            addr = ini.value( "addr", DATM_DEPTH ).toInt();
            if (( addr < 4 ) || (addr > 6 )) {   //( addr > MCP_ID_MAX )) {
                // Недопустимый адрес МЦП
                ERROR( map_addr, addr );
                break;
            }
            ty = addr - 4;

            if ( st_list_ex[ addr ].name != "" ) {
                // Не уникальный адрес узла
                ERROR( map_addr, addr );
                break;
            }

            st_list[ addr ].TypeMRMO = MCP_TYPE;
            //st_list_ex1[ addr ].gr = GR_LIMIT;
            st_list_ex[ addr ].name = ini.value( "name", "МЦП" ).toString();
            mcp_list[ ty ].name = st_list_ex[ addr ].name;
            mcp_list[ ty ].addr = addr+1;
            b = mcp_list[ ty ].name.toUtf8();

            sz = b.size();
            if ( sz ) memcpy( mcp_list[ ty ].name_ch, b.data(), sz );
            mcp_list[ ty ].name_ch[ sz ] = '\0';
            strcpy( st_list_ex[ addr ].name_ch, mcp_list[ ty ].name_ch );
            if ( mcp_count <= ty ) mcp_count = ty + 1;
            //st_count++;
            map[ i ].ind = ty;
            map[ i ].kind = MAP_MCP;
            mcp_list[ ty ].ind = i;

 printf("MCP(%d): %d / %d \n", i, map[i].x, map[i].y);
            break;


        case MAP_MI:  // МИ
            if (mcp_count==0){
                break;
            }
            addr = ini.value( "addr", DATM_DEPTH ).toInt();
            if (( addr < MI_ID_MIN ) || ( addr > MI_ID_MAX )) {
                // Недопустимый адрес МИ
                ERROR( map_addr, addr );
                break;
            }
/*
            if ( st_list_ex[ addr ].name != "" ) {
                // Не уникальный адрес узла
                ERROR( map_addr, addr );
                break;
            }
            st_list[ addr ].TypeMRMO = MI_TYPE;
            st_list_ex[ addr ].gr = GR_LIMIT;
            st_list_ex[ addr ].name = ini.value( "name", "МИ" ).toString();
*/
            a = addr - MI_ID_MIN;
            if ( mcp_count == 1){
                gr_mi_count1++;
                if ( gr_mi_count1>6){
                    gr_mi_count1--;
                    ERROR( map_addr, addr);
                    break;
                }
            }
            else
            if ( mcp_count == 2){
                gr_mi_count2++;
                if ( gr_mi_count2>6){
                    gr_mi_count2--;
                    ERROR( map_addr, addr);

                    break;
                }
            }
            else
            if ( mcp_count == 3){
                gr_mi_count3++;
                if ( gr_mi_count3>6){
                    gr_mi_count3--;
                    ERROR( map_addr, addr);

                    break;
                }
            }
            else{
               continue;
            }
            mi_list[ a ].name = ini.value( "name", "МИ" ).toString(); //st_list_ex[ addr ].name;
			b = mi_list[ a ].name.toUtf8();
			sz = b.size();
			if ( sz ) memcpy( mi_list[ a ].name_ch, b.data(), sz );
			mi_list[ a ].name_ch[ sz ] = '\0';
/*
			strcpy( st_list_ex[ addr ].name_ch, mi_list[ a ].name_ch );
*/
			sprintf( s, "%d", addr );
    //		mi_list[ a ].sn = ini_mi.value( s, 0 ).toInt();
            if ( mi_count <= a ) mi_count = a + 1;
			map[ i ].ind = a;
			map[ i ].kind = MAP_MI;
            mi_list[ a ].ind = i;

printf("MI(%d): %d / %d addr %d \n", i, map[i].x, map[i].y, addr);
			break;

		case MAP_IMAGE:  /* Статическое изображение */
			map[ i ].ind = 0;
			map[ i ].kind = MAP_IMAGE;

printf("IMAGE(%d): %d / %d \n", i, map[i].x, map[i].y);

			break;

        case MAP_UPS:  /* UPS */
            addr = ini.value( "addr", DATN_DEPTH ).toInt();
            if ( addr >= 255 ) {
                /* Недопустимый адрес станции */
                ERROR( map_addr, addr );
                break;
            }
            if ( st_list_ex1[ addr ].name != "" ) {
                /* Не уникальный адрес узла */
                ERROR( map_addr, addr );
                break;
            }

            st_list_ex1[ addr ].name = ini.value( "name", "" ).toString();

            b = st_list_ex1[ addr ].name.toUtf8();
            sz = b.size();
            if ( sz ) memcpy( st_list_ex1[ addr ].name_ch, b.data(), sz );
            st_list_ex1[ addr ].name_ch[ sz ] = '\0';
            st_list1[ addr ].TypeMRMO = ini.value( "type", 0 ).toInt();
            ch = ini.value( "ch", 0 ).toInt();
            st_list1[ addr ].byChannel = ( ch ) ? ch - 1 : 0;
            st_list1[ addr ].byNetNumsMask = 0x03;  /* По умолчанию первые две */
            st_list_ex1[ addr ].gr = GR_LIMIT;
            map[ i ].kind = MAP_UPS;
            if ( st_count1 <= addr ) st_count1 = addr + 1;
            map[ i ].ind = addr;
            st_list_ex1[ addr ].ind = i;
            up_list[i].mask=ini.value("mask",0).toInt();
            ups_count++;
            break;

		default:
			map[ i ].kind = MAP_NONE;
            ERROR( map_kind, i );
		}
		ini.endGroup();
	}
	/* Проверка st_list */
	for ( i = 0; i < st_count; i++ ) {
		if ( st_list_ex[ i ].name == "" ) {
			/* Нет станции */
			st_list[ i ].TypeMRMO = MRMOKPA;
			st_list_ex[ i ].gr = GR_LIMIT;
			ERROR( map_st_list, i );
		}
	}
	/* Проверка st_tst */
	if ( st_tst == ST_TST_DEFAULT ) {
		/* Нет ПКФ */
		ERROR( map_st_tst );
	}

	/* Число картинок */
	ini.beginGroup( "pic" );
	map_pic_count = ini.value( "count", 0 ).toInt();
	/* Имена файлов картинок */
	for ( i = 0; i < map_pic_count; i++ ) {
		sprintf( s, "%d", i + 1 );
		map_pic[ i ] = ini.value( s, "" ).toString();
	}
    ini.endGroup();
    ini_mi.endGroup();

}

void mi_comp_read( void )
{
	FILE *f;
	char s[ 256 ];

	mi_comp_count = 0;
	memset( mi_comp, 0, sizeof( mi_comp ));

    f = fopen( ini_at_home( encode_utf8( s, &ini_file_mi_comp, sizeof( s ))), "rb" );
	if ( f != NULL ) {
		/* Чтение файла коррект. кодов */
        while ( fread( mi_comp + mi_comp_count, sizeof( MI_COMP ), 1, f )) {

			for ( int ii = 0; ii < mi_count; ii++ ) {
                if ( mi_list[ ii ].sn == mi_comp[ mi_comp_count ].wSn ) {
					mi_list[ ii ].verify_c = mi_comp[ mi_comp_count ].wCSComp;
					break;
                }
			}

            ++mi_comp_count;
        }

        fclose( f );
	} else {  /* Нет файла */
        ERROR( file_open, ini_at_home( encode_console( s, &ini_file_mi_comp, sizeof( s ))), strerror( errno ));
	}
}


void map_free( void )
{
	map_count = 0;
	page_count = 0;
	map_pic_count = 0;
	map_hex_count = 0;
}

#include <dirent.h>

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//////////////////////////////////////////////////
#endif

void passport_read( QString dir )
{
	FILE *f;
	QString fn = dir + "/" FILE_PASSPORT;
	char s[ 256 ];

	/* Открыть файл passport.dat */
//	encode_system( s, &fn, sizeof( s ));
//	f = fopen( s, "rb" );
    f = fopen( fn.toUtf8(), "rb" );
    rzu_error = 4;
	if ( f != NULL ) {
		if ( fread( rzu_passport + rzu_count, sizeof( PASSPROT ), 1, f )) {
			rzu_dir[ rzu_count ] = dir;
			++rzu_count;
		} else {
            rzu_error = 5;
            encode_utf8( s, &fn, sizeof( s ));
			ERROR( file_is_empty, s );
		}
		fclose( f );
	} else {
        rzu_error = 6;
        encode_utf8( s, &fn, sizeof( s ));
		ERROR( file_open, s, strerror( errno ));
	}
}

void rzu_read( void )
{
	QDir d( ini_dir_rzu, "", QDir::Name, QDir::Dirs | QDir::NoDotAndDotDot );
	QDirIterator it( d );
	char s[ 256 ];

	rzu_count = 0;
    rzu_error = 1;

	if ( !d.exists()) {
        ERROR( rzu_read, encode_console( s, &ini_dir_rzu, sizeof( s )));

		return;
	}
    rzu_error = 2;
	while ( it.hasNext()) {
		it.next();
		if ( it.fileInfo().isDir()) {
            rzu_error = 3;
			if ( rzu_count >= RZU_LIMIT ) {
                ERROR( _RZU_LIMIT);

				break;
			}
			passport_read( it.fileInfo().absoluteFilePath());
		}
	}
}

uint8_t rzu_search( dword verify )
{
	for ( uint8_t i = 0; i < rzu_count; i++ ) {
		if ( rzu_passport[ i ].Veryfi == verify ) return ( i );
	}
	return ( RZU_LIMIT );
}

static dword varn2test( varn_t varn )
{
	/* XXX
        Исходим из соответствия  NM_TEST порядку тестов в datn-dict.h .
	 */
	int t = varn - varn_test_ + TEST__FIRST_;

	if (( t < TEST__FIRST_ ) || ( t > TEST__LAST_ )) return ( NO_TEST );

	return ( t );
}

void pack_read( void )
{
	varn_t t;
	char sec[ 64 ];
	char par[ 64 ];
	char s[ 256 ];
	QByteArray b;
    QSettings ini( ini_at_home( encode_system( s, &ini_file_pack, sizeof( s ))), QSettings::IniFormat );

	ini.setIniCodec( "UTF-8" );
	memset( pack_list, NO_TEST, sizeof( pack_list ));

    for ( int  i = 0; i < PACK_COUNT; i++ ) {
		sprintf( sec, "%d", i + 1 );
		ini.beginGroup( sec );

		pack_list[ i ].id = i + 1;
		b = ini.value( "title", "" ).toString().toUtf8();
		if ( b.size()) {
			int l = b.size();

			pack_title[ i + 1 ] = ( char *) malloc( l + 1 );
			memcpy( pack_title[ i  + 1 ], b.data(), l );
			pack_title[ i + 1 ][ l ] = 0;
		}
        for ( int ii = 0; ii < TEST_COUNT; ii++ ) {
			/* Чтение тестов */
			sprintf( par, "var_%d", ii + 1 );
			b = ini.value( par, "" ).toString().toLocal8Bit();
			if (( b.size() == 0 ) || ( b.size() >= ( int ) sizeof( s ))) break;
			memcpy( s, b.data(), b.size());
			s[ b.size()] = 0;
            t = datn_atovar( s, varn_test_, varn_test_ + TEST_COUNT - 1 );
			if (( t == varn_COUNT ) || ( t == varn_test_net )) break;
			pack_list[ i ].test[ ii ].wTestNum = varn2test( t );
			sprintf( par, "rep_%d", i + 1 );
			pack_list[ i ].test[ ii ].wRepeat = ini.value( par, 0 ).toInt();
			sprintf( par, "par_%d", i + 1 );
			pack_list[ i ].test[ ii ].wParam[ 0 ] = ini.value( par, 0 ).toInt();
		}
		ini.endGroup();
	}
}

/** \} */
