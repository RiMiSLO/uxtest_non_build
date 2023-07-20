/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  datn-desc.c
 *  \brief  Форматы данных массива состояния станций.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

//#define DATN_DESC_IMPLEMENT
//#include "define.h"
//#include "datu-dict.h"

//byte bcd2byte( byte bcd )
//{
//    return (( bcd & 0x0F ) + 10 * (( bcd & 0xF0 ) >> 4 ));
//}

//word bcd2word( word bcd )
//{
//    return ( bcd2byte( LOW_BYTE( bcd )) + 100 * bcd2byte( HIGH_BYTE( bcd )));
//}

//byte byte2bcd( byte bin )
//{
//    return ((( bin / 10 ) << 4 ) + ( bin % 10 ));
//}

//word word2bcd( word bin )
//{
//    return ((( word ) byte2bcd(( byte )( bin / 100 )) << 8 ) + byte2bcd(( byte )( bin % 100 )));
//}

//dword bcd2sec( dword bcd )
//{
//    return ( bcd2word( TIME_GET( bcd, H )) * 60 * 60 + bcd2byte( TIME_GET( bcd, M )) * 60 + bcd2byte( TIME_GET( bcd, S )));
//}

//dword sec2bcd( dword bin )
//{
//    dword rem = bin % ( 60 * 60 );   /* Остаток от часа */

//    return ( TIME_SET( H, word2bcd(( word )( bin / ( 60 * 60 ))))
//           | TIME_SET( M, byte2bcd(( byte )( rem / 60 )))
//           | TIME_SET( S, byte2bcd(( byte )( rem % 60 )))
//           );
//}
