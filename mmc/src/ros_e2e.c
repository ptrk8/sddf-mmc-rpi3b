#include "ros_e2e.h"

result_t ros_e2e_run_all_tests(void) {
    log_info("Starting ros_e2e_run_all_tests().");

    result_t res = ros_e2e_rcutils_is_file();
    if (result_is_err(res)) {
        return result_err_chain(res, "ros_e2e_rcutils_is_file() failed");
    }

    log_info("Finished ros_e2e_run_all_tests().");
    return result_ok();
}

result_t ros_e2e_rcutils_is_file(void) {
    log_info("Starting ros_e2e_rcutils_is_file().");

    bool is_file = rcutils_is_file("/i_dont_exist.txt");
    assert(false == is_file);

    log_info("Finished ros_e2e_rcutils_is_file().");
    return result_ok();
}
