#pragma once

#include <stdint.h>

#include "result.h"
#include "sleep.h"
#include "assert.h"
#include "clock.h"
#include "log.h"
#include "sdhci.h"
#include "bcm_emmc_regs.h"
#include "sdcard.h"

/**
 * Tests if sleep is working.
 * @return
 */
result_t mmc_driver_e2e_sleep();

/**
 * Simple read/write test on the SD card.
 * @return
 */
result_t mmc_driver_e2e_read_write_simple(
        bcm_emmc_regs_t *bcm_emmc_regs,
        sdcard_t *sdcard
);

/**
 * Read/write test on the SD card.
 * @return
 * @param bcm_emmc_regs
 * @param sdcard
 * @return
 */
result_t mmc_driver_e2e_read_write_multiple_blocks(
        bcm_emmc_regs_t *bcm_emmc_regs,
        sdcard_t *sdcard
);

/**
 * Tests the SD card CSD register.
 * @param sdcard
 * @return
 */
result_t mmc_driver_e2e_sdcard_card_specific_data(sdcard_t *sdcard);

