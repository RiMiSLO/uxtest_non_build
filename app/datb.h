/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datb.h
 *  \brief  ������ ��������� ����� ���.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef DATB_H
#define DATB_H

#ifdef  DATB_IMPLEMENT
#define DATB_EXT
#define DATB_EXT_INIT( dec, init ) \
	dec = init
#else
#define DATB_EXT extern
#define DATB_EXT_INIT( dec, init ) \
	extern dec
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \defgroup datb ������ ��������� ����� ���
 *  \ingroup app
 *
 *  <b>DATB_VAR( var, pos, mask, title, vote, red, string )</b> - ������� ������� ��������� �����.
 *  \param var ��� ��������.
 *  \param pos ������� � ���� ���������.
 *  \param mask ����� �������� � ���� ���������.
 *  \param title �������� ��������.
 *  \param vote ������� �����������.
 *  \param red ������� ������.
 *  \param string ������� �������������� � ���������� ����.
 *
 *  <b>DATB_VAR3( var, pos, mask, offset, vote )</b> - ������� ������� ��������� ����� (��������������).
 *  \param var ��� ��������.
 *  \param pos ������� � ���� ���������.
 *  \param mask ����� �������� � ���� ��������� (0 - �� ���������� �������).
 *  \param offset �������� �� ������ ������.
 *  \param vote ������� �����������.
 *
 *  \{
 */

typedef int varb_t;
typedef uint8_t valb_t;

/** \cond */

#define DATB_VAR( var, pos, mask, title, vote, red, string ) \
	varb_##var,
#define DATB_VAR3( var, pos, mask, offset, vote ) \
	varb_##var,

/**
 *  \brief �������������� ���������.
 */
enum {
#include "datb-dict.h"
	varb_COUNT
};

#undef DATB_VAR
#undef DATB_VAR3

/** \endcond */

/**
 *  \brief ��������� ��������� �������� ��������.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 */
DATB_EXT int datb_get_changed( const varb_t item, const int bl, const int ch );

/**
 *  \brief ��������� ��������� �������� �������� ���� �� �� ������ �� �������.
 *  \param item �������.
 *  \param bl ����� �����.
 */
DATB_EXT int datb_get_changed_bl( const varb_t item, const int bl );

/**
 *  \brief ��������� ��������� �������� ���� �� ������ �� ���������.
 *  \param bl ����� �����.
 */
DATB_EXT int datb_get_changed_any( const int bl );

/**
 *  \brief ���������� ������� ��������� �������� ���� ��������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 */
DATB_EXT void datb_set_all_changed( const int bl, const int ch );

/**
 *  \brief �������� ������� ��������� �������� ��������.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 */
DATB_EXT void datb_reset( const varb_t item, const int bl, const int ch );

/**
 *  \brief �������� ������� ��������� �������� ���� ��������� �����.
 *  \param bl ����� �����.
 */
DATB_EXT void datb_reset_all_bl( const int bl );

/**
 *  \brief ���������� �������� ��������.
 *  \param item �������.
 *  \param value ��������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 *  \param host ����� �����.
 */
DATB_EXT void datb_set( const varb_t item, valb_t value, const int bl, const int ch, const uint8_t host );

/**
 *  \brief �������� "����������" �������� (��������� �� �������� ���������� �� ���� ��) ��������.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 *  \param host ����� �����.
 *  \return ��������;
 */
DATB_EXT valb_t datb_get( const varb_t item, const int bl, const int ch, const uint8_t host );

/**
 *  \brief �������� "����������" �������� �������� � ��������� ����.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 *  \param [out]col ����� ��� �����.
 */
DATB_EXT char *datb_get_string( const varb_t item, const int bl, const int ch, int *col );

/**
 *  \brief �������� "������������" �������� ��������.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param vote ������� ����������� - varb_t vote( value_1, value_2, value_3 ),
 *  ����������� "������������" �������� �� ��������� ���������.
 */
DATB_EXT valb_t datb_get_bl( const varb_t item, const int bl, valb_t ( *vote )( valb_t, valb_t, valb_t ));

/**
 *  \brief �������� ����� �� "�����������" �������� ��������.\n
 *  0: ��������� ��
 *  1: ��1\n
 *  2: ��2\n
 *  3: ��3\n
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 */
DATB_EXT uint8_t datb_get_host( const varb_t item, const int bl, const int ch );

/**
 *  \brief �������� ����� �� "�����������" �������� �������� � ��������� ����.
 *  \param item �������.
 *  \param bl ����� �����.
 *  \param ch ����� ������.
 */
DATB_EXT char *datb_get_host_string( const varb_t item, const int bl, const int ch );

/**
 *  \brief �������� �������� ��������.
 *  \param item �������.
 */
DATB_EXT char *datb_get_title( const varb_t item );

DATB_EXT void datb_clear_updated( const int bl );
DATB_EXT void datb_set_updated( const int bl );
DATB_EXT uint8_t datb_check_updated( int bl );

/** \} */

#ifdef __cplusplus
}
#endif

#endif /* DATB_H */
