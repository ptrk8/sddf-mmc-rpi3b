/**
 * Represents the BCM2835 EMMC Registers as documented in Section 5 of the
 * BCM2835 ARM Peripherals manual.
 */

#pragma once

#include <stdint.h>

#include "result.h"
#include "control0.h"
#include "control1.h"

typedef struct bcm_emmc_regs bcm_emmc_regs_t;
struct __attribute__((__packed__, aligned(4))) bcm_emmc_regs {
    uint32_t arg2; /* ACMD23 Argument. */
    uint32_t blksizecnt; /* Block Size and Count. */
    uint32_t arg1; /* Argument. */
    uint32_t cmdtm; /* Command and Transfer Mode. */
    uint32_t resp0; /* Response 0. */
    uint32_t resp1; /* Response 1. */
    uint32_t resp2; /* Response 2. */
    uint32_t resp3; /* Response 3. */
    uint32_t data; /* Data. */
    uint32_t status; /* Status. */
    control0_t control0; /* Control 0. */
    control1_t control1; /* Control 1. */
    uint32_t interrupt; /* Interrupt. */
    uint32_t irpt_mask; /* Interrupt Mask. */
    uint32_t irpt_en; /* Interrupt Enable. */
    uint32_t control2; /* Control 2. */
    uint8_t undocumented0[0x50 - 0x40]; /* Undocumented portion of the Register Map. */
    uint32_t force_irpt; /* Force Interrupt. */
    uint8_t undocumented1[0x70 - 0x54]; /* Undocumented portion of the Register Map. */
    uint32_t boot_timeout; /* Boot Timeout. */
    uint32_t dbg_sel; /* Debug Select. */
    uint8_t undocumented2[0x80 - 0x78]; /* Undocumented portion of the Register Map. */
    uint32_t exrdfifo_cfg; /* Extended Read FIFO Configuration. */
    uint32_t exrdfifo_en; /* Extended Read FIFO Enable. */
    uint32_t tune_step; /* Tuning Step. */
    uint32_t tune_steps_std; /* Tuning Steps Standard. */
    uint32_t tune_steps_ddr; /* Tuning Steps DDR. */
    uint8_t undocumented3[0xf0 - 0x94]; /* Undocumented portion of the Register Map. */
    uint32_t spi_int_spt; /* SPI Interrupt Support. */
    uint8_t undocumented4[0xfc - 0xf4]; /* Undocumented portion of the Register Map. */
    uint32_t slot_isr_ver; /* Slot Interrupt Status and Version. */
};

/**
 * Zeros out the `control0` register.
 * @param bcm_emmc_regs
 */
result_t bcm_emmc_regs_zero_control0(bcm_emmc_regs_t *bcm_emmc_regs);

/**
 * Zeros out the `control1` register.
 * @param bcm_emmc_regs
 */
result_t bcm_emmc_regs_zero_control1(bcm_emmc_regs_t *bcm_emmc_regs);

/**
 * Resets the complete host circuit.
 * @param bcm_emmc_regs
 * @return
 */
result_t bcm_emmc_regs_reset_host_circuit(bcm_emmc_regs_t *bcm_emmc_regs);

/**
 * Checks if the host circuit is in reset.
 * @param bcm_emmc_regs
 * @param ret_val
 * @return
 */
result_t bcm_emmc_regs_is_host_circuit_reset(bcm_emmc_regs_t *bcm_emmc_regs, bool *ret_val);

/**
 * Sets the data timeout to the maximum value.
 * @param bcm_emmc_regs
 * @return
 */
result_t bcm_emmc_regs_set_max_data_timeout(bcm_emmc_regs_t *bcm_emmc_regs);

/**
 * Enables the internal clock.
 * @param bcm_emmc_regs
 * @return
 */
result_t bcm_emmc_regs_enable_internal_clock(bcm_emmc_regs_t *bcm_emmc_regs);


