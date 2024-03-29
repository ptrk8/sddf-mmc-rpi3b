#pragma once

#include "result.h"
#include "csd.h"

/**
 * Returns the C_SIZE from the CSD register.
 * @param csd
 * @param ret_val
 * @return
 */
result_t sdcard_data_v1_get_c_size(csd_t *csd, uint32_t *ret_val);

/**
 * Returns the memory capacity of the SD card.
 * @param csd
 * @param ret_val
 * @return
 */
result_t sdcard_data_v1_get_memory_capacity(csd_t *csd, uint64_t *ret_val);

/**
 * Returns the SD cards's block size.
 * @param csd
 * @param ret_val
 * @return
 */
result_t sdcard_data_v1_get_block_size(csd_t *csd, uint16_t *ret_val);

/**
 * Returns the number of blocks on the SD card.
 * @param csd
 * @param ret_val
 * @return
 */
result_t sdcard_data_v1_get_num_blocks(csd_t *csd, uint64_t *ret_val);