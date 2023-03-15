#pragma once

#include <stdint.h>

#include "result.h"
#include "gpio_mode.h"
#include "bcm_gpio_regs.h"

#define MAX_GPIO_NUM (54) /* GPIO 0..53 are valid */

/**
 * Sets up the GPIO pin.
 * @param bcm_gpio_regs_t
 * @param gpio
 * @param pin_no
 * @return
 */
result_t gpio_driver_setup_pin(
        bcm_gpio_regs_t *bcm_gpio_regs,
        uint8_t pin_no,
        gpio_mode_t gpio_mode
);



