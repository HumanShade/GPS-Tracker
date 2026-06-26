#include "apple_find_my.h"

#include <string.h>

void apple_find_my_set_address(uint8_t addr[APPLE_FIND_MY_ADDR_LEN],
                               const uint8_t key[APPLE_FIND_MY_KEY_LEN])
{
    addr[0] = key[0] | 0xC0u;  // mark as BLE random static address
    addr[1] = key[1];
    addr[2] = key[2];
    addr[3] = key[3];
    addr[4] = key[4];
    addr[5] = key[5];
}

void apple_find_my_build_adv(uint8_t adv[APPLE_FIND_MY_ADV_LEN],
                             const uint8_t key[APPLE_FIND_MY_KEY_LEN])
{
    adv[0] = 0x1E;  // length: 30 bytes follow
    adv[1] = 0xFF;  // manufacturer-specific data
    adv[2] = 0x4C;  // Apple company ID (LSB)
    adv[3] = 0x00;  // Apple company ID (MSB)
    adv[4] = 0x12;  // offline-finding type
    adv[5] = 0x19;  // offline-finding payload length (25)
    adv[6] = 0x00;  // status byte

    memcpy(&adv[7], &key[6], 22);  // key bytes 6..27

    adv[29] = key[0] >> 6;  // top two bits of key[0]
    adv[30] = 0x00;         // hint byte
}
