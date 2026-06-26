#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Length of an OpenHaystack / Find My advertisement key (the P-224 public key X
// coordinate) in bytes.
#define APPLE_FIND_MY_KEY_LEN  28

// Length of the BLE random static address derived from the key.
#define APPLE_FIND_MY_ADDR_LEN 6

// Length of the raw Find My BLE advertisement payload.
#define APPLE_FIND_MY_ADV_LEN  31

/**
 * Derive the 6-byte BLE random static address from a Find My advertisement key.
 *
 * Matches OpenHaystack / Macless-Haystack: addr[0] = key[0] | 0xC0 (top two bits
 * forced to 1 to mark a random static address), addr[1..5] = key[1..5].
 *
 * Pure logic (no BLE stack), so it is unit-tested on the host.
 */
void apple_find_my_set_address(uint8_t addr[APPLE_FIND_MY_ADDR_LEN],
                               const uint8_t key[APPLE_FIND_MY_KEY_LEN]);

/**
 * Build the 31-byte Find My advertisement payload from a Find My advertisement key.
 *
 * Layout (per OpenHaystack / Macless-Haystack):
 *   [0]      0x1E              total length (30)
 *   [1]      0xFF              manufacturer-specific data
 *   [2..3]   0x4C 0x00         Apple company ID
 *   [4]      0x12              offline-finding type
 *   [5]      0x19              payload length (25)
 *   [6]      0x00             status byte
 *   [7..28]  key[6..27]        22 key bytes
 *   [29]     key[0] >> 6       top two bits of key[0]
 *   [30]     0x00              hint byte
 */
void apple_find_my_build_adv(uint8_t adv[APPLE_FIND_MY_ADV_LEN],
                             const uint8_t key[APPLE_FIND_MY_KEY_LEN]);

#ifdef __cplusplus
}
#endif
