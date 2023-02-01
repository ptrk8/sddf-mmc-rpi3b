#include <stdint.h>
#include <sel4cp.h>
#include "mmc.h"
#include "uart.h"
#include "sd.h"

/* Used for transporting chars from `uart` to `serial_client`. */
uintptr_t mmc_to_serial_client_putchar_buf;

/* Base virtual address for the GPIO registers. We've deliberately set this
 * value to 0x3f200000 in `mmc.system`, which is the Physical Address the GPIO
 * registers are actually mapped to. */
uintptr_t gpio_base_vaddr;

/* Base virtual address for the Timer registers. We've deliberately set this
 * value to 0x3f003000 in `mmc.system`, which is the Physical Address the Timer
 * registers are actually mapped to. */
uintptr_t timer_base_vaddr;

/* Base virtual address for the EMMC registers. We've deliberately set this
 * value to 0x3f300000 in `mmc.system`, which is the Physical Address the EMMC
 * registers are actually mapped to. */
uintptr_t emmc_base_vaddr;

void init(void) {
    /* Initialise the UART. */
    uart_init(
            mmc_to_serial_client_putchar_buf,
            MMC_TO_SERIAL_CLIENT_PUTCHAR_CHANNEL
    );
    uart_puts("Successfully initialised UART in MMC PD.\n");

    if (sd_init() == SD_OK) {
        uart_puts("Successfully initialised SD card.\n");
    } else {
        uart_puts("Failed to initialise SD card.\n");
    }
}

void notified(sel4cp_channel ch) {

}
