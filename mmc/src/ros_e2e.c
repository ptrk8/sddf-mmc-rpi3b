#include "ros_e2e.h"

result_t ros_e2e_run_all_tests(void) {
    log_info("Starting ros_e2e_run_all_tests().");

    result_t res;

    res = ros_e2e_rcutils_is_file();
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_exists();
    if (result_is_err(res)) return res;

    log_info("Finished ros_e2e_run_all_tests().");
    return result_ok();
}

result_t ros_e2e_rcutils_is_file(void) {
    log_info("Starting ros_e2e_rcutils_is_file().");

    /* Checking that is_file returns false if file does not exist. */
    bool is_file = rcutils_is_file("i_dont_exist.txt");
    assert(false == is_file);

    /* Creating a new file. */
    FRESULT res;
    FATFS fs;
    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        log_info("Error mounting FS with res of %d.", res);
    }
    assert(FR_OK == res);

    FIL fp = {0};
    char const *path = "ros_e2e_rcutils_is_file.txt";
    res = f_open(&fp, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        log_info("Error opening file with res of %d.", res);
    }
    assert(FR_OK == res);

    res = f_close(&fp);
    if (res != FR_OK) {
        log_info("Error closing file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Checking `rcutils` returns true for file that does exist. */
    log_info("Checking `rcutils` returns true for file that does exist.");
    is_file = rcutils_is_file(path);
    assert(true == is_file);

    log_info("Finished ros_e2e_rcutils_is_file().");
    return result_ok();
}

result_t ros_e2e_rcutils_exists(void) {
    log_info("Starting ros_e2e_rcutils_exists().");

    /* Checking that rcutils_exists returns false if file does not exist. */
    bool exists = rcutils_exists("i_dont_exist.txt");
    assert(false == exists);

    /* Creating a new file. */
    FRESULT res;
    FATFS fs;
    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        log_info("Error mounting FS with res of %d.", res);
    }
    assert(FR_OK == res);

    FIL fp = {0};
    char const *path = "ros_e2e_rcutils_exists.txt";
    res = f_open(&fp, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        log_info("Error opening file with res of %d.", res);
    }
    assert(FR_OK == res);

    res = f_close(&fp);
    if (res != FR_OK) {
        log_info("Error closing file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Checking `rcutils` returns true for file that does exist. */
    log_info("Checking `rcutils` returns true for file that does exist.");
    exists = rcutils_exists(path);
    assert(true == exists);

    log_info("Finished ros_e2e_rcutils_exists().");
    return result_ok();
}

