#pragma once

#include <stdbool.h>

#include "result.h"

typedef struct sdhci_state sdhci_state_t;
struct sdhci_state {
    bool is_waiting_on_irq;
};

/**
 * Initializes `sdhci_state`.
 * @param sdhci_state
 * @return
 */
result_t sdhci_state_init(
        sdhci_state_t *sdhci_state
);

/*
 * Returns True if waiting on IRQ and False otherwise.
 * @param sdhci_state
 * @param ret_val
 * @return
 */
result_t sdhci_state_is_waiting_on_irq(
        sdhci_state_t *sdhci_state,
        bool *ret_val
);

/**
 * Sets the `is_waiting_on_irq` field of `sdhci_state` to `val`.
 * @param sdhci_state
 * @param val
 * @return
 */
result_t sdhci_state_set_is_waiting_on_irq(
        sdhci_state_t *sdhci_state,
        bool val
);

