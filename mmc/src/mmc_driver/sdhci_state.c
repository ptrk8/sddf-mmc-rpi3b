#include "sdhci_state.h"

result_t sdhci_state_init(
        sdhci_state_t *sdhci_state
) {
    if (sdhci_state == NULL) {
        return result_err("NULL `sdhci_state` passed to sdhci_state_init().");
    }
    sdhci_state->is_waiting_on_irq = false;
    return result_ok();
}

result_t sdhci_state_is_waiting_on_irq(sdhci_state_t *sdhci_state, bool *ret_val) {
    if (sdhci_state == NULL) {
        return result_err("NULL `sdhci_state` passed to sdhci_state_is_waiting_on_irq().");
    }
    *ret_val = sdhci_state->is_waiting_on_irq;
    return result_ok();
}

result_t sdhci_state_set_is_waiting_on_irq(sdhci_state_t *sdhci_state, bool val) {
    if (sdhci_state == NULL) {
        return result_err("NULL `sdhci_state` passed to sdhci_state_set_is_waiting_on_irq().");
    }
    sdhci_state->is_waiting_on_irq = val;
    return result_ok();
}
