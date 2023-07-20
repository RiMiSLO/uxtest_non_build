/*   _______ ___ ___      _______ _______ _______ _______      _______
    |   |   |   |   |  _ |_     _|    ___|     __|_     _| _  |__     |
    |   |   |-     -| |_|  |   | |    ___|__     | |   |  |_| |     __|
    |_______|___|___|      |___| |_______|_______| |___|      |_______|
 *//**
 *  \file  const.h
 *  \brief  Константы.
 *  \author  Vitaly Kravtsov (vitaliy.kravtsov@gmail.com)
 *  \copyright  See the LICENSE file.
 */

#ifndef CONST_H
#define CONST_H

/**
 *  \addtogroup app
 *  \{
 */

/**
  *  \brief Младший адрес устройства. \hideinitializer
  */
#define CAN_ADDR_MIN            1

/**
 *  \brief Младший адрес устройства в ЗИП. \hideinitializer
 */
#define CAN_ADDR_STANDBY_MIN    110

/**
 *  \brief Старший адрес устройства в ЗИП. \hideinitializer
 */
#define CAN_ADDR_STANDBY_MAX    119

/**
 * \brief Старший адрес устройства. \hideinitializer
 */
#define CAN_ADDR_MAX            127

/**
 * \brief Адрес устройства в резерве. \hideinitializer
 */
#define CAN_ADDR_RESERVE        CAN_ADDR_MAX

/**
 *  \brief Максимальное количество блоков. \hideinitializer
 */
#define BL_LIMIT                64

/**
 *  \brief Максимальное количество блоков для одного хоста. \hideinitializer
 */
#define BL_LIMIT_PER_HOST       32

/**
 *  \brief Количество каналов блоков. \hideinitializer
 */
#define BL_CH_COUNT             COUNT_CHANAL_UPO

/**
 *  \brief Количество хостов. \hideinitializer
 */
#define BL_HOST_COUNT           3

/**
 *  \brief Максимальное количество групп блоков. \hideinitializer
 */
#define GR_LIMIT                2


#define FILE_PASSPORT           "passport.dat"
#define FILE_ST_LIST            "MRMOList.dat"
#define FILE_BL_LIST            "UPO_Init.dat"

/**
 *  \brief Максимальное количество РЗУ. \hideinitializer
 */
#define RZU_LIMIT               16

/** \} */

#endif /* CONST_H */
