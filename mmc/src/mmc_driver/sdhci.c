#include "sdhci.h"

result_t sdhci_get_sd_clock_divisor(
        bcm_emmc_regs_t *bcm_emmc_regs,
        uint32_t freq,
        uint32_t *ret_val
) {
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_get_sd_clock_divisor().");
    }
    if (freq == 0) {
        return result_err("Zero `freq` passed to sdhci_get_sd_clock_divisor().");
    }
    /* The Pi's SD frequency is always 41.66667Mhz when running bare metal. */
    uint32_t divisor = (41666667 + freq - 1) / freq;
    /* Constrain divisor to max of 0x3FF. */
    if (divisor > 0x3FF) {
        divisor = 0x3FF;
    }
    /* Obtain the controller's Host Controller Spec Version. */
    uint8_t host_controller_spec_version = 0;
    result_t res = bcm_emmc_regs_get_host_controller_spec_version(
            bcm_emmc_regs,
            &host_controller_spec_version
    );
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to get host controller spec version in sdhci_get_sd_clock_divisor().");
    }
    /* If the Host Controller Spec Version is 1 or 2, we're in "8-bit Divided
     * Clock Mode". */
    if (host_controller_spec_version < 2) {
        uint_fast8_t shiftcount = arith_find_most_sig_bit_set(divisor);
        /* Note the offset of shift by 1 (look at the spec) */
        if (shiftcount > 0) shiftcount--;
        /* It's only 8 bits maximum on HOST_SPEC_V2 */
        if (shiftcount > 7) shiftcount = 7;
        /* Version 1 and 2 take power 2 */
        divisor = ((uint32_t) 1 << shiftcount);
    }
    /* TODO: Check this. */
    else if (divisor < 3) {
        divisor = 4;
    }
    *ret_val = divisor;
    return result_ok();
}

result_t sdhci_set_sd_clock(bcm_emmc_regs_t *bcm_emmc_regs, uint32_t freq) {
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_set_sd_clock().");
    }
    size_t retries_busy = 10000;
    bool cmd_or_data_lines_busy = false;
    do {
        usleep(1);
        bool data_lines_busy = false;
        result_t res = bcm_emmc_regs_is_data_lines_busy(
                bcm_emmc_regs,
                &data_lines_busy
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to check if data lines are busy in sdhci_set_sd_clock().");
        }
        bool cmd_line_busy = false;
        res = bcm_emmc_regs_is_cmd_line_busy(
                bcm_emmc_regs,
                &cmd_line_busy
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to check if cmd line is busy in sdhci_set_sd_clock().");
        }
        cmd_or_data_lines_busy = data_lines_busy || cmd_line_busy;
    } while(cmd_or_data_lines_busy && (retries_busy-- > 0));
    if (cmd_or_data_lines_busy) {
        return result_err("Timed out waiting for data/cmd lines to be free in sdhci_set_sd_clock().");
    }
    /* Disable SD clock. */
    result_t res = bcm_emmc_regs_disable_sd_clock(bcm_emmc_regs);
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to disable SD clock in sdhci_set_sd_clock().");
    }
    usleep(10);
    /* Get the SD clock divisor we want to set. */
    uint32_t sd_clock_divisor = 0;
    res = sdhci_get_sd_clock_divisor(
            bcm_emmc_regs,
            freq,
            &sd_clock_divisor
    );
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to get SD clock divisor in sdhci_set_sd_clock().");
    }
    /* Set SD Clock Mode to "Divided". */
    res = bcm_emmc_regs_set_sd_clock_mode_as_divided(bcm_emmc_regs);
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to set SD clock mode to \"Divided\" in sdhci_set_sd_clock().");
    }
    /* Set SD Clock Divisor. */
    res = bcm_emmc_regs_set_sd_clock_divisor(
            bcm_emmc_regs,
            sd_clock_divisor
    );
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to set SD clock divisor in sdhci_set_sd_clock().");
    }
    usleep(10);
    /* Enable the SD Clock. */
    res = bcm_emmc_regs_enable_sd_clock(bcm_emmc_regs);
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to enable SD clock in sdhci_set_sd_clock().");
    }
    log_trace("Setting SD clock to %uHz (divisor: %u).", freq, sd_clock_divisor);
    /* Wait for the SD Clock to stabilise. The SDHCI specifies timeout as 150
     * milliseconds (150,000 microseconds) */
    size_t retries_stable = 15000;
    bool is_sd_clock_stable = false;
    do {
        usleep(10);
        res = bcm_emmc_regs_is_sd_clock_stable(
                bcm_emmc_regs,
                &is_sd_clock_stable
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to check if SD clock is stable in sdhci_set_sd_clock().");
        }
    } while (!is_sd_clock_stable && (retries_stable-- > 0));
    if (!is_sd_clock_stable) {
        return result_err("Timed out waiting for SD clock to be stable in sdhci_set_sd_clock().");
    }
    log_trace("SD clock is stable.");
    return result_ok();
}

result_t sdhci_wait_for_interrupt(
        bcm_emmc_regs_t *bcm_emmc_regs,
        uint32_t interrupt_mask,
        sdhci_result_t *sdhci_result
) {
    *sdhci_result = SD_ERROR;
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_wait_for_interrupt().");
    }
    uint32_t mask_with_error = interrupt_mask | INT_ERROR_MASK;
    /* Wait for the interrupt. We specify a timeout of 1 second. */
    size_t retries = 100000;
    bool is_finished_or_error = false;
    do {
        usleep(10);
        result_t res = bcm_emmc_regs_mask_interrupt(
                bcm_emmc_regs,
                mask_with_error,
                &is_finished_or_error
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to wait for interrupt in sdhci_wait_for_interrupt().");
        }
    } while (!is_finished_or_error && (retries-- > 0));
    /* Read interrupt. */
    uint32_t interrupt_raw32 = 0;
    result_t res_get_raw32 = bcm_emmc_regs_get_interrupt_raw32(
            bcm_emmc_regs,
            &interrupt_raw32
    );
    if (result_is_err(res_get_raw32)) {
        return result_err_chain(res_get_raw32, "Failed to get `interrupt` in sdhci_wait_for_interrupt().");
    }
    /* Timeout case. */
    if (!is_finished_or_error) {
        *sdhci_result = SD_TIMEOUT;
        bool is_cmd_timeout = false;
        result_t res = bcm_emmc_regs_is_cmd_timeout_err(
                bcm_emmc_regs,
                &is_cmd_timeout
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to get `interrupt.CTO_ERR` in sdhci_wait_for_interrupt().");
        }
        bool is_data_timeout = false;
        res = bcm_emmc_regs_is_data_timeout_err(
                bcm_emmc_regs,
                &is_data_timeout
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to get `interrupt.DTO_ERR` in sdhci_wait_for_interrupt().");
        }
        /* Clear the interrupt register. */
        res = bcm_emmc_regs_set_interrupt_raw32(
                bcm_emmc_regs,
                interrupt_raw32
        );
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to clear `interrupt` in sdhci_wait_for_interrupt().");
        }
        /* Return error depending on the type of timeout. */
        if (is_cmd_timeout) {
            return result_err("Command timeout error in sdhci_wait_for_interrupt().");
        }
        if (is_data_timeout) {
            return result_err("Data timeout error in sdhci_wait_for_interrupt().");
        }
        return result_err("Timed out waiting for interrupt in sdhci_wait_for_interrupt().");
    }
    /* Error case. */
    bool is_error = false;
    result_t res = bcm_emmc_regs_mask_interrupt(bcm_emmc_regs, INT_ERROR_MASK, &is_error);
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to check for error in sdhci_wait_for_interrupt().");
    }
    if (is_error) {
        /* Clear the interrupt register. */
        res = bcm_emmc_regs_set_interrupt_raw32(bcm_emmc_regs, interrupt_raw32);
        if (result_is_err(res)) {
            return result_err_chain(res, "Failed to clear `interrupt` in sdhci_wait_for_interrupt().");
        }
        return result_err("Error interrupt in sdhci_wait_for_interrupt().");
    }
    /* Clear the interrupt register. */
    res = bcm_emmc_regs_set_interrupt_raw32(bcm_emmc_regs, interrupt_mask);
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to clear `interrupt` in sdhci_wait_for_interrupt().");
    }
    *sdhci_result = SD_OK;
    /* Success case. */
    return result_ok();
}

result_t sdhci_wait_for_cmd_in_progress(
        bcm_emmc_regs_t *bcm_emmc_regs,
        sdhci_result_t *sdhci_result
) {
    *sdhci_result = SD_ERROR;
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_wait_for_cmd_in_progress().");
    }
    bool cmd_in_progress = false;
    bool has_any_err = false;
    size_t retries = 100000;
    do {
        usleep(10);
        result_t res_cmd = bcm_emmc_regs_is_cmd_in_progress(
                bcm_emmc_regs,
                &cmd_in_progress
        );
        if (result_is_err(res_cmd)) {
            return result_err_chain(res_cmd, "Failed to check if command is in progress in sdhci_wait_for_cmd_in_progress().");
        }
        result_t res_err = bcm_emmc_regs_is_any_err(
                bcm_emmc_regs,
                &has_any_err
        );
        if (result_is_err(res_err)) {
            return result_err_chain(res_err, "Failed to check if any error occurred in sdhci_wait_for_cmd_in_progress().");
        }
    } while (cmd_in_progress && !has_any_err && (retries-- > 0));
    if (has_any_err) {
        return result_err("Error occurred in sdhci_wait_for_cmd_in_progress().");
    }
    if (cmd_in_progress) {
        *sdhci_result = SD_BUSY;
        return result_err("Timed out waiting for command in sdhci_wait_for_cmd_in_progress().");
    }
    *sdhci_result = SD_OK;
    return result_ok();
}

result_t sdhci_wait_for_data_in_progress(
        bcm_emmc_regs_t *bcm_emmc_regs,
        sdhci_result_t *sdhci_result
) {
    *sdhci_result = SD_ERROR;
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_wait_for_data_in_progress().");
    }
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_wait_for_data_in_progress().");
    }
    bool data_in_progress = false;
    bool has_any_err = false;
    size_t retries = 100000;
    do {
        usleep(10);
        result_t res_cmd = bcm_emmc_regs_is_data_in_progress(
                bcm_emmc_regs,
                &data_in_progress
        );
        if (result_is_err(res_cmd)) {
            return result_err_chain(res_cmd, "Failed to check if data is in progress in sdhci_wait_for_data_in_progress().");
        }
        result_t res_err = bcm_emmc_regs_is_any_err(
                bcm_emmc_regs,
                &has_any_err
        );
        if (result_is_err(res_err)) {
            return result_err_chain(res_err, "Failed to check if any error occurred in sdhci_wait_for_data_in_progress().");
        }
    } while (data_in_progress && !has_any_err && (retries-- > 0));
    if (has_any_err) {
        return result_err("Error occurred in sdhci_wait_for_data_in_progress().");
    }
    if (data_in_progress) {
        *sdhci_result = SD_BUSY;
        return result_err("Timed out waiting for data in sdhci_wait_for_data_in_progress().");
    }
    *sdhci_result = SD_OK;
    return result_ok();
}

#define R1_ERRORS_MASK       0xfff9c004

result_t sdhci_send_cmd(
        bcm_emmc_regs_t *bcm_emmc_regs,
        size_t sdhci_cmd_index,
        uint32_t arg,
        sdhci_result_t *sdhci_result
) {
    *sdhci_result = SD_ERROR;
    if (bcm_emmc_regs == NULL) {
        return result_err("NULL `bcm_emmc_regs` passed to sdhci_send_cmd().");
    }
    /* Check whether command is an App Command. */
    bool is_app_cmd = false;
    result_t res_is_app_cmd = sdhci_cmds_is_app_cmd(
            sdhci_cmd_index,
            &is_app_cmd
    );
    if (result_is_err(res_is_app_cmd)) {
        return result_err_chain(res_is_app_cmd, "Failed to check if command is an app command in sdhci_send_cmd().");
    }
    /* Obtain the command. */
    sdhci_cmd_t *sdhci_cmd = NULL;
    result_t res = sdhci_cmds_get_cmd(
            sdhci_cmd_index,
            &sdhci_cmd
    );
    if (result_is_err(res)) {
        return result_err_chain(res, "Failed to get command in sdhci_send_cmd().");
    }

    if (is_app_cmd) {
        /* Recursively call ourselves first. */
        result_t res_send_cmd = sdhci_send_cmd(
                bcm_emmc_regs,
                IX_APP_CMD,
                arg,
                sdhci_result
        );
        if (result_is_err(res_send_cmd)) {
            return result_err_chain(res_send_cmd, "Failed to send app command in sdhci_send_cmd().");
        }
    }

    /* Wait for command in progress. */
    result_t res_wait = sdhci_wait_for_cmd_in_progress(bcm_emmc_regs, sdhci_result);
    if (result_is_err(res_wait)) {
        return result_err_chain(res_wait, "Failed to wait for command in progress in sdhci_send_cmd().");
    }

    /* Clear interrupt flags. */
    result_t res_interrupt = bcm_emmc_regs_clear_interrupt(bcm_emmc_regs);
    if (result_is_err(res_interrupt)) {
        return result_err_chain(res_interrupt, "Failed to clear interrupt flags in sdhci_send_cmd().");
    }

    /* Set the argument register first. */
    result_t res_arg1 = bcm_emmc_regs_set_arg1(
            bcm_emmc_regs,
            arg
    );
    if (result_is_err(res_arg1)) {
        return result_err_chain(res_arg1, "Failed to set argument in sdhci_send_cmd().");
    }
    /* Get the command register value stored in `sdhci_cmd`. */
    cmdtm_t cmdtm;
    result_t res_get_cmdtm = sdhci_cmd_get_cmdtm(
            sdhci_cmd,
            &cmdtm
    );
    if (result_is_err(res_get_cmdtm)) {
        return result_err_chain(res_get_cmdtm, "Failed to get cmdtm in sdhci_send_cmd().");
    }
    /* Set the command register to the value obtained from `sdhci_cmd`. */
    result_t res_set_cmdtm = bcm_emmc_regs_set_cmdtm(
            bcm_emmc_regs,
            cmdtm
    );
    if (result_is_err(res_set_cmdtm)) {
        return result_err_chain(res_set_cmdtm, "Failed to set cmdtm in sdhci_send_cmd().");
    }
    /* Obtain the delay from the command. */
    size_t delay_us = 0;
    result_t res_get_delay = sdhci_cmd_get_delay(
            sdhci_cmd,
            &delay_us
    );
    if (result_is_err(res_get_delay)) {
        return result_err_chain(res_get_delay, "Failed to get delay in sdhci_send_cmd().");
    }
    /* Wait for the delay. */
    if (delay_us) {
        usleep(delay_us);
    }

    /* Wait until command complete interrupt */
    result_t res_wait_done = sdhci_wait_for_interrupt(
            bcm_emmc_regs,
            INT_CMD_DONE,
            sdhci_result
    );
    if (result_is_err(res_wait_done)) {
        return result_err_chain(res_wait_done, "Failed to wait for command complete interrupt in sdhci_send_cmd().");
    }

    /* Get the response from `resp0`. */
    uint32_t resp0;
    result_t res_get_resp0 = bcm_emmc_regs_get_resp0(
            bcm_emmc_regs,
            &resp0
    );
    if (result_is_err(res_get_resp0)) {
        return result_err_chain(res_get_resp0, "Failed to get resp0 in sdhci_send_cmd().");
    }

    /* Get the SDHCI command's response type. */
    cmd_rspns_type_t cmd_rspns_type;
    result_t res_rspns = sdhci_cmd_get_cmd_rspns_type(
            sdhci_cmd,
            &cmd_rspns_type
    );
    if (result_is_err(res_rspns)) {
        return result_err_chain(res_rspns, "Failed to get cmd_rspns_type in sdhci_send_cmd().");
    }
    /* Handle response depending on the SDHCI command's response type. */
    switch (cmd_rspns_type) {
        case CMD_NO_RESP: {
            *sdhci_result = SD_OK;
            return result_ok();
        }
        case CMD_BUSY48BIT_RESP: {
            *sdhci_result = resp0 & R1_ERRORS_MASK;
            break;
        }
        case CMD_48BIT_RESP: {
            /* Obtain the command index. */
            cmd_index_t cmd_index;
            result_t res_get_cmd_index = sdhci_cmd_get_cmd_index(
                    sdhci_cmd,
                    &cmd_index
            );
            if (result_is_err(res_get_cmd_index)) {
                return result_err_chain(res_get_cmd_index, "Failed to get cmd_index in sdhci_send_cmd().");
            }
            switch (cmd_index) {
                case 0x03:

                    break;
                case 0x08:
                    /* This is the switch-case for `IX_SEND_IF_COND`. RESP0 contains
                     * voltage acceptance and check pattern, which should match
                     * the argument. */
                    if (resp0 == arg) {
                        *sdhci_result = SD_OK;
                        return result_ok();
                    } else {
                        *sdhci_result = SD_ERROR;
                        return result_err("Response from SD card does not match argument in sdhci_send_cmd().");
                    }
                case 0x29:

                    break;
                default:

                    break;
            }
            break;
        }
        case CMD_136BIT_RESP: {

            *sdhci_result = SD_OK;
            break;
        }
    }

    return result_err("Response not processed by sdhci_send_cmd().");
}




